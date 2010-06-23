//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef PROTEAN_DETAIL_LIST_HPP
#define PROTEAN_DETAIL_LIST_HPP

#include <protean/config.hpp>
#include <protean/detail/sequence.hpp>

#include <vector>

#if defined(_MSC_VER)
#    pragma warning(push)
#    pragma warning(disable:4251)
#endif

namespace protean { namespace detail {
    
    class PROTEAN_DECL list : public sequence
    {
        typedef std::vector<variant> container_t;
    public:
        list(size_t size);
        int compare(const collection& rhs) const;
        boost::uint64_t hash(boost::uint64_t seed) const;
        bool empty() const;
        size_t size() const;
        void clear();

        variant& push_back(const variant &value);
        void pop_back();
        const variant& at(size_t index) const;
        variant& at(size_t index);

        variant_const_iterator_base* begin() const;
        variant_const_iterator_base* end() const;
        variant_iterator_base* begin();
        variant_iterator_base* end();

    private:
        container_t m_value;
    };

}} // namespace protean::detail


#if defined(_MSC_VER)
#    pragma warning(pop)
#endif

#endif // PROTEAN_DETAIL_LIST_HPP
