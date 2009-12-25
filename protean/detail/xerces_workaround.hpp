#include <protean/xml_defs.hpp>

#include <boost/pool/detail/singleton.hpp>

// The way Base64 handles memory has changed in Xerces3, and appears
// to be broken. I posted a question on the xerces list
// http://mail-archives.apache.org/mod_mbox/xerces-c-users/200912.mbox/ajax/%3c951708020912241731m5afa9559s4c1e523e23a103f@mail.gmail.com%3e
// to see if I was missing anything fundamental, but I am probably
// not. The bottom line is that the latest version of Xerces will
// crash when used either in accordance with its documentation (though
// that is difficult), or with the documentation patch at
// https://issues.apache.org/jira/browse/XERCESC-1847?page=com.atlassian.jira.plugin.system.issuetabpanels:all-tabpanel
// I can reproduce the crash reliably under MSVC 9.0 Express in debug
// mode, but not e.g. under Linux.  The workaround for now is to use a
// trivial custom memory manager that just passes the work onto
// new/delete. 
//
// With a bit of luck it should be possible to revert the workaround
// easily when they fix Xerces. The protean version this replaces
// assumed the updated documentation that uses delete. I am happy to
// do that on request. (Karel)

namespace {

class MyMemoryManagerImpl : public xercesc::MemoryManager
{

public:

  MyMemoryManagerImpl () throw () {}

  void * allocate (XMLSize_t size)
  {
    return new char [size];
  }

  void deallocate (void * a)
  {
    delete [] (reinterpret_cast<char *> (a));
  }

  xercesc::MemoryManager * getExceptionMemoryManager ()
  {
    return this;
  }

  ~MyMemoryManagerImpl () throw () {}
};

}

namespace protean {

  inline xercesc::MemoryManager * myMemoryManager ()
  {
    // this will not work, presumably because fgMemoryManager might
    // not be correctly initialised
    // return xercesc::XMLPlatformUtils::fgMemoryManager;

    return &boost::details::pool::singleton_default<MyMemoryManagerImpl>::instance ();
  }

}
