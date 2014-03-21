//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#if defined(_MSC_VER)
#    pragma warning(disable:4503 4996)
#endif

#include <protean/binary_reader.hpp>
#include <protean/variant.hpp>
#include <protean/object_proxy.hpp>
#include <protean/object_factory.hpp>
#include <protean/variant_base.hpp>
#include <protean/detail/data_table.hpp>
#include <protean/detail/data_table_column_serializers.hpp>

#include <boost/foreach.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/scoped_array.hpp>

namespace protean {
    
    binary_reader::binary_reader(std::istream &is, int mode) :
        m_is(is),
        m_mode(mode),
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
                boost::int32_t i;
                read( i ); value = i;
                break;
            }
            case variant::UInt32:
            {
                boost::uint32_t i;
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
                boost::int64_t i;
                read( i ); value = i;
                break;
            }
            case variant::UInt64:
            {
                boost::uint64_t i;
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
                read( v );
                
                value = v;

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
                boost::uint32_t size; read( size );

                for (boost::uint32_t i=0; i<size; ++i)
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
                boost::uint32_t size;
                read(size);
                value = variant(static_cast<variant::enum_type_t>(type), size);

                for (boost::uint32_t i=0; i<size; ++i)
                {
                    read(value[i]);
                }
                break;
            }
            case variant::Buffer:
            {
                boost::uint32_t size;
                read(size);

                value = variant(variant::Buffer, size);
                void* data = (void*)value.as<void*>();
                read(data, size);

                break;
            }
            case variant::TimeSeries:
            {
                value = variant(static_cast<variant::enum_type_t>(type));
                boost::uint32_t size; read(size);

                for (boost::uint32_t i=0; i<size; ++i)
                {
                    variant::date_time_t date_time;
                    read(date_time);

                    variant &val = value.push_back(date_time, variant(), variant::ReturnItem);
                    read(val);
                }
                break;
            }
            case variant::DataTable:
            {
                boost::int32_t numRows, numCols;
                read( numCols );
                read( numRows );

                value = variant( variant::DataTable, numRows );

				{
					std::vector<boost::int32_t> colTypes( numCols );
					BOOST_FOREACH( boost::int32_t& colType, colTypes )
						read( colType );

					std::vector<std::string> colNames( numCols );
					BOOST_FOREACH( std::string& colName, colNames )
						read( colName );

					for ( boost::int32_t i( 0 ); i != numCols; ++i )
						value.add_column( static_cast<variant::enum_type_t>( colTypes[i] ), colNames[i] );
				}

                detail::data_table& dt = value.m_value.get<variant::DataTable>();
                BOOST_FOREACH(detail::data_table::column_container_type::reference column, dt.columns())
                {
                    // Default-allocate `rows' column values to be read into (no reallocation required since
                    // capacity of `rows' was specified in DataTable construction)
                    column.resize( numRows );

                    if (column.type() & variant_base::Primitive)
                    {
                        boost::scoped_ptr<detail::data_table_column_reader> column_reader(
                            detail::make_data_table_column_binary_reader(column, *this)
                        );

                        while (column_reader->has_next())
                        {
                            column_reader->read();
                            column_reader->advance();
                        }
                    }
                    else
                    {
                        variant::iterator iter(column.begin());
                        variant::iterator end(column.end());
                        while (iter != end)
                            read(*(iter++));
                    }
                }

                break;
            }
            case variant::Object:
            {
                std::string class_name;
                boost::int32_t version;

                read(class_name);
                read(version);

                handle<object> obj;
                if (m_factory!=NULL)
                {
                    obj = m_factory->create_instance(class_name);
                    if (obj.null())
                    {
                        if ((m_mode & binary_mode::CreateProxy)!=0)
                        {
                            boost::throw_exception(variant_error("Unable to create object from factory"));
                        }
                    }
                }

                if (obj.null())
                {
                    obj = new object_proxy(class_name);
                }

                // read parameter dictionary
                variant params;
                read(params);
                obj->inflate(params, version);

                value = variant(obj);
        
                break;
            }
            case variant::Exception:
            {
                std::string type, message, source, stack;
                read(type);
                read(message);
                read(source);
                read(stack);
                value = exception_data(type, message, source, stack);
                break;
            }
            case variant::None:
                break;
            case variant::Any:
            {
                std::string s;
                read(s);
                value = variant(s);
                value.m_type = variant::Any;
                break;
            }
            case variant::Array:
            {
                boost::uint32_t size;
                read(size);
                boost::uint32_t array_type;
                read(array_type);

                handle<typed_array> a(new typed_array(size, static_cast<variant::enum_type_t>(array_type)));
                for (boost::uint32_t i=0; i<size; ++i)
                {
                    variant v;
                    read_value(static_cast<variant::enum_type_t>(array_type), v);
                    a->at(i).swap(v);
                }
                value = a;
                break;
            }
        default:        
            boost::throw_exception(variant_error("Case exhaustion: " + variant::enum_to_string(type)));        
        }
    }

    void binary_reader::read(variant& value)
    {
        boost::uint32_t type = variant::None;
        read(type);
        read_value(static_cast<variant::enum_type_t>(type), value);
    }
    void binary_reader::read(std::string& value)
    {
        boost::uint32_t length;
        read(length);

        boost::scoped_array<char> buffer( new char[length] );
        read_bytes(buffer.get(), length);
        value = std::string(buffer.get(), length);

    }
    void binary_reader::read(detail::string& value)
    {
        boost::uint32_t length;
        read(length);

        boost::scoped_array<char> buffer( new char[length] );
        read_bytes(buffer.get(), length);
        value.initialise(buffer.get(), length);
    }
    void binary_reader::read(bool& value)
    {
        boost::int32_t b;
        read(b);
        value = (b != 0);
    }
    void binary_reader::read(boost::int32_t& value)
    {
        read_bytes(reinterpret_cast<char*>(&value), 4 );
    }
    void binary_reader::read(boost::uint32_t& value )
    {
        read_bytes(reinterpret_cast<char*>(&value), 4 );
    }
    void binary_reader::read(boost::int64_t& value )
    {
        read_bytes(reinterpret_cast<char*>(&value), 8 );
    }
    void binary_reader::read(boost::uint64_t& value )
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
        if ((m_writer_mode & binary_mode::DateTimeAsTicks)!=0)
        {
            boost::uint32_t total_days;
            read(total_days);

            value = variant::min_date() + boost::gregorian::date_duration(total_days);
        }
        else
        {
            binary_date_t date;
            read_bytes(reinterpret_cast<char*>(&date), sizeof(binary_date_t));

            value = variant::date_t( date.year, date.month, date.day );
        }
    }
    void binary_reader::read(variant::time_t& value)
    {
        if ((m_writer_mode & binary_mode::DateTimeAsTicks)!=0)
        {
            boost::int64_t total_millis;
            read(total_millis);

            // Bit nasty! Can't simply do "value = boost::posix_time::millisec(total_millis);" because of:
            //  https://svn.boost.org/trac/boost/ticket/3471
            // Hopefully this will be fixed for v1.44.
            boost::uint32_t hours = total_millis / 3600000;
            boost::uint32_t tmp = total_millis % 3600000;
            boost::uint32_t mins = tmp / 60000;
            tmp %= 60000;
            boost::uint32_t secs = tmp / 1000;
            boost::uint32_t millis = tmp % 1000;

            value = boost::posix_time::time_duration(hours, mins, secs) + boost::posix_time::millisec(millis);
        }
        else
        {
            binary_time_t time;
            read_bytes(reinterpret_cast<char*>(&time), sizeof(binary_time_t));

            value = variant::time_t(time.hour, time.minute, time.second);
        }
    }
    void binary_reader::read(variant::date_time_t& value)
    {
        if ((m_writer_mode & binary_mode::DateTimeAsTicks)!=0)
        {
            variant::time_t duration;
            read(duration);

            value = variant::min_date_time() + duration;
        }
        else
        {
            variant::date_t date;
            variant::time_t time;

            read(date);
            read(time);

            value = variant::date_time_t(date, time);
        }
    }
    void binary_reader::read(void*& data, size_t length)
    {
        // 'data' will be owned by the variant
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
        int header[3];
        if (!m_is.read(reinterpret_cast<char*>(header), sizeof(header)))
        {
            boost::throw_exception(variant_error("Error reading header from stream"));
        }

        // check magic number
        if (header[0]!=static_cast<int> (binary_magic_number))
        {
            boost::throw_exception(variant_error("Bad magic number, this looks like invalid binary data"));
        }

        // store version information
        m_major_version = (header[1] >> 16) & 0x0000FFFF;
        m_minor_version = header[1] & 0x0000FFFF;

        if (m_major_version>binary_major_version)
        {
            boost::throw_exception(variant_error((boost::format("Version of received binary data is not compatible, received version %d > this version %d")
                % static_cast<int>(m_major_version)
                % static_cast<int>(binary_major_version)
            ).str()));
        }

        m_writer_mode = header[2];

        // create compression filter if necessary
        if ((m_writer_mode & binary_mode::Compress)!=0)
        {
            bool zlib_no_header = (m_writer_mode & binary_mode::ZlibHeader) == 0;
            m_filter.push(boost::iostreams::zlib_decompressor(binary_compression_params(zlib_no_header)));
        }
        m_filter.push(m_is);
    }

    void binary_reader::close()
    {
        m_filter.reset();
    }

	bool binary_reader::eof() const
	{
		return m_filter.eof();
	}

    binary_reader& operator>>(binary_reader& reader, variant& v)
    {
        reader.setup();
        reader.read( v );
        reader.close();

        return reader;
    }

} // namespace protean
