//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef PROTEAN_DETAIL_DATA_TABLE_HPP
#define PROTEAN_DETAIL_DATA_TABLE_HPP

#include <protean/config.hpp>
#include <protean/detail/data_table_column.hpp>
#include <protean/data_table_iterator.hpp>
#include <protean/variant_base.hpp>
#include <protean/detail/string.hpp>

#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/tuple/tuple.hpp>

#if defined(_MSC_VER)
#    pragma warning(push)
#    pragma warning(disable:4251)
#endif

namespace protean { namespace detail {

    class PROTEAN_DECL data_table : public collection
    {
    /* Typedefs */
    /************/
    public:
        typedef boost::ptr_vector<data_table_column_base> column_container_type;

    /* Construction */
    /****************/
    public:
        explicit data_table(size_t capacity = 0);

    /* Column setup */
    /****************/
    public:
        data_table& add_column(variant_base::enum_type_t type);
        data_table& add_column(variant_base::enum_type_t type, const std::string& name);

    private:
        template <variant_base::enum_type_t E>
        data_table& add_column(const std::string& name);

        class column_adder;

    /* Column access */
    /*****************/
    public:
        const data_table_column_base& get_column(const size_t i) const;
        const data_table_column_base& get_column(const std::string& name) const;
        const column_container_type& columns() const;

    private:
        data_table_column_base& get_column(const size_t i);
        data_table_column_base& get_column(const std::string& name);
        column_container_type& columns();

    /* Interface */
    /*************/
    public:
        template <typename Tuple>
        data_table& push_back(const Tuple& value);

    private:
        template <size_t N, typename HT, typename TT>
        void push_back_impl(const boost::tuples::cons<HT, TT>& tuple);
        template <size_t N, typename TT>
        void push_back_impl(const boost::tuples::cons<std::string, TT>& tuple);
        template <size_t N>
        void push_back_impl(const boost::tuples::null_type&);

    /* Collection interface */
    /************************/
    public:
        virtual bool empty() const;
        virtual size_t size() const;
        virtual void clear();
        
        virtual int compare(const collection& rhs) const;
        virtual boost::uint64_t hash(boost::uint64_t seed) const;

        // Variant iterators
        virtual variant_const_iterator_base* begin() const;
        virtual variant_const_iterator_base* end() const;
        virtual variant_iterator_base* begin();
        virtual variant_iterator_base* end();

    /* Typed iterators */
    /*******************/
    public:
        #define BEGIN_ITERATOR(z, n, t)                                                              \
            template <BOOST_PP_ENUM_PARAMS(BOOST_PP_ADD(n, 1), variant_base::enum_type_t t)>         \
            data_table_iterator<BOOST_PP_ENUM_PARAMS(BOOST_PP_ADD(n, 1), t)> begin();

        #define END_ITERATOR(z, n, t)                                                                \
            template <BOOST_PP_ENUM_PARAMS(BOOST_PP_ADD(n, 1), variant_base::enum_type_t t)>         \
            data_table_iterator<BOOST_PP_ENUM_PARAMS(BOOST_PP_ADD(n, 1), t)> end();

        #define BEGIN_CONST_ITERATOR(z, n, t)                                                        \
            template <BOOST_PP_ENUM_PARAMS(BOOST_PP_ADD(n, 1), variant_base::enum_type_t t)>         \
            data_table_const_iterator<BOOST_PP_ENUM_PARAMS(BOOST_PP_ADD(n, 1), t)> begin() const;

        #define END_CONST_ITERATOR(z, n, t)                                                          \
            template <BOOST_PP_ENUM_PARAMS(BOOST_PP_ADD(n, 1), variant_base::enum_type_t t)>         \
            data_table_const_iterator<BOOST_PP_ENUM_PARAMS(BOOST_PP_ADD(n, 1), t)> end() const;

        // data_table_iterator<E0, ..., En> begin();
        BOOST_PP_REPEAT(DATA_TABLE_MAX_COLUMNS, BEGIN_ITERATOR, DATA_TABLE_COLUMN_TYPE_PREFIX)
        // data_table_iterator<E0, ..., En> end();
        BOOST_PP_REPEAT(DATA_TABLE_MAX_COLUMNS, END_ITERATOR, DATA_TABLE_COLUMN_TYPE_PREFIX)
        // data_table_const_iterator<E0, ..., En> begin() const;
        BOOST_PP_REPEAT(DATA_TABLE_MAX_COLUMNS, BEGIN_CONST_ITERATOR, DATA_TABLE_COLUMN_TYPE_PREFIX)
        // data_table_const_iterator<E0, ..., En> end() const;
        BOOST_PP_REPEAT(DATA_TABLE_MAX_COLUMNS, END_CONST_ITERATOR, DATA_TABLE_COLUMN_TYPE_PREFIX)

    /* Friend classes for deserialisation */
    /**************************************/
    private:
        friend class binary_reader;
        friend class xml_default_handler;

    /* Member variables */
    /********************/
    private:
        column_container_type m_columns;
        const size_t          m_capacity;
    };

}} // namespace protean::detail

#include <protean/detail/data_table.ipp>


#if defined(_MSC_VER)
#    pragma warning(pop)
#endif

#endif // PROTEAN_DETAIL_DATA_TABLE_HPP
