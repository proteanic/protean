//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef PROTEAN_VARIANT_HPP
#define PROTEAN_VARIANT_HPP

#include <protean/config.hpp>
#include <protean/variant_base.hpp>

namespace protean {

    class PROTEAN_DECL variant : public variant_base
    {
    public:
        typedef boost::gregorian::date              date_t;
        typedef boost::posix_time::time_duration    time_t;
        typedef boost::posix_time::ptime            date_time_t;

		typedef variant								value_type;
		typedef value_type*							pointer;
		typedef const value_type*					const_pointer;
		typedef value_type&							reference;
		typedef const value_type&					const_reference;

		typedef size_t								difference_type;
		typedef std::bidirectional_iterator_tag		iterator_category;

    /* Construction */
    /****************/
    public:
        variant();
        variant(const variant& value);

        template<typename T>
        variant(T value, typename boost::enable_if<boost::is_pod<T> >::type* dummy = 0);

        explicit variant(enum_type_t type, size_t size=0);
        explicit variant(enum_type_t type, const variant& arg);
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

    private:
        explicit variant(const handle<object>& arg);

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

        /* Destructor */
        /**************/
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
        template<typename T>
        typename boost::disable_if<boost::mpl::or_<boost::is_pointer<T>, boost::is_base_of<object,T> >, T>::type
        as() const;

        template<typename T>
        typename boost::enable_if<boost::is_pointer<T>, const T>::type
        as() const;

        template<typename T>
        typename boost::enable_if<boost::is_base_of<object,T>, const T&>::type
        as() const;

    /* Collection interface */
    /************************/
    public:
        enum enum_return_trait_t {ReturnSelf, ReturnItem};

        typedef variant_iterator<const_iterator_traits> const_iterator;
        typedef variant_iterator<iterator_traits> iterator;

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
        static TARGET lexical_cast(const SOURCE& toCast);


        void swap(variant& rhs);

        size_t hash() const;

        variant down_cast(enum_type_t type) const;
        variant up_cast() const;

        std::string str(bool summarise=false, const std::string& indent="") const;

    private:
        template <typename T>
        friend variant make_object(const variant& params);

        friend variant make_any(const std::string& value);

        friend size_t hash_value(const variant& value);

    private:
        static void select_impl(const variant& input, const std::string& path, variant& result);

        // type discriminator
        enum_type_t m_type;

    private:
        friend class xml_reader;
        friend class xml_writer;
        friend class sax_content_handler;
        friend class binary_reader;
        friend class binary_writer;
    };

    variant make_any(const std::string& value);

} // namespace protean

#include <protean/variant.ipp>

#endif // PROTEAN_VARIANT_HPP
