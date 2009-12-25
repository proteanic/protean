//  (C) Copyright Johan Ditmar, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#include <boost/config/warning_disable.hpp>
#include <protean/binary_writer.hpp>
#include <protean/variant.hpp>

#include <boost/foreach.hpp>

namespace protean {
    
    binary_writer::binary_writer(std::ostream &os, int flags) :
        m_os(os),
        m_flags(flags)
    {
    }

    binary_writer::~binary_writer()
    {
        close();
    }

    void binary_writer::write_value(const variant& value)
    {
        switch( value.type() )
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
                const exception_info e( value.as<exception_info>() );
                write(e.type());
                write(e.message());
                break;
            }
	    default:
 	        boost::throw_exception (
		    variant_error ("Case exhaustion: " + variant::enum_to_string (value.type ()))); 
	 
        }
    }

    void binary_writer::write(const variant& value)
    {
        write(static_cast<boost::int32_t>(value.type()));
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
        boost::gregorian::greg_year_month_day ymd = value.year_month_day();

        binary_date_t date;
        date.year    = static_cast<short>(ymd.year);
        date.month    = static_cast<char>(ymd.month);
        date.day    = static_cast<char>(ymd.day);

        write_bytes(reinterpret_cast<const char*>(&date), sizeof(binary_date_t));
    }
    void binary_writer::write(const variant::time_t& value)
    {
        binary_time_t time;
        time.hour    = static_cast<char>(value.hours());
        time.minute    = static_cast<char>(value.minutes());
        time.second    = static_cast<char>(value.seconds());

        write_bytes(reinterpret_cast<const char*>(&time), sizeof(binary_time_t));
    }
    void binary_writer::write(const variant::date_time_t& value)
    {
        write(value.date());
        write(value.time_of_day());
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
        // [13 FF 48 49][MAJOR MINOR][FLAGS]
        header[0] = binary_magic_number;
        header[1] = (binary_major_version << 16) | binary_minor_version;
        header[2] = (m_flags & Compress) ? 1 : 0;

        if (!m_os.write(reinterpret_cast<const char*>(header), sizeof(header)))
        {
            boost::throw_exception(variant_error("Error writing header to stream"));
        }

        // create compression filter if necessary
        if ((m_flags & Compress)!=0)
        {
            m_filter.push(boost::iostreams::zlib_compressor(binary_compression_params()));
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
