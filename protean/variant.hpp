//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef PROTEAN_VARIANT_HPP
#define PROTEAN_VARIANT_HPP

#include <protean/config.hpp>
#include <protean/typed_array.hpp>
#include <protean/variant_base.hpp>
#include <protean/variant_ref.hpp>
#include <protean/data_table_iterator.hpp>
#include <protean/data_table_column_base.hpp>

#include <boost/preprocessor/repetition/repeat.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/repetition/enum.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

namespace protean {

    class variant_ref;
    class variant_cref;

    template<typename T>
    class range_array_iterator;

    class PROTEAN_DECL variant : public variant_base
    {
    public:
        typedef boost::gregorian::date              date_t;
        typedef boost::posix_time::time_duration    time_t;
        typedef boost::posix_time::ptime            date_time_t;

        typedef variant                             value_type;
        typedef value_type*                         pointer;
        typedef const value_type*                   const_pointer;
        typedef value_type&                         reference;
        typedef const value_type&                   const_reference;

		typedef boost::ptr_vector<data_table_column_base> column_collection_t;

        typedef variant_iterator<const_iterator_traits> const_iterator;
        typedef variant_iterator<iterator_traits>       iterator;
        typedef const_iterator_traits::difference_type  difference_type;

    public:
        static const date_time_t& min_date_time();
        static const date_time_t& max_date_time();
        static const date_t& min_date();
        static const date_t& max_date();

    /* Construction */
    /****************/
    public:
        variant();
        variant(const variant& value);

        template<typename T>
		explicit variant(T value, typename boost::enable_if<boost::is_arithmetic<T> >::type* dummy = 0);

        explicit variant(enum_type_t type, size_t size=0);
        explicit variant(enum_type_t type, const std::string& arg);
        explicit variant(const std::string& arg);
        explicit variant(const char *arg);
        explicit variant(bool arg);
        explicit variant(const date_t& arg);
        explicit variant(const time_t& arg);
        explicit variant(const date_time_t& arg);
        explicit variant(const void* data, size_t size);
        explicit variant(const exception_data& arg);
        explicit variant(const std::exception& arg);
        explicit variant(const object& arg);
        explicit variant(const typed_array& arg);

        explicit variant(const variant_ref& arg);
        explicit variant(const variant_cref& arg);

    private:
        explicit variant(const handle<object>& arg);
        explicit variant(const handle<typed_array>& arg);

    /* Assignment */
    /**************/
    public:
        variant& operator=(const variant &rhs);

        template<typename T>
        typename boost::enable_if<boost::is_pod<T>, variant&>::type
        operator=(T value);

        template<typename T>
        typename boost::disable_if<boost::is_pod<T>, variant&>::type
        operator=(const T& value);

        void swap(variant& rhs);

        void initialise(enum_type_t type, size_t size = 0);

    /* Destructor */
    /**************/
    public:
        ~variant();

    /* Type inspection */
    /*******************/
    public:
        enum_type_t type() const { return m_type; }

        template<typename T> 
        typename boost::disable_if<boost::is_base_of<object,T>, bool>::type
        is() const;

        template<typename T> 
        typename boost::enable_if<boost::is_base_of<object,T>, bool>::type
        is() const;

        template<int N>
        bool is() const;

    /* Type casting */
    /****************/
    public:

        // as<T*>() -> const T*
        template<typename T>
        struct return_pointer :
            boost::is_pointer<T>::type
        {};

        // as<T>() -> const T&
        template<typename T>
        struct return_reference :
            boost::mpl::or_<
                boost::is_same<exception_data, T>,
                boost::is_same<typed_array, T>,
                boost::is_base_of<object,T>
            >::type
        {};

        // as<T>() -> T
        template<typename T>
        struct return_value :
            boost::mpl::not_<
                boost::mpl::or_<
                    return_pointer<T>,
                    return_reference<T>
                >
            >::type
        {};

        template<typename T>
        typename boost::enable_if<return_value<T>, T>::type
        as() const;

        template<typename T>
        typename boost::enable_if<return_pointer<T>, const typename boost::remove_pointer<T>::type *>::type
        as() const;

        template<typename T>
        typename boost::enable_if<return_reference<T>, const T&>::type
        as() const;

    public:
        template<typename T>
        struct array_range_t
        {
            typedef boost::iterator_range<
                range_array_iterator<T>
            > type;
        };

        template<typename T>
        typename array_range_t<T>::type
        as_range() const;

        // Primitive -> Any
        variant any_cast() const;

        // Number -> T
        template<typename T>
        T numerical_cast() const;

    /* Collection interface */
    /************************/
    public:
        enum enum_return_trait_t {ReturnSelf, ReturnItem};

        const_iterator begin() const;
        const_iterator end() const;
        iterator begin();
        iterator end();

        bool empty() const;
        size_t size() const;
        void clear();

    /* List interface */
    /******************/
    public:
        variant& push_back(const variant& value, enum_return_trait_t ret = ReturnSelf);
        variant& pop_back();

    /* TimeSeries interface */
    /************************/
    public:
        variant& push_back(const date_time_t& time, const variant& value, enum_return_trait_t ret = ReturnSelf);

    /* DataTable interface */
    /***********************/
    public:
        variant& add_column(enum_type_t type);
        variant& add_column(enum_type_t type, const std::string& name);

		const column_collection_t& columns() const;
		column_collection_t& columns();

        template <typename TUPLE>
        variant& push_back(const TUPLE& value);

        #define DATA_TABLE_BEGIN_ITERATOR_DECL(z, n, t)                                            \
            template <BOOST_PP_ENUM_PARAMS(BOOST_PP_ADD(n, 1), enum_type_t t)>                     \
            data_table_iterator<BOOST_PP_ENUM_PARAMS(BOOST_PP_ADD(n, 1), t)> begin();

        #define DATA_TABLE_END_ITERATOR_DECL(z, n, t)                                              \
            template <BOOST_PP_ENUM_PARAMS(BOOST_PP_ADD(n, 1), enum_type_t t)>                     \
            data_table_iterator<BOOST_PP_ENUM_PARAMS(BOOST_PP_ADD(n, 1), t)> end();

        #define DATA_TABLE_BEGIN_CONST_ITERATOR_DECL(z, n, t)                                      \
            template <BOOST_PP_ENUM_PARAMS(BOOST_PP_ADD(n, 1), enum_type_t t)>                     \
            data_table_const_iterator<BOOST_PP_ENUM_PARAMS(BOOST_PP_ADD(n, 1), t)> begin() const;

        #define DATA_TABLE_END_CONST_ITERATOR_DECL(z, n, t)                                        \
            template <BOOST_PP_ENUM_PARAMS(BOOST_PP_ADD(n, 1), enum_type_t t)>                     \
            data_table_const_iterator<BOOST_PP_ENUM_PARAMS(BOOST_PP_ADD(n, 1), t)> end() const;

        BOOST_PP_REPEAT(DATA_TABLE_MAX_COLUMNS, DATA_TABLE_BEGIN_ITERATOR_DECL,       DATA_TABLE_COLUMN_TYPE_PREFIX)
        BOOST_PP_REPEAT(DATA_TABLE_MAX_COLUMNS, DATA_TABLE_END_ITERATOR_DECL,         DATA_TABLE_COLUMN_TYPE_PREFIX)
        BOOST_PP_REPEAT(DATA_TABLE_MAX_COLUMNS, DATA_TABLE_BEGIN_CONST_ITERATOR_DECL, DATA_TABLE_COLUMN_TYPE_PREFIX)
        BOOST_PP_REPEAT(DATA_TABLE_MAX_COLUMNS, DATA_TABLE_END_CONST_ITERATOR_DECL,   DATA_TABLE_COLUMN_TYPE_PREFIX)

    /* Sequence interface */
    /*********************/
    public:
        const variant& at(size_t index) const;
        variant& at(size_t index);
        const variant& operator[](size_t index) const;
        variant& operator[](size_t index);

    /* Mapping interface */
    /*********************/
    public:
        variant& insert(const std::string& key, const variant& value, enum_return_trait_t ret = ReturnSelf);
        bool has_key(const std::string& key) const;

        const variant& at(const std::string& key) const;
        variant& at(const std::string& key);
        const variant& operator[](const std::string& key) const;
        variant& operator[]( const std::string &key );
        variant& remove(const std::string& key);
        variant range(const std::string& key) const;

    /* Comparison/Equality */
    /***********************/
    public:
        int compare(const variant& rhs) const;
        bool operator<(const variant& rhs) const;
        bool operator==(const variant& rhs) const;
        bool operator!=(const variant& rhs) const;

    /* Miscelaneous */
    /****************/
    public:
        variant select(const std::string& path) const;

        template<typename TARGET, typename SOURCE>
        static TARGET lexical_cast(const SOURCE& arg);

        template<typename TARGET>
        static TARGET lexical_cast(const std::string& arg);

        boost::uint64_t hash(boost::uint64_t seed = 0) const;

        std::string str(bool summarise=false, const std::string& indent="") const;

    private:
        template <typename T>
        friend variant make_object(const variant& params);

    private:
        static void select_impl(const variant& input, const std::string& path, variant& result);

        // type discriminator
        enum_type_t m_type;

    private:
        friend class detail::xml_default_handler;
        friend class binary_reader;
        friend class binary_writer;
    };

} // namespace protean

#include <protean/variant.ipp>
#include <protean/variant_ref.ipp>
#include <protean/array_iterator.hpp>

#endif // PROTEAN_VARIANT_HPP
