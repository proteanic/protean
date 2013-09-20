//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef PROTEAN_DETAIL_DATA_TABLE_COLUMN_HPP
#define PROTEAN_DETAIL_DATA_TABLE_COLUMN_HPP

#include <protean/config.hpp>
#include <protean/variant_error.hpp>
#include <protean/variant_iterator.hpp>
#include <protean/variant_base.hpp>
#include <protean/detail/data_table_variant_iterator.hpp>
#include <protean/detail/data_table_types.hpp>

#include <vector>
#include <string>
#include <boost/preprocessor.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/function.hpp>
#include <boost/any.hpp>
#include <boost/utility.hpp>
#include <boost/throw_exception.hpp>
#include <boost/format.hpp>

#if defined(_MSC_VER)
#    pragma warning(push)
#    pragma warning(disable:4251)
#endif

namespace protean { namespace detail {

    class PROTEAN_DECL data_table_column_base : boost::noncopyable
    {
    /* Construction */
    /****************/
    public:
        data_table_column_base(const std::string& name, variant_base::enum_type_t type) : m_name(name), m_type(type) {}
        virtual ~data_table_column_base() {}

        virtual data_table_column_base* clone() const = 0;
        virtual void resize(size_t n)                 = 0;

    protected:
        data_table_column_base(const data_table_column_base& rhs) : m_name(rhs.m_name), m_type(rhs.m_type) {}
        void operator=(const data_table_column_base&);

    /* Attributes */
    /**************/
    public:
        virtual const std::string& name() const { return m_name; }
        variant_base::enum_type_t type() const  { return m_type; }

    /* Variant interface */
    /*********************/
    public:
        virtual bool empty() const  = 0;
        virtual size_t size() const = 0;
        virtual void clear()        = 0;

    public:
        // Variant iterators
        virtual variant_const_iterator_base* begin() const = 0;
        virtual variant_const_iterator_base* end() const   = 0;
        virtual variant_const_iterator_base* begin()       = 0;
        virtual variant_const_iterator_base* end()         = 0;

    /* Typed method interfaces */
    /***************************/
    public:
        // Because C++ doesn't permit virtual templated methods, dispatch to 'templated' virtual impl using pointer
        template <typename V>
        void push_back(const V& value);

        template <typename V>
        typename std::vector<V>::const_iterator begin() const;

        template <typename V>
        typename std::vector<V>::const_iterator end() const;

        template <typename V>
        typename std::vector<V>::iterator begin();

        template <typename V>
        typename std::vector<V>::iterator end();

    protected:
        // Method implementations
        #define PROTEAN_DETAIL_DATA_TABLE_COLUMN_DECLARE_VIRTUAL_IMPLS(r, data, type)     \
            virtual void push_back_impl(const type& value, type* = 0)             = 0;    \
            virtual std::vector<type>::const_iterator begin_impl(type* = 0) const = 0;    \
            virtual std::vector<type>::const_iterator end_impl(type* = 0) const   = 0;    \
            virtual std::vector<type>::iterator begin_impl(type* = 0)             = 0;    \
            virtual std::vector<type>::iterator end_impl(type* = 0)               = 0;

        BOOST_PP_SEQ_FOR_EACH(PROTEAN_DETAIL_DATA_TABLE_COLUMN_DECLARE_VIRTUAL_IMPLS, , PROTEAN_DETAIL_DATA_TABLE_COLUMN_SUPPORTED_TYPES)

    /* Member variables */
    /********************/
    private:
        const std::string               m_name;
        const variant_base::enum_type_t m_type;
    };

    // Free-standing clone function, required and used by boost::ptr_vector in data_table
    inline data_table_column_base* new_clone(const data_table_column_base& other)
    {
        return other.clone();
    }


    template <typename T>
    class data_table_column : public data_table_column_base
    {
    /* Typedefs */
    /************/
    public:
        typedef std::vector<T>                           container_type;
        typedef typename container_type::iterator        iterator;
        typedef typename container_type::const_iterator  const_iterator;
        typedef typename container_type::difference_type difference_type;

    /* Construction */
    /****************/
    public:
        data_table_column(const std::string& name, variant_base::enum_type_t type, size_t capacity = 0);

        virtual data_table_column_base* clone() const;

        virtual void resize(size_t n);

    /* Variant interface */
    /*********************/
    public:
        virtual bool empty() const;
        virtual size_t size() const;
        virtual void clear();

    public:
        // Variant iterators
        virtual variant_const_iterator_base* begin() const;
        virtual variant_const_iterator_base* end() const;
        virtual variant_const_iterator_base* begin();
        virtual variant_const_iterator_base* end();

    /* Typed method implementations */
    /********************************/
    protected:
        // Typed method implementation template-dispatchers
        #define PROTEAN_DETAIL_DATA_TABLE_COLUMN_DEFINE_VIRTUAL_IMPLS_DECL(r, data, type)    \
            virtual void push_back_impl(const type& value, type* p = 0);                     \
            virtual std::vector<type>::const_iterator begin_impl(type* p = 0) const;         \
            virtual std::vector<type>::const_iterator end_impl(type* p = 0) const;           \
            virtual std::vector<type>::iterator begin_impl(type* p = 0);                     \
            virtual std::vector<type>::iterator end_impl(type* p = 0);

        BOOST_PP_SEQ_FOR_EACH(PROTEAN_DETAIL_DATA_TABLE_COLUMN_DEFINE_VIRTUAL_IMPLS_DECL, , PROTEAN_DETAIL_DATA_TABLE_COLUMN_SUPPORTED_TYPES)

    private:
        // Typed method template implementations

        // void push_back()
        template <typename V>
        void push_back_impl_tmpl(const V& value, typename boost::enable_if<boost::is_same<V, T>, void*>::type = 0);

        template <typename V>
        void push_back_impl_tmpl(const V& value, typename boost::disable_if<boost::is_same<V, T>, void*>::type = 0);

        // const_iterator begin() const
        template <typename V>
        typename std::vector<V>::const_iterator begin_impl_tmpl(typename boost::enable_if<boost::is_same<V, T>, void*>::type = 0) const;

        template <typename V>
        typename std::vector<V>::const_iterator begin_impl_tmpl(typename boost::disable_if<boost::is_same<V, T>, void*>::type = 0) const;

        // const_iterator end() const
        template <typename V>
        typename std::vector<V>::const_iterator end_impl_tmpl(typename boost::enable_if<boost::is_same<V, T>, void*>::type = 0) const;

        template <typename V>
        typename std::vector<V>::const_iterator end_impl_tmpl(typename boost::disable_if<boost::is_same<V, T>, void*>::type = 0) const;

        // iterator begin()
        template <typename V>
        typename std::vector<V>::iterator begin_impl_tmpl(typename boost::enable_if<boost::is_same<V, T>, void*>::type = 0);

        template <typename V>
        typename std::vector<V>::iterator begin_impl_tmpl(typename boost::disable_if<boost::is_same<V, T>, void*>::type = 0);

        // iterator end()
        template <typename V>
        typename std::vector<V>::iterator end_impl_tmpl(typename boost::enable_if<boost::is_same<V, T>, void*>::type = 0);

        template <typename V>
        typename std::vector<V>::iterator end_impl_tmpl(typename boost::disable_if<boost::is_same<V, T>, void*>::type = 0);

    /* Member variables */
    /********************/
    private:
        container_type m_values;
    };


    /* Specialised typedefs used in static data table iterators as types to denote empty tuple entry */
    /*************************************************************************************************/
    template <>
    class data_table_column<boost::tuples::null_type> : public data_table_column_base
    {
    public:
        typedef boost::tuples::null_type iterator;
        typedef boost::tuples::null_type const_iterator;
    };

}} // namespace protean::detail

#include <protean/detail/data_table_column.ipp>


#if defined(_MSC_VER)
#    pragma warning(pop)
#endif

#endif // PROTEAN_DETAIL_DATA_TABLE_COLUMN_HPP
