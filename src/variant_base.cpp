#include <protean/variant.hpp>

#include <boost/algorithm/string/predicate.hpp>

namespace protean {

    variant_base::variant_base()
    {
    }

    // not sure if this method should try and default construct any type.. maybe the collections would do..
    variant_base::variant_base(enum_type_t type, size_t size)
    {
        if ((type & Number)!=0)
        {
            m_value.set<UInt64>(0);
        }
        else
        {
            switch( type )
            {
                case None:
					m_value.set<None>(boost::blank());
					break;
                case Date:
                    m_value.set<Date>(boost::gregorian::date());
                    break;
                case Time:
                    m_value.set<Time>(boost::posix_time::time_duration());
                    break;
                case DateTime:
                    m_value.set<DateTime>(boost::posix_time::ptime());
                    break;
                case TimeSeries:
                    m_value.set<TimeSeries>(detail::timeseries());
                    break;
                case Any:
                    m_value.set<Any>(detail::string());
                    break;
                case String:
                    m_value.set<String>(detail::string());
                    break;
                case Dictionary:
                    m_value.set<Dictionary>(detail::dictionary());
                    break;
                case Bag:
                    m_value.set<Bag>(detail::bag());
                    break;
                case List:
                    m_value.set<List>(detail::list(size));
                    break;
                case Tuple:
                    m_value.set<Tuple>(detail::tuple(size));
                    break;
                case Buffer:
                    m_value.set<Buffer>(handle<detail::buffer>(new detail::buffer(size)));
                    break;
				default:
					boost::throw_exception(variant_error(std::string("Cannot default construct variant of type ") + enum_to_string(type)));
            }
        }
    }

    variant_base::variant_base(enum_type_t type, const variant_base& rhs)
    {
        if ((type & Number)!=0)
        {
            m_value.set<UInt64>(rhs.m_value.get<UInt64>());
        }
        else
        {
            switch(type)
            {
                case None:
					m_value.set<None>(rhs.m_value.get<None>());
					break;
                case Any:
                    m_value.set<Any>(rhs.m_value.get<Any>());
                    break;
                case String:
                    m_value.set<String>(rhs.m_value.get<String>());
                    break;
                case Date:
                    m_value.set<Date>(rhs.m_value.get<Date>());
                    break;
                case Time:
                    m_value.set<Time>(rhs.m_value.get<Time>());
                    break;
                case DateTime:
                    m_value.set<DateTime>(rhs.m_value.get<DateTime>());
                    break;
                case List:
                    m_value.set<List>(rhs.m_value.get<List>());
                    break;
                case Dictionary:
                    m_value.set<Dictionary>(rhs.m_value.get<Dictionary>());
                    break;
                case Bag:
                    m_value.set<Bag>(rhs.m_value.get<Bag>());
                    break;
                case TimeSeries:
                    m_value.set<TimeSeries>(rhs.m_value.get<TimeSeries>());
                    break;
                case Buffer:
                    m_value.set<Buffer>(rhs.m_value.get<Buffer>());
                    break;
                case Tuple:
                    m_value.set<Tuple>(rhs.m_value.get<Tuple>());
                    break;
                case Object:
                    m_value.set<Object>(rhs.m_value.get<Object>());
                    break;
                case Exception:
                    m_value.set<Exception>(rhs.m_value.get<Exception>());
                    break;
				default:
					boost::throw_exception(variant_error(std::string("Attempt to copy unknown variant type ") + enum_to_string(type)));
            }
        }
    }

    void variant_base::destroy(enum_type_t type)
    {
        if ((type & Number)!=0)
        {
            m_value.set<UInt64>(0);
        }
        else
        {
            switch( type )
            {
                case None:
                    m_value.destroy<None>();
                    break;
                case Any:
                    m_value.destroy<Any>();
                    break;
                case String:
                    m_value.destroy<String>();
                    break;
                case Date:
                    m_value.destroy<Date>();
                    break;
                case Time:
                    m_value.destroy<Time>();
                    break;
                case DateTime:
                    m_value.destroy<DateTime>();
                    break;
                case List:
                    m_value.destroy<List>();
                    break;
                case Dictionary:
                    m_value.destroy<Dictionary>();
                    break;
                case Bag:
                    m_value.destroy<Bag>();
                    break;
                case TimeSeries:
                    m_value.destroy<TimeSeries>();
                    break;
                case Buffer:
                    m_value.destroy<Buffer>();
                    break;
                case Tuple:
                    m_value.destroy<Tuple>();
                    break;
                case Object:
                    m_value.destroy<Object>();
                    break;
                case Exception:
                    m_value.destroy<Exception>();
                    break;
				default:
					boost::throw_exception(variant_error(std::string("Attempt to destruct unknown variant type ") + enum_to_string(type)));
            }
        }
    }

    std::string variant_base::enum_to_string(enum_type_t type)
    {
        return variant_impl_t::enum_to_string(type);
    }
    variant_base::enum_type_t variant_base::string_to_enum(const std::string& type)
    {
        return static_cast<variant_base::enum_type_t>(variant_impl_t::string_to_enum(type));
    }

} // namespace protean
