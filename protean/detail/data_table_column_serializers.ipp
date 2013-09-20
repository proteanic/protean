//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#include <protean/detail/data_table_types.hpp>

namespace protean { namespace detail {

    /* Column writers ************************************/
    /* For writing a column's values to some destination */
    /*****************************************************/

    /* Base column writer */
    /**********************/
    template <typename T>
    data_table_column_writer_base<T>::data_table_column_writer_base(const data_table_column_base& column)
        : m_iter(column.begin<T>()),
          m_end(column.end<T>())
    {}

    template <typename T>
    bool data_table_column_writer_base<T>::has_next() const
    {
        return m_iter != m_end;
    }

    template <typename T>
    void data_table_column_writer_base<T>::advance()
    {
        ++m_iter;
    }

    /* Binary writer: writing to a binary_writer */
    /*********************************************/
    template <typename T>
    void binary_writer_traits<T>::write(binary_writer& writer, const T& value)
    {
        writer.write(value);
    }

    template <typename T>
    data_table_column_binary_writer<T>::
    data_table_column_binary_writer(const data_table_column_base& column, binary_writer& writer)
        : data_table_column_writer_base<T>(column),
          m_writer(writer)
    {}

    template <typename T>
    void data_table_column_binary_writer<T>::write()
    {
        data_table_column_binary_writer<T>::binary_writer_traits::write(m_writer, *m_iter);
    }

    /* Stream writer: writing to a std::osteram */
    /********************************************/
    template <typename T>
    inline void stream_writer_traits<T>::write(std::ostream& os, const T& value)
    {
        os << value;
    }

    inline void stream_writer_traits<detail::string>::write(std::ostream& os, const detail::string& str)
    {
        os << str.value();
    }

    template <typename T>
    data_table_column_stream_writer<T>::
    data_table_column_stream_writer(const data_table_column_base& column, std::ostream& os)
        : data_table_column_writer_base<T>(column),
          m_os(os)
    {}

    template <typename T>
    void data_table_column_stream_writer<T>::write()
    {
        data_table_column_stream_writer<T>::stream_writer_traits::write(m_os, *m_iter);
    }


    /* Column readers ************************************/
    /* For reading values from some source into a column */
    /*****************************************************/

    /* Base column reader */
    /**********************/
    template <typename T>
    data_table_column_reader_base<T>::
    data_table_column_reader_base(data_table_column_base& column)
        : m_iter(column.begin<T>()),
          m_end(column.end<T>())
    {}

    template <typename T>
    bool data_table_column_reader_base<T>::has_next() const
    {
        return m_iter != m_end;
    }

    template <typename T>
    void data_table_column_reader_base<T>::advance()
    {
        ++m_iter;
    }

    /* Binary reader: reading from a binary_reader */
    /***********************************************/
    template <typename T>
    inline void binary_reader_traits<T>::read(binary_reader& reader, T& value)
    {
        reader.read(value);
    }

    inline void binary_reader_traits<bool>::read(binary_reader& reader, data_table_column<bool>::container_type::reference value)
    {
        bool tmp;
        reader.read(tmp);
        value = tmp;
    }

    template <typename T>
    data_table_column_binary_reader<T>::
    data_table_column_binary_reader(data_table_column_base& column, binary_reader& reader)
        : data_table_column_reader_base<T>(column),
          m_reader(reader)
    {}

    template <typename T>
    void data_table_column_binary_reader<T>::read()
    {
        data_table_column_binary_reader<T>::binary_reader_traits::read(m_reader, *m_iter);
    }

    /* Stream reader: reading from a std::istream */
    /**********************************************/
    template <typename T>
    inline void stream_reader_traits<T>::read(std::istream& is, T& value)
    {
        is >> value;
    }

    inline void stream_reader_traits<bool>::read(std::istream& is, data_table_column<bool>::container_type::reference value)
    {
        bool tmp;
        is >> tmp;
        value = tmp;
    }

    inline void stream_reader_traits<detail::string>::read(std::istream& is, detail::string& value)
    {
        std::string tmp;
        std::getline(is, tmp);
        value.initialise(tmp.c_str(), tmp.size());
    }

    template <typename T>
    data_table_column_stream_reader<T>::
    data_table_column_stream_reader(data_table_column_base& column, const boost::shared_ptr<std::istream>& is)
        : data_table_column_reader_base<T>(column),
          m_is(is)
    {}

    template <typename T>
    void data_table_column_stream_reader<T>::read()
    {
        data_table_column_stream_reader<T>::stream_reader_traits::read(*m_is, *m_iter);
    }


    /* For making DataTable column readers and writers given a column type at runtime */
    /**********************************************************************************/
    namespace
    {
        /* For instantiating column serializers (readers/writers) of the correct type at */
        /* runtime given a data table column base ****************************************/
        template <
            template <class> class ColumnSerializer,
            typename ColumnSerializerBase,
            typename Column,
            typename Serializer
        >
        struct column_serializer_allocator
        {
            typedef ColumnSerializerBase* result_type;

            column_serializer_allocator(Column& column, Serializer& serializer)
                : m_column(column),
                  m_serializer(serializer)
            {}

            template <typename DataTableTypePair>
            ColumnSerializerBase* operator()(DataTableTypePair* = 0)
            {
                return new ColumnSerializer<typename DataTableTypePair::second>(m_column, m_serializer);
            }

        private:
            Column&     m_column;
            Serializer& m_serializer;
        };

        template <
            template <class> class ColumnWriter,
            typename Writer
        >
        struct column_writer_allocator
        {
            typedef column_serializer_allocator<
                ColumnWriter,
                data_table_column_writer,
                const data_table_column_base,
                Writer
            > type;
        };

        template <
            template <class> class ColumnReader,
            typename Reader
        >
        struct column_reader_allocator
        {
            typedef column_serializer_allocator<
                ColumnReader,
                data_table_column_reader,
                data_table_column_base,
                Reader
            > type;
        };
    }

    /* Inline helpers for creating column serializers (readers/writers) of the correct type */
    /****************************************************************************************/
    inline data_table_column_writer* make_data_table_column_binary_writer(const data_table_column_base& column, binary_writer& writer)
    {
        column_writer_allocator<data_table_column_binary_writer, binary_writer>::type writer_allocator(column, writer);
        return data_table_runtime_type_map(column.type(), writer_allocator);
    }

    inline data_table_column_writer* make_data_table_column_stream_writer(const data_table_column_base& column, std::ostream& os)
    {
        column_writer_allocator<data_table_column_stream_writer, std::ostream>::type writer_allocator(column, os);
        return data_table_runtime_type_map(column.type(), writer_allocator);
    }

    inline data_table_column_reader* make_data_table_column_binary_reader(data_table_column_base& column, binary_reader& reader)
    {
        column_reader_allocator<data_table_column_binary_reader, binary_reader>::type reader_allocator(column, reader);
        return data_table_runtime_type_map(column.type(), reader_allocator);
    }

    inline data_table_column_reader* make_data_table_column_stream_reader(data_table_column_base& column, const boost::shared_ptr<std::istream>& is)
    {
        column_reader_allocator<data_table_column_stream_reader, const boost::shared_ptr<std::istream> >::type reader_allocator(column, is);
        return data_table_runtime_type_map(column.type(), reader_allocator);
    }

}} // namespace protean::detail
