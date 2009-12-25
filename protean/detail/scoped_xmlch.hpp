#include <protean/xml_defs.hpp>
#include <boost/utility.hpp>

namespace protean
{

// Some xerces functions (e.g. xercesc::XMLString::transcode) return a
// string that needs to be freed using xercesc::XMLString::release. It
// is easy to find advice on the web that says it is fine to use
// boost::scoped_array for this, but that is incorrect. In particular,
// under Linux it results in a block allocated using (non-array) new
// getting later freed by delete [].

class scoped_xmlch : boost::noncopyable
{

public:

  scoped_xmlch (XMLCh * ch) throw () : ch(ch) {}

  XMLCh * get () throw ()
  {
    return ch;
  }

  ~scoped_xmlch () throw ()
  {
    xercesc::XMLString::release (&ch);
  }

private:

  XMLCh * ch;

};

}
