//  (C) Copyright Johan Ditmar, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#include <boost/config/warning_disable.hpp>
#include <protean/binary_reader.hpp>
#include <protean/variant.hpp>
#include <protean/object_proxy.hpp>
#include <protean/object_factory.hpp>
#include <protean/variant_base.hpp>

#include <boost/scoped_array.hpp>

namespace protean {
    
    binary_reader::binary_reader(std::istream &is, int flags) :
        m_is(is),
        m_flags(flags),
        m_factory(NULL)
    {
    }

    binary_reader::~binary_reader()
    {
        close();
    }

    void binary_reader::read_value(variant::enum_type_t type, variant& value)
    {
        switch( type )
        {
            case variant::Double:
            {
                double v;
                read( v ); value = v;
                break;
            }
            case variant::String:
            {
                std::string s;
                read( s ); value = s;
                break;
            }
            case variant::Int32:
            {
                variant::int32_t i;
                read( i ); value = i;
                break;
            }
            case variant::UInt32:
            {
                variant::uint32_t i;
                read( i ); value = i;
                break;
            }
            case variant::Boolean:
            {
                bool b;
                read( b ); value = b;
                break;
            }
            case variant::Int64:
            {
                variant::int64_t i;
                read( i ); value = i;
                break;
            }
            case variant::UInt64:
            {
                variant::uint64_t i;
                read( i ); value = i;
                break;
            }
            case variant::Float:
            {
                float v;
                read( v ); value = v;
                break;
            }
            case variant::Date:
            {
                variant::date_t v;
                read( v ); value = v;
                break;
            }
            case variant::Time:
            {
                variant::time_t t;
                read( t ); value = t;
                break;
            }
            case variant::DateTime:
            {
                variant::date_time_t dt;
                read( dt ); value = dt;
                break;
            }
            case variant::Dictionary:
            case variant::Bag:
            {
                value = variant( static_cast<variant::enum_type_t>(type) );
                int size; read( size );

                for (int i=0; i<size; ++i)
                {
                    std::string name;
                    read(name);

                    variant &val = value.insert(name, variant(), variant::ReturnItem);
                    read( val );
                }
                break;
            }
            case variant::List:
            case variant::Tuple:
            {
                int size;
                read(size);
                value = variant(static_cast<variant::enum_type_t>(type), size);

                for ( int i=0; i<size; ++i )
                {
                    read(value[i]);
                }
                break;
            }
            case variant::Buffer:
            {
                void*  data;
                size_t size;
                read(data, size);
                value = variant(data, size, false);
                break;
            }
            case variant::TimeSeries:
            {
                value = variant(static_cast<variant::enum_type_t>(type));
                int size; read(size);

                for (int i=0; i<size; ++i)
                {
                    variant::date_time_t date_time;
                    read(date_time);

                    variant &val = value.push_back(date_time, variant(), variant::ReturnItem);
                    read(val);
                }
                break;
            }
            case variant::Object:
            {
                std::string class_name;
                int version;

                read(class_name);
                read(version);

                object_handle obj;
                if (m_factory!=NULL)
                {
                    obj = m_factory->create_instance(class_name);
                    if (!obj)
                    {
                        if ((m_flags & CreateProxy)!=0)
                        {
                            boost::throw_exception(variant_error("Unable to create object from factory"));
                        }
                    }
                }

                if (!obj)
                {
                    obj = new object_proxy(class_name);
                }

                value = variant(obj);

                // read parameter dictionary
                variant params;
                read(params);
                value.as<object>().inflate(params, version);
        
                break;
            }
            case variant::Exception:
            {
                std::string type, message;
                read(type);
                read(message);
                value = exception_info(type, message);
                break;
            }
            case variant::Any:
            {
                std::string s;
                read( s ); value = variant(s).upCast();
                break;
            }
	    default:	    
	        boost::throw_exception (
                    variant_error ("Case exhaustion: " + variant::enum_to_string (type)));	    
        }
    }

    void binary_reader::read(variant& value)
    {
        int type = variant::None;
        read(type);
        read_value(static_cast<variant::enum_type_t>(type), value);
    }
    void binary_reader::read(std::string& value)
    {
        int length;
        read(length);

        boost::scoped_array<char> buffer( new char[length] );
        read_bytes(buffer.get(), length);
        value = std::string(buffer.get(), length);

    }
    void binary_reader::read(bool& value)
    {
        int b;
        read( b );
        value = (b != 0);
    }
    void binary_reader::read(int& value)
    {
        read_bytes(reinterpret_cast<char*>(&value), sizeof(int) );
    }
    void binary_reader::read(variant::int32_t& value)
    {
        read_bytes(reinterpret_cast<char*>(&value), 4 );
    }
    void binary_reader::read(variant::uint32_t& value )
    {
        read_bytes(reinterpret_cast<char*>(&value), 4 );
    }
    void binary_reader::read(variant::int64_t& value )
    {
        read_bytes(reinterpret_cast<char*>(&value), 8 );
    }
    void binary_reader::read(variant::uint64_t& value )
    {
        read_bytes(reinterpret_cast<char*>(&value), 8 );
    }
    void binary_reader::read(float& value)
    {
        read_bytes(reinterpret_cast<char*>(&value), sizeof(float) );
    }
    void binary_reader::read(double& value)
    {
        read_bytes(reinterpret_cast<char*>(&value), sizeof(double) );
    }
    void binary_reader::read(variant::date_t& value)
    {
        binary_date_t date;
        read_bytes(reinterpret_cast<char*>(&date), sizeof(binary_date_t));

        value = variant::date_t( date.year, date.month, date.day );
    }
    void binary_reader::read(variant::time_t& value)
    {
        binary_time_t time;
        read_bytes(reinterpret_cast<char*>(&time), sizeof(binary_time_t));

        value = variant::time_t(time.hour, time.minute, time.second);
    }
    void binary_reader::read(variant::date_time_t& value)
    {
        variant::date_t date;
        variant::time_t time;

        read(date);
        read(time);

        value = variant::date_time_t(date, time);    
    }
    void binary_reader::read(void*& data, size_t& length)
    {
        int i;
        read(i);
        length = i;

        // 'data' will be owned by the variant
        data = malloc( length );
        read_bytes(reinterpret_cast<char*>(data), length);
    }

    void binary_reader::set_factory(object_factory& factory)
    {
        m_factory = &factory;
    }

    void binary_reader::read_bytes(char* buffer, size_t length)
    {
        if (!m_filter.read(buffer, length))
        {
            boost::throw_exception(variant_error("Error reading from stream"));
        }

        // read packing bytes (if necessary)
        size_t residual = (4 - (length % 4)) % 4;
        if (residual != 0)
        {
            static int packing = 0;
            if (!m_filter.read(reinterpret_cast<char*>(&packing), residual))
            {
                boost::throw_exception(variant_error("Error reading from stream"));
            }
        }
    }

    void binary_reader::setup()
    {
        if( !m_is.good() )
        {
            boost::throw_exception(variant_error("Input stream is bad"));
        }

        // read header
        int header[2];
        if (!m_is.read(reinterpret_cast<char*>(header), sizeof(header)))
        {
            boost::throw_exception(variant_error("Error reading header from stream"));
        }

        // check magic number
        if ((header[0] >> 16)!=binary_magic_number)
        {
            boost::throw_exception(variant_error("Bad magic number, this looks like invalid binary data"));
        }

        // store version information
        m_major_version = ((header[0] >> 8) & 0x000000FF);
        m_minor_version = header[0] & 0x000000FF;

        if (m_major_version>binary_major_version)
        {
            boost::throw_exception(variant_error((boost::format("Version of received binary data is not compatible, received version %d > this version %d")
                % static_cast<int>(m_major_version)
                % static_cast<int>(binary_major_version)
            ).str()));
        }

        // create compression filter if necessary
        if ((header[1] & 0x00000001)!=0)
        {
            m_filter.push(boost::iostreams::zlib_decompressor(binary_compression_params()));
        }
        m_filter.push(m_is);
    }

    void binary_reader::close()
    {
        m_filter.reset();
    }

    binary_reader& operator>>(binary_reader& reader, variant& v)
    {
        reader.setup();
        reader.read( v );
        reader.close();

        return reader;
    }

} // namespace protean
