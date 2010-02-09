//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef PROTEAN_DETAIL_STRING_HPP
#define PROTEAN_DETAIL_STRING_HPP

#include <protean/config.hpp>

#include <string>
#include <boost/cstdint.hpp>

namespace protean { namespace detail {

    class PROTEAN_DECL string
    {
    public:
        string();
        string(const char* text);
        string(const char* text, size_t len);
        string(const string& rhs);
        ~string();

        string& operator=(const string& rhs);

        const std::string value() const;
        void initialise(const char* value, size_t size);

        bool onStack() const;

        void swap(string& rhs);

        bool empty() const;

        size_t size() const;
        int compare(const string& rhs) const;
        size_t hash() const;

    private:
        char* heapPointer();
        const char* heapPointer() const;
        void heapPointer(char* ptr);

        void setStackFlag();
        void clearStackFlag();

        void setStackSize(boost::uint8_t sz);
        boost::uint8_t getStackSize() const;

        static void* alignedMalloc(size_t size);
        static void alignedFree(void* ptr);

    private:
        union {
            char* m_heapPtr;
            char m_stack[8];
            boost::uint64_t m_rawData;
        };
    };

}} // namespace protean::detail

#endif // PROTEAN_DETAIL_STRING_HPP
