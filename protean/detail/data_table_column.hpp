#ifndef PROTEAN_DETAIL_DATA_TABLE_COLUMN_HPP
#define PROTEAN_DETAIL_DATA_TABLE_COLUMN_HPP

#include <protean/config.hpp>
#include <protean/variant_error.hpp>
#include <protean/variant_iterator.hpp>
#include <protean/variant_base.hpp>
#include <protean/detail/data_table_variant_iterator.hpp>
#include <protean/detail/data_table_types.hpp>
#include <protean/data_table_column_base.hpp>

#include <vector>
#include <string>
#include <boost/tuple/tuple.hpp>
#include <boost/throw_exception.hpp>
#include <boost/preprocessor/seq/for_each.hpp>

#if defined(_MSC_VER)
#    pragma warning(push)
#    pragma warning(disable:4251)
#endif

namespace protean { namespace detail {

    /* Used in conjunction with boost::enable(disable)_if and SFINAE for templated virtual dispatch */
    /************************************************************************************************/
    template <variant_base::enum_type_t E, variant_base::enum_type_t F>
    struct is_same_column : public boost::false_type {};

    template <variant_base::enum_type_t E>
    struct is_same_column<E, E> : public boost::true_type {};

    /* Used in conjunction with boost::enable(disable)_if and SFINAE for variant fallback column operations. */
    /* Second template param is unused but needs to be there in order for boost::enable(disable)_if to work. */
    /*********************************************************************************************************/
    template <typename T, typename V>
    struct is_variant : public boost::false_type {};

    template <typename V>
    struct is_variant<variant, V> : public boost::true_type {};


    template <variant_base::enum_type_t E>
    class data_table_column : public data_table_column_base
    {
    /* Typedefs */
    /************/
    public:
        typedef column_traits<E>                 traits;

        typedef typename traits::enum_type       enum_type;
        typedef typename traits::value_type      value_type;

        typedef typename traits::container_type  container_type;
        typedef typename traits::iterator        iterator;
        typedef typename traits::const_iterator  const_iterator;
        typedef typename traits::difference_type difference_type;

    /* Construction */
    /****************/
    public:
        data_table_column(const std::string& name, size_t capacity = 0);

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
        virtual variant_iterator_base* begin();
        virtual variant_iterator_base* end();

    /* Typed method implementations */
    /********************************/
    protected:
        // Typed method implementation template-dispatchers
        #define PROTEAN_DETAIL_DT_COLUMN_VIRTUAL_IMPLS_DECL(r, data, elem)                                                        \
            virtual std::vector<GET_TYPE(elem)>::const_iterator begin_impl(column_traits<GET_ENUM(elem)>::enum_type* = 0) const;  \
            virtual std::vector<GET_TYPE(elem)>::const_iterator end_impl(column_traits<GET_ENUM(elem)>::enum_type* = 0) const;    \
            virtual std::vector<GET_TYPE(elem)>::iterator begin_impl(column_traits<GET_ENUM(elem)>::enum_type* = 0);              \
            virtual std::vector<GET_TYPE(elem)>::iterator end_impl(column_traits<GET_ENUM(elem)>::enum_type* = 0);

        #define PROTEAN_DETAIL_DT_COLUMN_VIRTUAL_PUSH_BACK_IMPL_DECL(r, data, type)  \
            virtual void push_back_impl(const type& value, type* = 0);               \

        virtual void push_back_impl(const variant& value, variant*);

        BOOST_PP_SEQ_FOR_EACH(PROTEAN_DETAIL_DT_COLUMN_VIRTUAL_PUSH_BACK_IMPL_DECL, , SUPPORTED_TYPES)
        BOOST_PP_SEQ_FOR_EACH(PROTEAN_DETAIL_DT_COLUMN_VIRTUAL_IMPLS_DECL, , COLUMN_TYPES)

    private:
        // Typed method template implementations

        // void push_back()
        template <typename V>
        void push_back_impl_tmpl(const V& value, typename boost::enable_if<boost::is_same<V, value_type>, void*>::type = 0);

        template <typename V>
        void push_back_impl_tmpl(const V& value, typename boost::disable_if<boost::is_same<V, value_type>, void*>::type = 0);

        // void push_back(variant)
        template <typename V>
        void push_back_variant_impl_tmpl(const variant& value, typename boost::enable_if<is_variant<value_type, V>, void*>::type = 0);

        template <typename V>
        void push_back_variant_impl_tmpl(const variant& value, typename boost::disable_if<is_variant<value_type, V>, void*>::type = 0);

        // const_iterator begin() const
        template <variant_base::enum_type_t F>
        typename column_traits<F>::const_iterator begin_impl_tmpl(typename boost::enable_if<is_same_column<E, F>, void*>::type p = 0) const;

        template <variant_base::enum_type_t F>
        typename column_traits<F>::const_iterator begin_impl_tmpl(typename boost::disable_if<is_same_column<E, F>, void*>::type p = 0) const;

        // const_iterator end() const
        template <variant_base::enum_type_t F>
        typename column_traits<F>::const_iterator end_impl_tmpl(typename boost::enable_if<is_same_column<E, F>, void*>::type = 0) const;

        template <variant_base::enum_type_t F>
        typename column_traits<F>::const_iterator end_impl_tmpl(typename boost::disable_if<is_same_column<E, F>, void*>::type = 0) const;

        // iterator begin()
        template <variant_base::enum_type_t F>
        typename column_traits<F>::iterator begin_impl_tmpl(typename boost::enable_if<is_same_column<E, F>, void*>::type = 0);

        template <variant_base::enum_type_t F>
        typename column_traits<F>::iterator begin_impl_tmpl(typename boost::disable_if<is_same_column<E, F>, void*>::type = 0);

        // iterator end()
        template <variant_base::enum_type_t F>
        typename column_traits<F>::iterator end_impl_tmpl(typename boost::enable_if<is_same_column<E, F>, void*>::type = 0);

        template <variant_base::enum_type_t F>
        typename column_traits<F>::iterator end_impl_tmpl(typename boost::disable_if<is_same_column<E, F>, void*>::type = 0);

    /* Member variables */
    /********************/
    private:
        container_type m_values;
    };


    /* Specialised typedefs used in static data table iterators as types to denote empty tuple entry */
    /*************************************************************************************************/
    template <>
    class data_table_column<variant_base::None> : public data_table_column_base
    {
    public:
        typedef boost::tuples::null_type iterator;
        typedef boost::tuples::null_type const_iterator;

	private:
		data_table_column(); // Silence compiler warning; is never constructed
    };

}} // namespace protean::detail

#include <protean/detail/data_table_column.ipp>


#if defined(_MSC_VER)
#    pragma warning(pop)
#endif

#endif // PROTEAN_DETAIL_DATA_TABLE_COLUMN_HPP
