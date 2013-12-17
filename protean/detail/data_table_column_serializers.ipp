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
    template <variant_base::enum_type_t E>
    data_table_column_writer_base<E>::data_table_column_writer_base(const data_table_column_base& column)
        : m_iter(column.begin<E>()),
          m_end(column.end<E>())
    {}

    template <variant_base::enum_type_t E>
    bool data_table_column_writer_base<E>::has_next() const
    {
        return m_iter != m_end;
    }

    template <variant_base::enum_type_t E>
    void data_table_column_writer_base<E>::advance()
    {
        ++m_iter;
    }

    /* Binary writer: writing to a binary_writer */
    /*********************************************/
    template <variant_base::enum_type_t E>
    void binary_writer_traits<E>::write(binary_writer& writer, const typename column_traits<E>::value_type& value)
    {
        writer.write(value);
    }

    template <variant_base::enum_type_t E>
    data_table_column_binary_writer<E>::
    data_table_column_binary_writer(const data_table_column_base& column, binary_writer& writer)
        : data_table_column_writer_base<E>(column),
          m_writer(writer)
    {}

    template <variant_base::enum_type_t E>
    void data_table_column_binary_writer<E>::write()
    {
        data_table_column_binary_writer<E>::binary_writer_traits::write(m_writer, *m_iter);
    }

    /* Stream writer: writing to a std::osteram */
    /********************************************/
    template <variant_base::enum_type_t E>
    inline void stream_writer_traits<E>::write(std::ostream& os, const typename column_traits<E>::value_type& value)
    {
        os << value;
    }

    inline void stream_writer_traits<variant_base::String>::write(std::ostream& os, const detail::string& str)
    {
        os << str.value();
    }

    inline void stream_writer_traits<variant_base::Any>::write(std::ostream& os, const detail::string& str)
    {
        os << str.value();
    }

    template <variant_base::enum_type_t E>
    data_table_column_stream_writer<E>::
    data_table_column_stream_writer(const data_table_column_base& column, std::ostream& os)
        : data_table_column_writer_base<E>(column),
          m_os(os)
    {}

    template <variant_base::enum_type_t E>
    void data_table_column_stream_writer<E>::write()
    {
        data_table_column_stream_writer<E>::stream_writer_traits::write(m_os, *m_iter);
    }


    /* Column readers ************************************/
    /* For reading values from some source into a column */
    /*****************************************************/

    /* Base column reader */
    /**********************/
    template <variant_base::enum_type_t E>
    data_table_column_reader_base<E>::
    data_table_column_reader_base(data_table_column_base& column)
        : m_iter(column.begin<E>()),
          m_end(column.end<E>())
    {}

    template <variant_base::enum_type_t E>
    bool data_table_column_reader_base<E>::has_next() const
    {
        return m_iter != m_end;
    }

    template <variant_base::enum_type_t E>
    void data_table_column_reader_base<E>::advance()
    {
        ++m_iter;
    }

    /* Binary reader: reading from a binary_reader */
    /***********************************************/
    template <variant_base::enum_type_t E>
    inline void binary_reader_traits<E>::read(binary_reader& reader, typename column_traits<E>::value_type& value)
    {
        reader.read(value);
    }

    inline void binary_reader_traits<variant_base::Boolean>::read(binary_reader& reader, data_table_column<variant_base::Boolean>::container_type::reference value)
    {
        bool tmp;
        reader.read(tmp);
        value = tmp;
    }

    template <variant_base::enum_type_t E>
    data_table_column_binary_reader<E>::
    data_table_column_binary_reader(data_table_column_base& column, binary_reader& reader)
        : data_table_column_reader_base<E>(column),
          m_reader(reader)
    {}

    template <variant_base::enum_type_t E>
    void data_table_column_binary_reader<E>::read()
    {
        data_table_column_binary_reader<E>::binary_reader_traits::read(m_reader, *m_iter);
    }

    /* Stream reader: reading from a std::istream */
    /**********************************************/
    template <variant_base::enum_type_t E>
    inline void stream_reader_traits<E>::read(std::istream& is, typename column_traits<E>::value_type& value)
    {
        is >> value;
    }

    inline void stream_reader_traits<variant_base::Boolean>::read(std::istream& is, data_table_column<variant_base::Boolean>::container_type::reference value)
    {
        bool tmp;
        is >> tmp;
        value = tmp;
    }

    inline void stream_reader_traits<variant_base::String>::read(std::istream& is, detail::string& value)
    {
        std::string tmp;
        std::getline(is, tmp);
        value.initialise(tmp.c_str(), tmp.size());
    }

    inline void stream_reader_traits<variant_base::Any>::read(std::istream& is, detail::string& value)
    {
        std::string tmp;
        std::getline(is, tmp);
        value.initialise(tmp.c_str(), tmp.size());
    }

    template <variant_base::enum_type_t E>
    data_table_column_stream_reader<E>::
    data_table_column_stream_reader(data_table_column_base& column, const boost::shared_ptr<std::istream>& is)
        : data_table_column_reader_base<E>(column),
          m_is(is)
    {}

    template <variant_base::enum_type_t E>
    void data_table_column_stream_reader<E>::read()
    {
        data_table_column_stream_reader<E>::stream_reader_traits::read(*m_is, *m_iter);
    }


    /* For making DataTable column readers and writers given a column type at runtime */
    /**********************************************************************************/
    namespace
    {
        /* For instantiating column serializers (readers/writers) of the correct type at */
        /* runtime given a data table column base ****************************************/
        template <
            template <variant_base::enum_type_t> class ColumnSerializer,
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

            template <typename TypedEnum>
            result_type operator()(TypedEnum* = 0)
            {
                return new ColumnSerializer<static_cast<variant_base::enum_type_t>(TypedEnum::value)>(m_column, m_serializer);
            }

        private:
            Column&     m_column;
            Serializer& m_serializer;
        };

        template <
            template <variant_base::enum_type_t> class ColumnWriter,
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
            template <variant_base::enum_type_t> class ColumnReader,
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
        return enum_runtime_map<data_table_column_primitive_enums, enum_equality_comparer>(
            column.type(), writer_allocator, "Cannot binary-serialize primitive datatable column of type " + variant_base::enum_to_string(column.type())
        );
    }

    inline data_table_column_writer* make_data_table_column_stream_writer(const data_table_column_base& column, std::ostream& os)
    {
        column_writer_allocator<data_table_column_stream_writer, std::ostream>::type writer_allocator(column, os);
        return enum_runtime_map<data_table_column_primitive_enums, enum_equality_comparer>(
            column.type(), writer_allocator, "Cannot XML-serialize primitive datatable column of type " + variant_base::enum_to_string(column.type())
        );
    }

    inline data_table_column_reader* make_data_table_column_binary_reader(data_table_column_base& column, binary_reader& reader)
    {
        column_reader_allocator<data_table_column_binary_reader, binary_reader>::type reader_allocator(column, reader);
        return enum_runtime_map<data_table_column_primitive_enums, enum_equality_comparer>(
            column.type(), reader_allocator, "Cannot binary-serialize primitive datatable column of type " + variant_base::enum_to_string(column.type())
        );
    }

    inline data_table_column_reader* make_data_table_column_stream_reader(data_table_column_base& column, const boost::shared_ptr<std::istream>& is)
    {
        column_reader_allocator<data_table_column_stream_reader, const boost::shared_ptr<std::istream> >::type reader_allocator(column, is);
        return enum_runtime_map<data_table_column_primitive_enums, enum_equality_comparer>(
            column.type(), reader_allocator, "Cannot XML-serialize primitive datatable column of type " + variant_base::enum_to_string(column.type())
        );
    }

}} // namespace protean::detail
