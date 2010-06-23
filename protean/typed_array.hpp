//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef PROTEAN_TYPED_ARRAY_HPP
#define PROTEAN_TYPED_ARRAY_HPP

#include <protean/config.hpp>

#include <protean/detail/ref_counted.hpp>
#include <protean/variant_base.hpp>

#include <protean/array_iterator.hpp>

namespace protean {

    class variant;
    class variant_ref;
    class variant_cref;
    
    class PROTEAN_DECL typed_array : public detail::ref_counted
    {
    public:
        typed_array(size_t size, variant_base::enum_type_t type);
        typed_array(size_t size, const variant& init);

        typed_array(const typed_array& rhs);
        typed_array& operator=(const typed_array& rhs);

        ~typed_array();

        size_t size() const;
        variant_base::enum_type_t type() const;

        variant_ref operator[](size_t n);
        variant_cref operator[](size_t n) const;

        int compare(const typed_array& rhs) const;
        boost::uint64_t hash(boost::uint64_t seed) const;

        void swap(typed_array& rhs);

        typedef array_iterator<array_const_iterator_traits> const_iterator;
        typedef array_iterator<array_iterator_traits> iterator;

        const_iterator begin() const;
        const_iterator end() const;
        iterator begin();
        iterator end();

    private:
        variant_base& at(size_t n);
        const variant_base& at(size_t n) const;

    private:
        friend class detail::xml_default_handler;
        friend class binary_reader;
        friend class binary_writer;

    private:
        boost::uint32_t             m_size;
        variant_base*               m_data;
        variant_base::enum_type_t   m_type;
    };

} // namespace protean

#endif // PROTEAN_TYPED_ARRAY_HPP
