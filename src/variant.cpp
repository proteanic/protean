//  (C) Copyright Johan Ditmar, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#include <boost/foreach.hpp>
#include <boost/regex.hpp>
#include <boost/format.hpp>
#include <boost/math/special_functions/fpclassify.hpp>
#include <boost/functional/hash.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include <protean/variant.hpp>
#include <protean/detail/hash.hpp>

#include <protean/detail/dummy_iterator.hpp>

#include <protean/detail/variant_macros_define.hpp>

namespace protean {

    /* Constructors */
    variant::variant() :
        m_type(None)
    {
    }
    variant::variant(const std::string& arg) :
        m_type(String)
    {
        m_value.set<String>(detail::string(arg.c_str(), arg.size()));
    }
    variant::variant(const char *arg) :
        m_type(String)
    {
        m_value.set<String>(detail::string(arg));
    }
    variant::variant(bool arg) :
        m_type(Boolean)
    {
        m_value.set<Boolean>(arg);
    }

    variant::variant(const date_t& arg) :
        m_type(Date)
    {
        if (arg.is_special())
        {
            boost::throw_exception(variant_error("Attempt to construct Date variant with invalid date"));
        }
        m_value.set<Date>(arg);
    }
    variant::variant(const time_t& arg) :
        m_type(Time)
    {
        if ( arg.is_special() )
        {
            boost::throw_exception(variant_error("Attempt to construct Time variant with invalid time"));
        }
        m_value.set<Time>(arg);
    }
    variant::variant(const date_time_t& arg) :
        m_type(DateTime)
    {
        if (arg.is_special())
        {
            boost::throw_exception(variant_error("Attempt to construct DateTime variant with invalid date/time"));
        }
        m_value.set<DateTime>(arg);
    }

    variant::variant(void* data, size_t size, bool copy_data) :
        m_type(Buffer)
    {
        m_value.set<Buffer>(detail::buffer(data, size, copy_data));
    }

    variant::variant(const exception_info& arg) :
        m_type(Exception)
    {
        m_value.set<Exception>(arg);
    }

    variant::variant(const std::exception& arg) :
        m_type(Exception)
    {
        m_value.set<Exception>(exception_info(arg));
    }

    variant::variant(const object& arg) :
        m_type(Object)
    {
        m_value.set<Object>(arg.clone());
    }

    variant::variant(const handle<object>& arg) :
        m_type(Object)
    {
        m_value.set<Object>(arg);
    }

    variant::variant(const variant& rhs) :
        variant_base(rhs.m_type, rhs),
        m_type(rhs.m_type)
    {
    }

    void variant::swap(variant& rhs)
    {
        m_value.swap(rhs.m_value);
        std::swap(m_type, rhs.m_type);
    }

    variant& variant::operator=(const variant &value)
    {
        variant temp(value);
        swap(temp);
        return *this;
    }

    variant::variant(enum_type_t type, size_t size) :
        variant_base(type, size),
        m_type(type)
    {
    }

    variant::variant(enum_type_t type, const variant& rhs)
    {
        boost::throw_exception(variant_error("TODO"));

		if (type==rhs.type())
		{
            variant temp(rhs);
            swap(temp);
		}
    }

    variant::~variant()
    {
        destroy(m_type);
    }

    template<typename T>
    static int compare_using_less(const T& lhs, const T& rhs)
    {
        if (lhs==lhs)
        {
            return 0;
        }
        else
        {
            return lhs<rhs ? -1 : 1;
        }
    }

    int variant::compare( const variant& rhs ) const
    {
        if ( m_type == rhs.type() )
        {
            // structures are of same type
            switch (m_type)
            {
            case None:
                return 0;
            case Any:
                return m_value.get<Any>().compare(rhs.m_value.get<Any>());
            case String:
                return m_value.get<String>().compare(rhs.m_value.get<String>());
            case Int32:
                return compare_using_less(m_value.get<Int32>(), rhs.m_value.get<Int32>());
            case UInt32:
                return compare_using_less(m_value.get<UInt32>(), rhs.m_value.get<UInt32>());
            case Int64:
                return compare_using_less(m_value.get<Int64>(), rhs.m_value.get<Int64>());
            case UInt64:
                return compare_using_less(m_value.get<UInt64>(), rhs.m_value.get<UInt64>());
            case Float:
                return compare_using_less(m_value.get<Float>(), rhs.m_value.get<Float>());
            case Double:
                return compare_using_less(m_value.get<Double>(), rhs.m_value.get<Double>());
            case Boolean:
                if ( m_value.get<Boolean>()==rhs.m_value.get<Boolean>() )
                    return 0;
                else if ( m_value.get<Boolean>() )
                    return 1;
                else
                    return -1;
                break;
            case Date:
                return compare_using_less(m_value.get<Date>(), rhs.m_value.get<Date>());
            case Time:
                return compare_using_less(m_value.get<Time>(), rhs.m_value.get<Time>());
            case DateTime:
                return compare_using_less(m_value.get<DateTime>(), rhs.m_value.get<DateTime>());
            case Buffer:
                return m_value.get<Buffer>().compare(rhs.m_value.get<Buffer>());
            case List:
            case Tuple:
            case Dictionary:
            case Bag:
            case TimeSeries:
                return m_value.get<Collection>().compare(rhs.m_value.get<Collection>());
            case Exception:
                return m_value.get<Exception>().compare(rhs.m_value.get<Exception>());
            case Object:
                return m_value.get<Object>()->compare( *rhs.m_value.get<Object>() );
                break;
            default:
                boost::throw_exception(variant_error("Unrecognised variant type " + enum_to_string(m_type)));
            }
        }
        else
        {
             return compare_using_less(m_type, rhs.type());
        }
    }

    bool variant::operator<(const variant& rhs) const
    {
        return compare(rhs) < 0;
    }

    bool variant::operator==(const variant& rhs) const
    {
        return compare(rhs) == 0;
    }

    bool variant::operator!=(const variant& rhs) const
    {
        return compare(rhs) != 0;
    }

    bool variant::empty() const
    {
        BEGIN_VARIANT_CONTEXT();

        CHECK_VARIANT_FUNCTION(Collection | Any, "empty()");

        if (is<Any>())
        {
            return m_value.get<Any>().empty();
        }
        else
        {
            return m_value.get<Collection>().empty();
        }

        END_VARIANT_CONTEXT();
    }

    size_t variant::size() const
    {
        BEGIN_VARIANT_CONTEXT();

        CHECK_VARIANT_FUNCTION(Collection | Buffer, "size()");

        if (is<Collection>())
        {
            return m_value.get<Collection>().size();
        }
        else
        {
            return m_value.get<Buffer>().size();
        }

        END_VARIANT_CONTEXT();
    }

    void variant::clear()
    {
        BEGIN_VARIANT_CONTEXT();

        CHECK_VARIANT_FUNCTION(Collection, "clear()");

        m_value.get<Collection>().clear();

        END_VARIANT_CONTEXT();
    }

    variant& variant::push_back(const variant& value, enum_return_trait_t ret)
    {
        BEGIN_VARIANT_CONTEXT();

        CHECK_VARIANT_FUNCTION(List, "push_back()");

        variant& result = m_value.get<List>().push_back(value);

        return (ret==ReturnSelf ? *this : result);

        END_VARIANT_CONTEXT();
    }

    variant& variant::pop_back()
    {
        BEGIN_VARIANT_CONTEXT();

        CHECK_VARIANT_FUNCTION(List, "pop_back()");

        m_value.get<List>().pop_back();
        return *this;

        END_VARIANT_CONTEXT();
    }

    variant& variant::push_back(const date_time_t& time, const variant &value, enum_return_trait_t ret)
    {
        BEGIN_VARIANT_CONTEXT();

        CHECK_VARIANT_FUNCTION(TimeSeries, "pop_back()");

        variant& result = m_value.get<TimeSeries>().push_back(time, value);

        return (ret==ReturnSelf ? *this : result);

        END_VARIANT_CONTEXT();
    }

    variant& variant::insert(const std::string& key, const variant& value, enum_return_trait_t ret)
    {
        BEGIN_VARIANT_CONTEXT();

        CHECK_VARIANT_FUNCTION(Mapping, "insert(\"" + key + "\")")

        variant& result = m_value.get<Mapping>().insert(key, value);

        return (ret==ReturnSelf ? *this : result);

        END_VARIANT_CONTEXT();
    }
    variant& variant::remove(const std::string& key)
    {
        BEGIN_VARIANT_CONTEXT();

        CHECK_VARIANT_FUNCTION(Mapping, "remove(\"" + key + "\")")

        m_value.get<Mapping>().remove(key);
        return *this;

        END_VARIANT_CONTEXT();
    }
    bool variant::has_key(const std::string& key) const
    {
        BEGIN_VARIANT_CONTEXT();

        CHECK_VARIANT_FUNCTION(Mapping, "has_key(\"" + key + "\")")

        return m_value.get<Mapping>().has_key(key);

        END_VARIANT_CONTEXT();
    }
    const variant& variant::at(size_t index) const
    {
        BEGIN_VARIANT_CONTEXT();

        CHECK_VARIANT_FUNCTION(Sequence, "at()")

        return m_value.get<Sequence>().at(index);

        END_VARIANT_CONTEXT();
    }
    variant& variant::at(size_t index)
    {
        BEGIN_VARIANT_CONTEXT();

        CHECK_VARIANT_FUNCTION(Sequence, "at()")

        return m_value.get<Sequence>().at(index);

        END_VARIANT_CONTEXT();
    }
    const variant& variant::operator[](size_t index) const
    {
        return at(index);
    }
    variant& variant::operator[](size_t index)
    {
        return at(index);
    }
    const variant& variant::at(const std::string& key) const
    {
        BEGIN_VARIANT_CONTEXT();

        CHECK_VARIANT_FUNCTION(Mapping, "at(\"" + key + "\")");

        return m_value.get<Mapping>().at(key);

        END_VARIANT_CONTEXT();
    }

    variant& variant::at(const std::string& key)
    {
        BEGIN_VARIANT_CONTEXT();

        CHECK_VARIANT_FUNCTION(Mapping, "at(\"" + key + "\")");

        return m_value.get<Mapping>().at(key);

        END_VARIANT_CONTEXT();
    }

    const variant& variant::operator[](const std::string& key) const
    {
        return at(key);
    }

    variant& variant::operator[](const std::string& key)
    {
        return at(key);
    }

    variant variant::range(const std::string& key) const
    {
        BEGIN_VARIANT_CONTEXT();

        CHECK_VARIANT_FUNCTION(Mapping, "range(\"" + key + "\")");

        return m_value.get<Mapping>().range(key);
        
        END_VARIANT_CONTEXT();
    }

    variant variant::select(const std::string& path) const
    {
        variant result(variant::List);
        select_impl(*this, path, result);
        return result;
    }

    /*static*/ void variant::select_impl(const variant &input, const std::string& path, variant& result)
    {   
        std::string head, tail;

        size_t head_start(path.find_first_not_of('/'));
        if (head_start!=std::string::npos)
        {
            size_t head_end(path.find_first_of('/', head_start));
            head = path.substr(head_start, head_end-head_start);
            if (head_end!=std::string::npos)
            {
                tail = path.substr(head_end+1);
            }
        }

        if (head.empty())
        {
            result.push_back(input);
            return;
        }

        std::string node, pred_key, pred_val;

        static const boost::regex expr("^(\\*|\\w+)(?:\\[@?(\\w+)=\"?(\\w[\\s\\w]+)\"?\\])?");
        boost::smatch what;
        if (boost::regex_match(head, what, expr))
        {
            node = what[1].str();
            pred_key = what[2].str();
            pred_val = what[3].str();
        }
        else 
        {
            boost::throw_exception(variant_error(std::string("Select path has invalid syntax: ") + head));
        }

        variant nodes(variant::List);
        if (node.empty() || node=="*")
        {
            BOOST_FOREACH(const variant& i, input)
            {
                nodes.push_back(i);
            }
        }
        else if (input.is<Mapping>())
        {
            BOOST_FOREACH(const variant& i, input.range(node))
            {
                nodes.push_back(i);
            }
        }

        BOOST_FOREACH(const variant& i, nodes)
        {
            if (pred_key.empty() || (i.is<Mapping>() && i.has_key(pred_key) && i.at(pred_key).up_cast().as<std::string>()==pred_val))
            {
                select_impl(i, tail, result);
            }
        }
    }

    variant::const_iterator variant::begin() const
    {
        BEGIN_VARIANT_CONTEXT();

        CHECK_VARIANT_FUNCTION(Any | Collection, "begin()");

        if (is<Any>())
        {
            if (m_value.get<Any>().empty())
            {
                return const_iterator(new detail::dummy_const_iterator());
            }
            else
            {
                boost::throw_exception(variant_error("Attempt to call begin() on non-empty Any variant"));
            }
        }
        else
        {
            return m_value.get<Collection>().begin();
        }

        END_VARIANT_CONTEXT();
    }
    variant::const_iterator variant::end() const
    {
        BEGIN_VARIANT_CONTEXT();

        CHECK_VARIANT_FUNCTION(Any | Collection, "end()");

        if (is<Any>())
        {
            if (m_value.get<Any>().empty())
            {
                return const_iterator(new detail::dummy_const_iterator());
            }
            else
            {
                boost::throw_exception(variant_error("Attempt to call end() on non-empty Any variant"));
            }
        }
        else
        {
            return const_iterator( m_value.get<Collection>().end() );
        }

        END_VARIANT_CONTEXT();
    }
    variant::iterator variant::begin()
    {
        BEGIN_VARIANT_CONTEXT();

        CHECK_VARIANT_FUNCTION(Any | Collection, "begin()");

        if (is<Any>())
        {
            if (m_value.get<Any>().empty())
            {
                return iterator(new detail::dummy_iterator());
            }
            else
            {
                boost::throw_exception(variant_error("Attempt to call begin() on non-empty Any variant"));
            }
        }
        else
        {
            return iterator(m_value.get<Collection>().begin());
        }

        END_VARIANT_CONTEXT();
    }
    variant::iterator variant::end()
    {
        BEGIN_VARIANT_CONTEXT();

        CHECK_VARIANT_FUNCTION(Any | Collection, "end()");

        if (is<Any>())
        {
            if (m_value.get<Any>().empty())
            {
                return iterator(new detail::dummy_iterator());
            }
            else
            {
                boost::throw_exception(variant_error("Attempt to call end() on non-empty Any variant"));
            }
        }
        else
        {
            return iterator( m_value.get<Collection>().end() );
        }

        END_VARIANT_CONTEXT();
    }

    /*
     * variant::lexical_cast
     */
    template<> bool variant::lexical_cast<bool>(const std::string &arg)
    {
        if (boost::algorithm::iequals(arg, "true") || boost::algorithm::iequals(arg, "1"))
        {
            return true;
        }
        else if (boost::algorithm::iequals(arg, "false") || boost::algorithm::iequals(arg, "0"))
        {
            return false;
        }
        else
        {
            boost::throw_exception(variant_error(std::string("Bad lexical cast: failed to interpret ") + arg + " as bool"));
        }
    }
    template<> float variant::lexical_cast<float>(const std::string &arg)
    {
        if (arg=="NaN")
        {
            return std::numeric_limits<float>::quiet_NaN();
        }
        else if (arg=="-INF")
        {
            return std::numeric_limits<float>::min();
        }
        else if (arg=="INF")
        {
            return std::numeric_limits<float>::max();
        }
        else
        {
            try
            {
                return boost::lexical_cast<float>(arg);
            }
            catch(const boost::bad_lexical_cast&)
            {
                boost::throw_exception(variant_error(std::string("Bad lexical cast: failed to interpret ") + arg + " as float"));
            }
        }
    }
    template<> double variant::lexical_cast<double>(const std::string &arg)
    {
        if (arg=="NaN")
        {
            return std::numeric_limits<double>::quiet_NaN();
        }
        else if (arg=="-INF")
        {
            return std::numeric_limits<double>::min();
        }
        else if (arg=="INF")
        {
            return std::numeric_limits<double>::max();
        }
        else
        {
            try
            {
                return boost::lexical_cast<double>(arg);
            }
            catch(const boost::bad_lexical_cast&)
            {
                boost::throw_exception(variant_error(std::string("Bad lexical cast: failed to interpret ") + arg + " as double"));
            }
        }
    }
    template<> variant::date_t variant::lexical_cast<variant::date_t>(const std::string &arg)
    {
        const boost::regex expr( "^\\d{4}-\\d{2}-\\d{2}$" );
        if ( boost::regex_match(arg, expr) )
        {
            variant::date_t value = boost::gregorian::from_string(arg);
            if (!value.is_special()) return value;
        }
        boost::throw_exception(variant_error(std::string("Bad lexical cast: invalid date '") + arg + "' specified, expecting YYYY-MM-DD"));
    }

    template<> variant::time_t variant::lexical_cast<variant::time_t>(const std::string &arg)
    {
        const boost::regex expr( "^\\d{2}:\\d{2}:\\d{2}(.\\d*)?$" );
        if ( boost::regex_match(arg, expr ))
        {
            variant::time_t value = boost::posix_time::duration_from_string(arg);
            if (!value.is_special()) return value;
        }
        else
        {
            // We assume this is a time duration (ISO 8601) under the form: PnYnMnDTnHnMnS
            const boost::regex expr( "^(-)?P(?:([0-9]+)Y)?(?:([0-9]+)M)?(?:([0-9]+)D)?(?:T(?:([0-9]+)H)?(?:([0-9]+)M)?(?:([0-9]+)S)?)?(?:.([0-9]+))?$" );

            boost::smatch match;
            boost::regex_search(arg, match, expr);
            if ( match[0].matched )
            {
                bool sign = match[1].first!=match[1].second;
                long years = (match[2].first!=match[2].second) ? boost::lexical_cast<long>(match[2]) : 0;
                long months = (match[3].first!=match[3].second) ? boost::lexical_cast<long>(match[3]) : 0;

                if ( years!=0 || months!=0 )
                {
                    boost::throw_exception(variant_error(std::string("Unable to interpret '") + arg + "' as duration, hours or years > 0"));
                }

                long days = (match[4].first!=match[4].second) ? boost::lexical_cast<long>(match[4]) : 0;
                long hours = (match[5].first!=match[5].second) ? boost::lexical_cast<long>(match[5]) : 0;
                hours += days*24;

                long mins = (match[6].first!=match[6].second) ? boost::lexical_cast<long>(match[6]) : 0;
                long secs = (match[7].first!=match[7].second) ? boost::lexical_cast<long>(match[7]) : 0;
            
                long fracs = (match[8].first!=match[8].second) ? boost::lexical_cast<long>(match[8]) : 0;

                if (fracs!=0)
                {
                    boost::throw_exception(variant_error(std::string("Unable to interpret '") + arg + "' as duration, fractional seconds > 0"));
                }

                variant::time_t value = time_t( hours, mins, secs );
                
                if (sign) value = value.invert_sign();

                if ( !value.is_special() ) return value;
            }
        }
        boost::throw_exception(variant_error(std::string("Bad lexical cast: invalid time '") + arg + "' specified, expecting expecting HH:MM:SS[.fff] or P[n]Y[n]M[n]DT[n]H[n]M[n]S"));
    }

    template<> variant::date_time_t variant::lexical_cast<variant::date_time_t>(const std::string &arg)
    {
        const boost::regex expr( "^(.+)T(.+)$" );
        boost::smatch matches;
        if (boost::regex_match(arg, matches, expr))
        {
            variant::date_t date = lexical_cast<variant::date_t>( matches[1].str() );
            variant::time_t time = lexical_cast<variant::time_t>( matches[2].str() );
            variant::date_time_t value( date, time );

            if ( !value.is_special() ) return value;
        }
        boost::throw_exception(variant_error(std::string("Bad lexical cast: invalid date/time '") + arg + "' specified, expecting expecting YYYY-MM-DDTHH:MM:SS"));
    }

    template<> std::string variant::lexical_cast<std::string>(const bool &arg)
    {
        return (arg ? "true" : "false");
    }
    template<> std::string variant::lexical_cast<std::string>(const float &arg)
    {
        if ( boost::math::fpclassify(arg)==FP_NAN )
        {
            return "NaN";
        }
        else if (arg==std::numeric_limits<float>::max())
        {
            return "INF";
        }
        else if (arg==std::numeric_limits<float>::min())
        {
            return "-INF";
        }
        else
        {
            try
            {
                return boost::lexical_cast<std::string>(arg);
            }
            catch(const boost::bad_lexical_cast&)
            {
                boost::throw_exception(variant_error((boost::format("Bad lexical cast: failed to interpret %f as std::string") % arg).str()));
            }
        }
    }
    template<> std::string variant::lexical_cast<std::string>(const double& arg)
    {
        if (boost::math::fpclassify(arg)==FP_NAN)
        {
            return "NaN";
        }
        else if (arg==std::numeric_limits<double>::max())
        {
            return "INF";
        }
        else if (arg==std::numeric_limits<double>::min())
        {
            return "-INF";
        }
        else
        {
            try
            {
                return boost::lexical_cast<std::string>(arg);
            }
            catch(const boost::bad_lexical_cast&)
            {
                boost::throw_exception(variant_error((boost::format("Bad lexical cast: failed to interpret %f as std::string") % arg).str()));
            }
        }
    }
    template<> std::string variant::lexical_cast<std::string>(const date_t& arg)
    {
        if (arg.is_special())
        {
            boost::throw_exception(variant_error("Bad lexical cast: invalid date"));
        }
        return boost::gregorian::to_iso_extended_string(arg);
    }
    template<> std::string variant::lexical_cast<std::string>(const time_t& arg)
    {
        if(arg.is_special())
        {
            boost::throw_exception(variant_error("Bad lexical cast: invalid time"));
        }
        return boost::posix_time::to_simple_string(arg);
    }
    template<> std::string variant::lexical_cast<std::string>(const date_time_t& arg)
    {
        if (arg.is_special())
        {
            boost::throw_exception(variant_error("Bad lexical cast: invalid date/time"));
        }
        return boost::posix_time::to_iso_extended_string(arg);
    }

    variant variant::down_cast(enum_type_t type) const
    {
        BEGIN_VARIANT_CONTEXT();

        CHECK_VARIANT_FUNCTION(Primitive, "down_cast()");

        if ( type==Any )
        {
            return *this;
        }
        else if ( (type & Primitive)!=0 )
        {
            std::string data = as<std::string>();

            variant result;
            switch ( type )
            {
            case String:
                result = data;
                break;
            case Int32:
                result = lexical_cast<boost::int32_t>( data );
                break;
            case UInt32:
                result = lexical_cast<boost::uint32_t>( data );
                break;
            case Int64:
                result = lexical_cast<boost::int64_t>( data );
                break;
            case UInt64:
                result = lexical_cast<boost::uint64_t>( data );
                break;
            case Float:
                result = lexical_cast<float>( data );
                break;
            case Double:
                result = lexical_cast<double>( data );
                break;
            case Boolean:
                result = lexical_cast<bool>( data );
                break;
            case Date:
                result = lexical_cast<date_t>( data );
                break;
            case Time:
                result = lexical_cast<time_t>( data );
                break;
            case DateTime:
                result = lexical_cast<date_time_t>( data );
                break;
             default:
                boost::throw_exception(variant_error("Unrecognised variant type"));
            }
            return result;
        }
        else
        {
            boost::throw_exception(variant_error(std::string("Attempt to cast Any variant to non-primitive type ") + enum_to_string(type)));
        }

        END_VARIANT_CONTEXT();
    }

    variant variant::up_cast() const
    {
        BEGIN_VARIANT_CONTEXT();

        CHECK_VARIANT_FUNCTION(Primitive, "up_cast()");

        if ( m_type==Any )
        {
            return *this;
        }
        else
        {
            std::string data;

            switch ( m_type )
            {
            case String:
                data = as<std::string>();
                break;
            case Int32:
                data = lexical_cast<std::string>( m_value.get<Int32>() );
                break;
            case UInt32:
                data = lexical_cast<std::string>( m_value.get<UInt32>() );
                break;
            case Int64:
                data = lexical_cast<std::string>( m_value.get<Int64>() );
                break;
            case UInt64:
                data = lexical_cast<std::string>( m_value.get<UInt64>() );
                break;
            case Float:
                data = lexical_cast<std::string>( m_value.get<Float>() );
                break;
            case Double:
                data = (boost::format("%|.20|") % m_value.get<Double>() ).str();
                break;
            case Boolean:
                data = lexical_cast<std::string>( m_value.get<Boolean>() );
                break;
            case Date:
                data = lexical_cast<std::string>( m_value.get<Date>() );
                break;
            case Time:
                data = lexical_cast<std::string>( m_value.get<Time>() );
                break;
            case DateTime:
                data = lexical_cast<std::string>( m_value.get<DateTime>() );
                break;
             default:
                boost::throw_exception(variant_error("Unrecognised variant type"));
            }

            return variant::make_any(data);
        }

        END_VARIANT_CONTEXT();
    }

    /*
     * variant::is
     */
    template<> bool variant::is<std::string>()              const { return m_type==String; }
    template<> bool variant::is<bool>()                     const { return m_type==Boolean; }
    template<> bool variant::is<variant::date_t>()          const { return m_type==Date; }
    template<> bool variant::is<variant::time_t>()          const { return m_type==Time; }
    template<> bool variant::is<variant::date_time_t>()     const { return m_type==DateTime; }
    template<> bool variant::is<void*>()                    const { return m_type==Buffer; }
    template<> bool variant::is<object>()                   const { return m_type==Object; }
    template<> bool variant::is<object_proxy>()             const
    {
        return ( is<Object>() && m_value.get<Object>().is<object_proxy>() );
    }

    /*
     * variant::as
     */
    template<> std::string variant::as<std::string>() const 
    {
        BEGIN_VARIANT_CONTEXT();

        CHECK_VARIANT_FUNCTION(Any | String, "as<std::string>()");

        return m_value.get<String>().value();

        END_VARIANT_CONTEXT();
    }
    template<> bool variant::as<bool>() const
    {
        BEGIN_VARIANT_CONTEXT();

        CHECK_VARIANT_FUNCTION(Any | Boolean, "as<bool>()");

        if (is<Any>())
        {
            return lexical_cast<bool>(m_value.get<Any>().value());
        }
        else
        {
            return m_value.get<Boolean>();
        }

        END_VARIANT_CONTEXT();
    }
    template<> variant::date_t variant::as<variant::date_t>() const
    {
        BEGIN_VARIANT_CONTEXT();

        CHECK_VARIANT_FUNCTION(Any | Date, "as<date_t>()");

        if (is<Any>())
        {
            return lexical_cast<date_t>(m_value.get<Any>().value());
        }
        else
        {
            return m_value.get<Date>();
        }

        END_VARIANT_CONTEXT();
    }
    template<> variant::time_t variant::as<variant::time_t>() const
    {
        BEGIN_VARIANT_CONTEXT();

        CHECK_VARIANT_FUNCTION(Any | Time, "as<time_t>()");

        if (is<Any>())
        {
            return lexical_cast<time_t>(m_value.get<Any>().value());
        }
        else
        {
            return m_value.get<Time>();
        }

        END_VARIANT_CONTEXT();
    }
    template<> variant::date_time_t variant::as<variant::date_time_t>() const
    {
        BEGIN_VARIANT_CONTEXT();

        CHECK_VARIANT_FUNCTION(Any | DateTime, "as<date_time_t>()");

        if (is<Any>())
        {
            return lexical_cast<date_time_t>(m_value.get<Any>().value());
        }
        else
        {
            return m_value.get<DateTime>();
        }

        END_VARIANT_CONTEXT();
    }
    template<> void* variant::as<void*>() const
    {
        BEGIN_VARIANT_CONTEXT();

        CHECK_VARIANT_FUNCTION(Buffer, "as<void*>()");

        return m_value.get<Buffer>().data();

        END_VARIANT_CONTEXT();
    }
    template<> exception_info variant::as<exception_info>() const
    {
        BEGIN_VARIANT_CONTEXT();

        CHECK_VARIANT_FUNCTION(Exception, "as<variant::exception_info>()");

        return m_value.get<Exception>();

        END_VARIANT_CONTEXT();
    }

    template<> const object& variant::as<object>() const
    {
        BEGIN_VARIANT_CONTEXT();

        CHECK_VARIANT_FUNCTION(Object, "as<object>()");

        return *m_value.get<Object>();

        END_VARIANT_CONTEXT();
    }

    template<> object& variant::as<object>()
    {
        BEGIN_VARIANT_CONTEXT();

        CHECK_VARIANT_FUNCTION(Object, "as<object>()");

        handle<object>& obj = m_value.get<Object>();

        if ( obj.unique() )
        {
            return *obj;
        }
        else
        {
            obj = obj->clone();
            return *obj;
        }

        END_VARIANT_CONTEXT();
    }

    std::string variant::str(bool summarise, const std::string& indent) const
    {
        static const std::string tab = "   ";
        static const std::string no_indent = "";
        
        std::ostringstream oss;

        oss << indent;
        switch ( type() )
        {
            case None:
                oss << "None";
                break;
            case Any:
                oss << "Any('" << as<std::string>() << "')";
                break;
            case String:
                oss << "'" << as<std::string>() << "'";
                break;
            case Int32:
            case UInt32:
            case Int64:
            case UInt64:
            case Float:
            case Double:
            case Boolean:
            case Date:
            case Time:
            case DateTime:
                oss << up_cast().as<std::string>();
                break;
            case List:
            {
                if (summarise)
                {
                    oss << "List(size=" << size() << ")";
                }
                else
                {
                    oss << "[\n";
                    variant::const_iterator it, end(variant::end());
                    for( it=begin(); it!=end; )
                    {
                        oss << it->str(false, indent + tab);
                        if (++it!=end)
                        {
                            oss << ",";
                        }
                        oss << "\n";
                    }
                    oss << indent << "]";
                }
                break;
            }
            case Dictionary:
            {
                if (summarise)
                {
                    oss << "Dictionary(size=" << size() << ")";
                }
                else
                {
                    oss << "{\n";
                    variant::const_iterator it, end(variant::end());
                    for( it=begin(); it!=end; )
                    {
                        oss << indent << tab << it.key() << ": ";
                        if (it->is<Primitive>())
                        {
                            oss << it->str(false, no_indent);
                        }
                        else
                        {
                            oss << "\n" << it->str(false, indent + tab + tab);
                        }
                        if (++it!=end)
                        {
                            oss << ",";
                        }
                        oss << "\n";
                    }
                    oss << indent << "}";
                }
                break;
            }
            case Bag:
            {
                if (summarise)
                {
                    oss << "Bag(size=" << size() << ")";
                }
                else
                {
                    oss << "[\n";
                    variant::const_iterator it, end(variant::end());
                    for( it=begin(); it!=end; )
                    {
                        oss << indent << tab << "(" << it.key() << ", ";
                        if (it->is<Primitive>())
                        {
                            oss << it->str(false, no_indent);
                        }
                        else
                        {
                            oss << "\n" << it->str(false, indent + tab);
                        }
                        oss << ")";
                        if (++it!=end)
                        {
                            oss << ",";
                        }
                        oss << "\n";
                    }
                    oss << indent << "]";
                }
                break;
            }
            case TimeSeries:
            {
                if (summarise)
                {
                    oss << "TimeSeries(size=" << size() << ")";
                }
                else
                {
                    oss << "TimeSeries(\n";
                    variant::const_iterator it, end(variant::end());
                    for( it=begin(); it!=end; )
                    {
                        oss << indent << tab << "(" << it.time() << ", ";
                        if (it->is<Primitive>())
                        {
                            oss << it->str(false, no_indent);
                        }
                        else
                        {
                            oss << "\n" << it->str(false, indent + tab);
                        }
                        oss << ")";
                        if (++it!=end)
                        {
                            oss << ",";
                        }
                        oss << "\n";
                    }
                    oss << indent << ")";
                }
                break;
            }
            case Buffer:
            {
                if (summarise)
                {
                    oss << "Buffer(address=0x" << as<void*>() << ", size=" << size() << ")";
                }
                else
                {
                    oss << "Buffer(";

                    if (as<void*>()!=NULL)
                    {
                        const unsigned char* byteArray(reinterpret_cast<const unsigned char*>(as<void*>()));
                        for(size_t i=0; i<size(); ++i)
                        {
                            if (i>0) oss << " ";
                            oss << (boost::format("%02x") % static_cast<int>(byteArray[i])).str();
                        }
                    }
                    else
                    {
                        oss << "NULL";
                    }
                    oss << ')';
                }
                break;
            }
            case Tuple:
            {
                if (summarise)
                {
                    oss << "Tuple(size=" << size() << ")";
                }
                else
                {
                    oss << "(\n";
                    variant::const_iterator it, end(variant::end ());
                    for( it=begin(); it!=end; )
                    {
                        oss << it->str(false, indent + tab);
                        if (++it!=end)
                        {
                            oss << ",";
                        }
                        oss << "\n";
                    }
                    oss << indent << ")";
                }
                break;
            }
            case Exception:
            {
                const exception_info& x(m_value.get<Exception>());
                oss << x.type() << "('" << x.message() << "')";
                break;
            }
            case Object:
            {
                handle<object> obj(m_value.get<Object>());
                if (summarise)
                {
                    oss << obj->name() << "(version=" << obj->version() << ")";
                }
                else
                {
                    variant params;
                    obj->deflate(params);

                    oss << obj->name() << "(\n";
                    oss << params.str(false, indent + tab);
                    oss << "\n" << indent;
                }
                break;
            }
            default:
                boost::throw_exception(variant_error("Unrecognised variant type"));
        }
        return oss.str();
    }

    size_t variant::hash() const
    {
        size_t seed = 0;

        boost::hash_combine(seed, type());

        switch ( type() )
        {
            case None:
                break;
            case Any:
                boost::hash_combine(seed, m_value.get<Any>().value());
                break;
            case String:
                boost::hash_combine(seed, m_value.get<String>().value());
                break;
            case Int32:
                boost::hash_combine(seed, m_value.get<Int32>());
                break;
            case UInt32:
                boost::hash_combine(seed, m_value.get<UInt32>());
                break;
            case Int64:
                boost::hash_combine(seed, m_value.get<Int64>());
                break;
            case UInt64:
                boost::hash_combine(seed, m_value.get<UInt64>());
                break;
            case Float:
                boost::hash_combine(seed, m_value.get<Float>());
                break;
            case Double:
                boost::hash_combine(seed, m_value.get<Double>());
                break;
            case Boolean:
                boost::hash_combine(seed, m_value.get<Boolean>());
                break;
            case Date:
                boost::hash_combine(seed, m_value.get<Date>());
                break;
            case Time:
                boost::hash_combine(seed, m_value.get<Time>());
                break;
            case DateTime:
                boost::hash_combine(seed, m_value.get<DateTime>());
                break;
            case List:
            case Dictionary:
            case Bag:
            case TimeSeries:
            {
                boost::hash_combine(seed, m_value.get<Collection>().hash());
                break;
            }
            case Buffer:
            {
                boost::hash_combine(seed, m_value.get<Buffer>().hash());
                break;
            }
            case Tuple:
            {
                boost::hash_combine(seed, m_value.get<Tuple>().hash());
                break;
            }
            case Exception:
            {
                boost::hash_combine(seed, m_value.get<Exception>().hash());
                break;
            }
            case Object:
            {
                boost::hash_combine(seed, m_value.get<Object>()->hash());
                break;
            }
            default:
            {
                boost::throw_exception(variant_error("Unrecognised variant type"));
            }
        }
        return seed;
    }

    std::size_t hash_value(const variant& value)
    {
        return value.hash();
    }

    /*static*/ variant variant::make_any(const std::string& value)
    {
        variant result(Any);
        result.m_value.set<Any>(detail::string(value.c_str(), value.size()));
        return result;
    }

} // namespace protean

#include <protean/detail/variant_macros_undef.hpp>
