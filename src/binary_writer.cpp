//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#if defined(_MSC_VER)
#    pragma warning(disable:4503 4996)
#endif

#include <protean/binary_writer.hpp>
#include <protean/variant.hpp>
#include <protean/detail/data_table.hpp>
#include <protean/detail/data_table_column_serializers.hpp>

#include <boost/foreach.hpp>
#include <boost/scoped_ptr.hpp>

namespace protean {
    
    binary_writer::binary_writer(std::ostream &os, int mode) :
        m_os(os),
        m_mode(mode | binary_mode::DateTimeAsTicks)
    {
    }

    binary_writer::~binary_writer()
    {
        close();
    }

    void binary_writer::write_value(const variant& value)
    {
        switch(value.type())
        {
            case variant::Double:
                write(value.as<double>());
                break;
            case variant::String:
                write(value.as<std::string>());
                break;
            case variant::Int32:
                write(value.as<boost::int32_t>());
                break;
            case variant::Boolean:
                write(value.as<bool>());
                break;
            case variant::UInt32:
                write(value.as<boost::uint32_t>());
                break;
            case variant::Int64:
                write(value.as<boost::int64_t>());
                break;
            case variant::UInt64:
                write(value.as<boost::uint64_t>());
                break;
            case variant::Float:
                write(value.as<float>());
                break;
            case variant::Date:
            {
                write(value.as<variant::date_t>());
                break;
            }
            case variant::Time:
            {
                write(value.as<variant::time_t>());
                break;
            }
            case variant::DateTime:
            {
                write(value.as<variant::date_time_t>());
                break;
            }
            case variant::Dictionary:
            case variant::Bag:
            {
                write(static_cast<boost::uint32_t>(value.size()));

                variant::const_iterator it, end = value.end();
                for ( it=value.begin(); it != end; ++it )
                {
                    write(it.key());
                    write(it.value() );
                }
                break;
            }
            case variant::Tuple:
            {
                write(static_cast<boost::uint32_t>(value.size()));

                for(size_t n(0); n!= value.size(); ++n)
                {
                    write(value[n]);
                }

                break;
            }
            case variant::List:
            {
                write(static_cast<boost::uint32_t>(value.size()));

                BOOST_FOREACH(const variant& child, value)
                {
                    write(child);
                }
                break;
            }
            case variant::Buffer:
            {
                write(value.as<void*>(), value.size());
                break;
            }
            case variant::TimeSeries:
            {
                write(static_cast<boost::uint32_t>(value.size()));

                variant::const_iterator it, end = value.end();
                for (it=value.begin(); it != end; ++it)
                {
                    write(it.time());
                    write(it.value());
                }
                break;
            }
            case variant::DataTable:
            {
                const detail::data_table& dt( value.m_value.get<variant::DataTable>() );
				const auto& columns( dt.columns() );

				write( static_cast<boost::int32_t>( columns.size() ) );
                write( static_cast<boost::int32_t>( dt.size() ) );

                BOOST_FOREACH( const auto& column, columns )
					write( static_cast<boost::int32_t>( column.type() ) );
                BOOST_FOREACH( const auto& column, columns )
					write( column.name() );

                BOOST_FOREACH(detail::data_table::column_container_type::const_reference column, columns)
                {
                    if (column.type() & variant_base::Primitive)
                    {
                        boost::scoped_ptr<detail::data_table_column_writer> column_writer(
                            detail::make_data_table_column_binary_writer(column, *this)
                        );

                        while (column_writer->has_next())
                        {
                            column_writer->write();
                            column_writer->advance();
                        }
                    }
                    else
                    {
		                variant::const_iterator iter(column.begin());
		                variant::const_iterator end(column.end());
		                while (iter != end)
		                    write(*(iter++));
                    }
                }
                break;
            }
            case variant::Object:
            {
                const object& obj(value.as<object>());

                // write class name
                write(obj.name());

                // write version
                write(static_cast<boost::int32_t>(obj.version()));

                // write parameter dictionary
                variant params;
                obj.deflate(params);
                write(params);

                break;
            }
            case variant::None:
                break;
            case variant::Any:
                write(value.as<std::string>());
                break;
            case variant::Exception:
            {
                const exception_data& e(value.as<exception_data>());
                write(e.type());
                write(e.message());
                write(e.source());
                write(e.stack());
                break;
            }
            case variant::Array:
            {
                const typed_array& a(value.as<typed_array>());
                write(static_cast<boost::uint32_t>(a.size()));
                write(static_cast<boost::uint32_t>(a.type()));

                for(size_t n(0); n!=a.size(); ++n)
                {
                    write_value(variant(a[n]));
                }
                break;
            }

        default:
             boost::throw_exception(variant_error ("Case exhaustion: " + variant::enum_to_string(value.type()))); 
     
        }
    }

    void binary_writer::write(const variant& value)
    {
        write(static_cast<boost::uint32_t>(value.type()));
        write_value( value );
    }

    void binary_writer::write(const std::string& value)
    {
        size_t length = value.size();

        write(static_cast<boost::uint32_t>(length));
        if (length>0)
        {
            write_bytes(value.c_str(), length);
        }
    }
    void binary_writer::write(const detail::string& value)
    {
        size_t length = value.size();

        write(static_cast<boost::uint32_t>(length));
        if (length>0)
        {
            write_bytes(value.value(), length);
        }
    }
    void binary_writer::write(bool arg)
    {
        boost::int32_t value(arg ? 1 : 0);
        write(value);
    }
    void binary_writer::write(boost::int32_t value)
    {
        write_bytes(reinterpret_cast<const char*>(&value), 4);
    }
    void binary_writer::write(boost::uint32_t value)
    {
        write_bytes(reinterpret_cast<const char*>(&value), 4);
    }
    void binary_writer::write(boost::int64_t value)
    {
        write_bytes(reinterpret_cast<const char*>(&value), 8);
    }
    void binary_writer::write(boost::uint64_t value)
    {
        write_bytes(reinterpret_cast<const char*>(&value), 8);
    }
    void binary_writer::write(float value)
    {
        write_bytes(reinterpret_cast<const char*>(&value), sizeof(float));
    }
    void binary_writer::write(double value)
    {
        write_bytes(reinterpret_cast<const char*>(&value), sizeof(double));
    }
    void binary_writer::write(const variant::date_t& value)
    {
		write((boost::uint32_t)(value - variant::min_date()).days());
    }
    void binary_writer::write(const variant::time_t& value)
    {
        write((boost::int64_t)value.total_milliseconds());
    }
    void binary_writer::write(const variant::date_time_t& value)
    {
        write(value - variant::min_date_time());
    }

    void binary_writer::write(const void* data, size_t length)
    {
        write(static_cast<boost::uint32_t>(length));
        write_bytes(reinterpret_cast<const char*>(data), length);
    }

    void binary_writer::write_bytes(const char* buffer, size_t length)
    {
        // write data
        if (!m_filter.write(buffer, length))
        {
            boost::throw_exception(variant_error("Error writing to stream"));
        }

        // write packing bytes (if necessary)
        size_t residual = (4 - (length % 4)) % 4;
        if (residual!=0)
        {
            const int packing = 0;
            if (!m_filter.write(reinterpret_cast<const char*>(&packing), residual))
            {
                boost::throw_exception(variant_error("Error writing to stream"));
            }
        }
    }
    
    void binary_writer::setup()
    {
        if(!m_os.good())
        {
            boost::throw_exception(variant_error("Output stream is bad"));
        }

        boost::uint32_t header[3];

        // write header
        // [13 FF 48 49][MAJOR MINOR][MODE]
        header[0] = binary_magic_number;
        header[1] = (binary_major_version << 16) | binary_minor_version;
        header[2] = m_mode;

        if (!m_os.write(reinterpret_cast<const char*>(header), sizeof(header)))
        {
            boost::throw_exception(variant_error("Error writing header to stream"));
        }

        // create compression filter if necessary
        if ((m_mode & binary_mode::Compress)!=0)
        {
            bool zlib_no_header = (m_mode & binary_mode::ZlibHeader)==0;
            m_filter.push(boost::iostreams::zlib_compressor(binary_compression_params(zlib_no_header)));
        }
        m_filter.push(m_os);
    }

    void binary_writer::close()
    {
        m_filter.reset();
    }

    binary_writer& operator<<(binary_writer& writer, const variant& v)
    {
        writer.setup();
        writer.write( v );
        writer.close();

        return writer;
    }

} // namespace protean
