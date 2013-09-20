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
    template <typename T>
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

    template <typename T>
    class PROTEAN_DECL data_table_column_writer_base : public data_table_column_writer
    {
    public:
        explicit data_table_column_writer_base(const data_table_column_base& column);

    public:
        virtual bool has_next() const;
        virtual void advance();

    protected:
        typename data_table_column<T>::container_type::const_iterator       m_iter;
        const typename data_table_column<T>::container_type::const_iterator m_end;
    };

    /* Binary writer: writing to a binary_writer */
    /*********************************************/
    template <typename T>
    struct binary_writer_traits
    {
        static void write(binary_writer& writer, const T& value);
    };

    template <typename T>
    class PROTEAN_DECL data_table_column_binary_writer : public data_table_column_writer_base<T>
    {
    public:
        typedef binary_writer_traits<T> binary_writer_traits;

        data_table_column_binary_writer(const data_table_column_base& column, binary_writer& writer);

    public:
        virtual void write();

    private:
        binary_writer& m_writer;
    };

    /* Stream writer: writing to a std::osteram */
    /********************************************/
    template <typename T>
    struct stream_writer_traits
    {
        static void write(std::ostream& os, const T& value);
    };

    template <>
    struct stream_writer_traits<detail::string>
    {
        static void write(std::ostream& os, const detail::string& str);
    };

    template <typename T>
    class PROTEAN_DECL data_table_column_stream_writer : public data_table_column_writer_base<T>
    {
    public:
        typedef stream_writer_traits<T> stream_writer_traits;

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

    template <typename T>
    class PROTEAN_DECL data_table_column_reader_base : public data_table_column_reader
    {
    public:
        explicit data_table_column_reader_base(data_table_column_base& column);

    public:
        virtual bool has_next() const;
        virtual void advance();

    protected:
        typename data_table_column<T>::container_type::iterator       m_iter;
        const typename data_table_column<T>::container_type::iterator m_end;
    };

    /* Binary reader: reading from a binary_reader */
    /***********************************************/
    template <typename T>
    struct binary_reader_traits
    {
        static void read(binary_reader& reader, T& value);
    };

    template <>
    struct binary_reader_traits<bool>
    {
        static void read(binary_reader& reader, data_table_column<bool>::container_type::reference value);
    };

    template <typename T>
    class PROTEAN_DECL data_table_column_binary_reader : public data_table_column_reader_base<T>
    {
    public:
        typedef binary_reader_traits<T> binary_reader_traits;

        data_table_column_binary_reader(data_table_column_base& column, binary_reader& reader);

    public:
        virtual void read();

    private:
        binary_reader& m_reader;
    };

    /* Stream reader: reading from a std::istream */
    /**********************************************/
    template <typename T>
    struct stream_reader_traits
    {
        static void read(std::istream& is, T& value);
    };

    template <>
    struct stream_reader_traits<bool>
    {
        static void read(std::istream& is, data_table_column<bool>::container_type::reference value);
    };

    template <>
    struct stream_reader_traits<detail::string>
    {
        static void read(std::istream& is, detail::string& value);
    };

    template <typename T>
    class PROTEAN_DECL data_table_column_stream_reader : public data_table_column_reader_base<T>
    {
    public:
        typedef stream_reader_traits<T> stream_reader_traits;

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
