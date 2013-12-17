//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef PROTEAN_DETAIL_DATA_TABLE_COLUMN_SERIALIZERS_HPP
#define PROTEAN_DETAIL_DATA_TABLE_COLUMN_SERIALIZERS_HPP

#include <protean/config.hpp>
#include <protean/binary_writer.hpp>
#include <protean/binary_reader.hpp>

#include <boost/shared_ptr.hpp>

#if defined(_MSC_VER)
#    pragma warning(push)
#    pragma warning(disable:4251)
#endif

namespace protean { namespace detail {

    // Forward declaration
    class data_table_column_base;
    template <variant_base::enum_type_t E>
    class data_table_column;


    /* Column writers ************************************/
    /* For writing a column's values to some destination */
    /*****************************************************/
    class PROTEAN_DECL data_table_column_writer
    {
    public:
        virtual ~data_table_column_writer() {}

    public:
        virtual bool has_next() const = 0;
        virtual void advance()        = 0;
        virtual void write()          = 0;
    };

    template <variant_base::enum_type_t E>
    class PROTEAN_DECL data_table_column_writer_base : public data_table_column_writer
    {
    public:
        explicit data_table_column_writer_base(const data_table_column_base& column);

    public:
        virtual bool has_next() const;
        virtual void advance();

    protected:
        typename data_table_column<E>::container_type::const_iterator       m_iter;
        const typename data_table_column<E>::container_type::const_iterator m_end;
    };

    /* Binary writer: writing to a binary_writer */
    /*********************************************/
    template <variant_base::enum_type_t E>
    struct binary_writer_traits
    {
        static void write(binary_writer& writer, const typename column_traits<E>::value_type& value);
    };

    template <variant_base::enum_type_t E>
    class PROTEAN_DECL data_table_column_binary_writer : public data_table_column_writer_base<E>
    {
    public:
        typedef binary_writer_traits<E> binary_writer_traits;

        data_table_column_binary_writer(const data_table_column_base& column, binary_writer& writer);

    public:
        virtual void write();

    private:
        binary_writer& m_writer;
    };

    /* Stream writer: writing to a std::osteram */
    /********************************************/
    template <variant_base::enum_type_t E>
    struct stream_writer_traits
    {
        static void write(std::ostream& os, const typename column_traits<E>::value_type& value);
    };

    template <>
    struct stream_writer_traits<variant_base::String>
    {
        static void write(std::ostream& os, const detail::string& str);
    };

    template <>
    struct stream_writer_traits<variant_base::Any>
    {
        static void write(std::ostream& os, const detail::string& str);
    };

    template <variant_base::enum_type_t E>
    class PROTEAN_DECL data_table_column_stream_writer : public data_table_column_writer_base<E>
    {
    public:
        typedef stream_writer_traits<E> stream_writer_traits;

        data_table_column_stream_writer(const data_table_column_base& column, std::ostream& os);

    public:
        virtual void write();

    private:
        std::ostream& m_os;
    };


    /* Column readers ************************************/
    /* For reading values from some source into a column */
    /*****************************************************/
    class PROTEAN_DECL data_table_column_reader
    {
    public:
        virtual ~data_table_column_reader() {}

    public:
        virtual bool has_next() const = 0;
        virtual void advance()        = 0;
        virtual void read()           = 0;
    };

    template <variant_base::enum_type_t E>
    class PROTEAN_DECL data_table_column_reader_base : public data_table_column_reader
    {
    public:
        explicit data_table_column_reader_base(data_table_column_base& column);

    public:
        virtual bool has_next() const;
        virtual void advance();

    protected:
        typename data_table_column<E>::container_type::iterator       m_iter;
        const typename data_table_column<E>::container_type::iterator m_end;
    };

    /* Binary reader: reading from a binary_reader */
    /***********************************************/
    template <variant_base::enum_type_t E>
    struct binary_reader_traits
    {
        static void read(binary_reader& reader, typename column_traits<E>::value_type& value);
    };

    template <>
    struct binary_reader_traits<variant_base::Boolean>
    {
        static void read(binary_reader& reader, data_table_column<variant_base::Boolean>::container_type::reference value);
    };

    template <variant_base::enum_type_t E>
    class PROTEAN_DECL data_table_column_binary_reader : public data_table_column_reader_base<E>
    {
    public:
        typedef binary_reader_traits<E> binary_reader_traits;

        data_table_column_binary_reader(data_table_column_base& column, binary_reader& reader);

    public:
        virtual void read();

    private:
        binary_reader& m_reader;
    };

    /* Stream reader: reading from a std::istream */
    /**********************************************/
    template <variant_base::enum_type_t E>
    struct stream_reader_traits
    {
        static void read(std::istream& is, typename column_traits<E>::value_type& value);
    };

    template <>
    struct stream_reader_traits<variant_base::Boolean>
    {
        static void read(std::istream& is, data_table_column<variant_base::Boolean>::container_type::reference value);
    };

    template <>
    struct stream_reader_traits<variant_base::String>
    {
        static void read(std::istream& is, detail::string& value);
    };

    template <>
    struct stream_reader_traits<variant_base::Any>
    {
        static void read(std::istream& is, detail::string& value);
    };

    template <variant_base::enum_type_t E>
    class PROTEAN_DECL data_table_column_stream_reader : public data_table_column_reader_base<E>
    {
    public:
        typedef stream_reader_traits<E> stream_reader_traits;

        data_table_column_stream_reader(data_table_column_base& column, const boost::shared_ptr<std::istream>& is);

    public:
        virtual void read();

    private:
        boost::shared_ptr<std::istream> m_is;
    };


    /*********************************************************************************/
    /* Helpers for creating column serializers (readers/writers) of the correct type */
    /*********************************************************************************/
    data_table_column_writer* make_data_table_column_binary_writer(const data_table_column_base& column, binary_writer& writer);
    data_table_column_writer* make_data_table_column_stream_writer(const data_table_column_base& column, std::ostream& os);
    data_table_column_reader* make_data_table_column_binary_reader(data_table_column_base& column, binary_reader& reader);
    data_table_column_reader* make_data_table_column_stream_reader(data_table_column_base& column, const boost::shared_ptr<std::istream>& is);

}} // namespace protean::detail

#include <protean/detail/data_table_column_serializers.ipp>


#if defined(_MSC_VER)
#    pragma warning(pop)
#endif

#endif // PROTEAN_DETAIL_DATA_TABLE_COLUMN_SERIALIZERS_HPP
