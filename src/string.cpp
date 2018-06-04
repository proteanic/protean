#include <protean/detail/string.hpp>
#include <protean/detail/hash.hpp>

#include <cstring>
#include <cstdlib>

namespace protean { namespace detail {

    static const boost::uint64_t s_onStackMask    = 0x0000000000000001ull;

    string::string() :
        m_rawData(0)
    {
        setStackFlag();
    }

    string::string(const char* text) :
        m_rawData(0)
    {
        setStackFlag();
        initialise(text, std::strlen(text));
    }

    string::string(const char* text, size_t len) :
        m_rawData(0)
    {
        setStackFlag();
        initialise(text, len);
    }

    string::string( const string& rhs ) :
        m_rawData(rhs.m_rawData)
    {
        if ( !rhs.onStack() ) 
        {
            m_rawData=0;
            initialise(rhs.heapPointer(), rhs.size());
        }
    }

    string::~string()
    {
        if ( !onStack() )
        {
            alignedFree(heapPointer());
        }
    }

    string& string::operator=(const string& rhs)
    {
        string(rhs).swap(*this);
        return *this;
    }

    void string::swap(string& rhs)
    {
        std::swap(m_rawData, rhs.m_rawData);
    }

    const char* string::value() const
    {
        if (onStack())
        {
            return &m_stack[1];
        }
        else
        {
            return heapPointer();
        }
    }

    void string::initialise( const char* value, size_t size )
    {
        if ( size<7 )
        {
			memcpy(&m_stack[1], value, size);
            setStackFlag();
        }
        else
        {
            heapPointer(static_cast<char*>(alignedMalloc(size+1)));
            #ifdef _MSC_VER
            strncpy_s(heapPointer(), size+1, value, _TRUNCATE);
            #else
            // This triggers unpleasant warnings under MSVC, but
            // maybe is is better to define _CRT_SECURE_NO_DEPRECATE
            // than to use two different versions.
            strncpy (heapPointer(), value, size);
            // strncopy will not terminate the string unless it has
            // enough space
            *(heapPointer()+size)=0;
            #endif
        }
    }

    bool string::onStack() const {
        return (m_rawData&s_onStackMask) != 0;
    }

    size_t string::size() const
    {
        return std::strlen(value());
    }

    bool string::empty() const
    {
        return size()==0;
    }

    const char* string::heapPointer() const
    {
        string* mutableThis(const_cast<string*>(this));
        return mutableThis->heapPointer();
    }

    char* string::heapPointer()
    {
        assert(!onStack());
        return m_heapPtr;
    }

    void string::heapPointer(char* ptr)
    {
        m_heapPtr=ptr;
        clearStackFlag();
    }

    void string::setStackFlag()
    {
        m_rawData |= s_onStackMask;
    }

    void string::clearStackFlag()
    {
        m_rawData &= ~s_onStackMask;
    }

    /*static*/ void* string::alignedMalloc(size_t size)
    {
        // malloc is guaranteed to have alignment suitable for any pod type in MSVC (we just need the lsb)
        return std::malloc(size);
    }

    /*static*/ void string::alignedFree(void* ptr)
    {
        std::free(ptr);
    }

    int string::compare(const string& rhs) const
    {
		return strcmp(value(), rhs.value());
    }

    boost::uint64_t string::hash(boost::uint64_t seed) const
    {
        return hash_value(value(), seed);
    }

}} // namespace protean::detail
