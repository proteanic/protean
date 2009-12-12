//  (C) Copyright Johan Ditmar, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#include <protean/detail/string.hpp>

#include <boost/functional/hash.hpp>

namespace protean { namespace detail {

    static const boost::uint64_t s_onStackMask    = 0x0000000000000001ull;
    static const boost::uint64_t s_sizeMask        = 0x00000000000000FEull;

    string::string() :
        m_rawData(0)
    {
        setStackFlag();
    }

    string::string(const char* text) :
        m_rawData(0)
    {
        setStackFlag();
        initialise(text, strlen(text));
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

    const std::string string::value() const
    {
        if (onStack())
        {
            size_t sz(size());
            std::string temp(sz, '@');

            boost::uint64_t data(m_rawData);
            for(size_t n(1); n<= sz; ++n)
            {
                data>>=8;
                temp[sz-n]=data&0xFF;
            }

            return temp;
        }
        else
        {
            return std::string(heapPointer());
        }
    }

    void string::initialise( const char* value, size_t size )
    {
        if ( size<8 )
        {
            m_rawData=0;
            for(size_t n(0); n!= size; ++n)
            {
                m_rawData|=value[n];
                m_rawData<<=8;
            }
            setStackSize(static_cast<boost::uint8_t>(size));
            setStackFlag();
        }
        else
        {
            heapPointer(static_cast<char*>(alignedMalloc(size+1)));
            #ifdef MSVC
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

    size_t string::size() const {
        if (onStack())
            return getStackSize();
        else
            return strlen(heapPointer());
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

    void string::setStackSize(boost::uint8_t sz)
    {
        m_rawData &= ~s_sizeMask;
        m_rawData |= (sz << 1);
    }

    boost::uint8_t string::getStackSize() const
    {
        return static_cast<boost::uint8_t>( (m_rawData & s_sizeMask)  >> 1 );
    }


    /*static*/ void* string::alignedMalloc(size_t size)
    {
        // malloc is guaranteed to have alignment suitable for any pod type in MSVC (we just need the lsb)
        return ::malloc(size);
    }

    /*static*/ void string::alignedFree(void* ptr)
    {
        ::free(ptr);
    }

    int string::compare(const string& rhs) const
    {
        return value().compare(rhs.value());
    }

    size_t string::hash() const
    {
        size_t seed = 0;
        boost::hash_combine(seed, value());

        return seed;
    }

}} // namespace protean::detail
