//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#include <boost/foreach.hpp>
#include <boost/regex.hpp>
#include <boost/format.hpp>
#include <boost/math/special_functions/fpclassify.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include <protean/variant.hpp>

#include <protean/detail/dummy_iterator.hpp>
#include <protean/detail/hash.hpp>
#include <protean/variant_ref.hpp>

#include <protean/detail/variant_macros_define.hpp>

namespace protean {

    const variant::date_t& variant::min_date()
    {
        static variant::date_t min_date(boost::gregorian::min_date_time);
        return min_date;
    }

    const variant::date_t& variant::max_date()
    {
        static variant::date_t max_date(boost::gregorian::max_date_time);
        return max_date;
    }

    const variant::date_time_t& variant::min_date_time()
    {
        static variant::date_time_t min_date_time(boost::posix_time::min_date_time);
        return min_date_time;
    }

    const variant::date_time_t& variant::max_date_time()
    {
        static variant::date_time_t max_date_time(variant::max_date(), variant::time_t(23, 59, 59) + boost::posix_time::millisec(999));
        return max_date_time;
    }

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

    variant::variant(const void* data, size_t size) :
        m_type(Buffer)
    {
        m_value.set<Buffer>(new detail::buffer(data, size));
    }

    variant::variant(const exception_data& arg) :
        m_type(Exception)
    {
        m_value.set<Exception>(arg);
    }

    variant::variant(const std::exception& arg) :
        m_type(Exception)
    {
        m_value.set<Exception>(exception_data(arg));
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

    variant::variant(const typed_array& arg) :
        m_type(Array)
    {
        m_value.set<Array>(new typed_array(arg));
    }

    variant::variant(const handle<typed_array>& arg) :
        m_type(Array)
    {
        m_value.set<Array>(arg);
    }

    variant::variant(const variant_ref& arg) :
        variant_base(arg.m_type, *arg.m_value),
        m_type(arg.m_type)
    {
    }

    variant::variant(const variant_cref& arg) :
        variant_base(arg.m_type, *arg.m_value),
        m_type(arg.m_type)
    {
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

    variant::variant(enum_type_t type, size_t size)
    {
		initialise(type, size);
    }

 	void variant::initialise(enum_type_t type, size_t size)
    {
		variant_base::initialise(type, size);
		m_type = type;
    }

	variant::variant(enum_type_t type, const std::string& arg) :
        m_type(type)
    {
        BEGIN_TRANSLATE_ERROR();

        if ((type & Primitive)==0)
        {
            boost::throw_exception(
                variant_error("Attempt to construct non-primitive type (" + enum_to_string(type) + ") from string")
            );
        }

        switch (type)
        {
        case Any:
        case String:
            m_value.set<String>(detail::string(arg.c_str(), arg.size()));
            break;
        case Int32:
            m_value.set<Int32>(lexical_cast<boost::int32_t>(arg));
            break;
        case UInt32:
            m_value.set<Int32>(lexical_cast<boost::uint32_t>(arg));
            break;
        case Int64:
            m_value.set<Int64>(lexical_cast<boost::int64_t>(arg));
            break;
        case UInt64:
            m_value.set<UInt64>(lexical_cast<boost::uint64_t>(arg));
            break;
        case Float:
            m_value.set<Float>(lexical_cast<float>(arg));
            break;
        case Double:
            m_value.set<Double>(lexical_cast<double>(arg));
            break;
        case Boolean:
            m_value.set<Boolean>(lexical_cast<bool>(arg));
            break;
        case Date:
            m_value.set<Date>(lexical_cast<date_t>(arg));
            break;
        case Time:
            m_value.set<Time>(lexical_cast<time_t>(arg));
            break;
        case DateTime:
            m_value.set<DateTime>(lexical_cast<date_time_t>(arg));
            break;
         default:
             boost::throw_exception(variant_error("Case exhaustion: " + enum_to_string(type)));
        }

        END_TRANSLATE_ERROR();
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

    int variant::compare(const variant& rhs) const
    {
        if (m_type == rhs.type())
        {
            return variant_base::compare(m_type, rhs);
        }
        else
        {
            return m_type<rhs.type() ? -1 : 1;
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
        BEGIN_TRANSLATE_ERROR();

        CHECK_VARIANT_FUNCTION(Collection | Any, "empty()");

        if (is<Any>())
        {
            return m_value.get<Any>().empty();
        }
        else
        {
            return m_value.get<Collection>().empty();
        }

        END_TRANSLATE_ERROR();
    }

    size_t variant::size() const
    {
        BEGIN_TRANSLATE_ERROR();

        CHECK_VARIANT_FUNCTION(Collection | Buffer, "size()");

        if (is<Collection>())
        {
            return m_value.get<Collection>().size();
        }
        else
        {
            return m_value.get<Buffer>()->size();
        }

        END_TRANSLATE_ERROR();
    }

    void variant::clear()
    {
        BEGIN_TRANSLATE_ERROR();

        CHECK_VARIANT_FUNCTION(Collection, "clear()");

        m_value.get<Collection>().clear();

        END_TRANSLATE_ERROR();
    }

    variant& variant::push_back(const variant& value, enum_return_trait_t ret)
    {
        BEGIN_TRANSLATE_ERROR();

        CHECK_VARIANT_FUNCTION(List, "push_back()");

        variant& result = m_value.get<List>().push_back(value);

        return (ret==ReturnSelf ? *this : result);

        END_TRANSLATE_ERROR();
    }

    variant& variant::pop_back()
    {
        BEGIN_TRANSLATE_ERROR();

        CHECK_VARIANT_FUNCTION(List, "pop_back()");

        m_value.get<List>().pop_back();
        return *this;

        END_TRANSLATE_ERROR();
    }

    variant& variant::push_back(const date_time_t& time, const variant &value, enum_return_trait_t ret)
    {
        BEGIN_TRANSLATE_ERROR();

        CHECK_VARIANT_FUNCTION(TimeSeries, "pop_back()");

        variant& result = m_value.get<TimeSeries>().push_back(time, value);

        return (ret==ReturnSelf ? *this : result);

        END_TRANSLATE_ERROR();
    }

    variant& variant::insert(const std::string& key, const variant& value, enum_return_trait_t ret)
    {
        BEGIN_TRANSLATE_ERROR();

        CHECK_VARIANT_FUNCTION(Mapping, "insert(\"" + key + "\")")

        variant& result = m_value.get<Mapping>().insert(key, value);

        return (ret==ReturnSelf ? *this : result);

        END_TRANSLATE_ERROR();
    }
    variant& variant::remove(const std::string& key)
    {
        BEGIN_TRANSLATE_ERROR();

        CHECK_VARIANT_FUNCTION(Mapping, "remove(\"" + key + "\")")

        m_value.get<Mapping>().remove(key);
        return *this;

        END_TRANSLATE_ERROR();
    }
    bool variant::has_key(const std::string& key) const
    {
        BEGIN_TRANSLATE_ERROR();

        CHECK_VARIANT_FUNCTION(Mapping, "has_key(\"" + key + "\")")

        return m_value.get<Mapping>().has_key(key);

        END_TRANSLATE_ERROR();
    }
    const variant& variant::at(size_t index) const
    {
        BEGIN_TRANSLATE_ERROR();

        CHECK_VARIANT_FUNCTION(Sequence, "at()")

        return m_value.get<Sequence>().at(index);

        END_TRANSLATE_ERROR();
    }
    variant& variant::at(size_t index)
    {
        BEGIN_TRANSLATE_ERROR();

        CHECK_VARIANT_FUNCTION(Sequence, "at()")

        return m_value.get<Sequence>().at(index);

        END_TRANSLATE_ERROR();
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
        BEGIN_TRANSLATE_ERROR();

        CHECK_VARIANT_FUNCTION(Mapping, "at(\"" + key + "\")");

        return m_value.get<Mapping>().at(key);

        END_TRANSLATE_ERROR();
    }

    variant& variant::at(const std::string& key)
    {
        BEGIN_TRANSLATE_ERROR();

        CHECK_VARIANT_FUNCTION(Mapping, "at(\"" + key + "\")");

        return m_value.get<Mapping>().at(key);

        END_TRANSLATE_ERROR();
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
        BEGIN_TRANSLATE_ERROR();

        CHECK_VARIANT_FUNCTION(Mapping, "range(\"" + key + "\")");

        return m_value.get<Mapping>().range(key);
        
        END_TRANSLATE_ERROR();
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
            if (pred_key.empty() || (i.is<Mapping>() && i.has_key(pred_key) && i.at(pred_key).any_cast().as<std::string>()==pred_val))
            {
                select_impl(i, tail, result);
            }
        }
    }

    variant::const_iterator variant::begin() const
    {
        BEGIN_TRANSLATE_ERROR();

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

        END_TRANSLATE_ERROR();
    }
    variant::const_iterator variant::end() const
    {
        BEGIN_TRANSLATE_ERROR();

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

        END_TRANSLATE_ERROR();
    }
    variant::iterator variant::begin()
    {
        BEGIN_TRANSLATE_ERROR();

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

        END_TRANSLATE_ERROR();
    }
    variant::iterator variant::end()
    {
        BEGIN_TRANSLATE_ERROR();

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

        END_TRANSLATE_ERROR();
    }

    /*
     * variant::lexical_cast
     */
    template<> bool variant::lexical_cast<bool>(const std::string& arg)
    {
        if (arg=="true" || arg=="1")
        {
            return true;
        }
        else if (arg=="false" || arg=="0")
        {
            return false;
        }
        else
        {
            boost::throw_exception(variant_error(std::string("Bad lexical cast: failed to interpret ") + arg + " as bool"));
        }
    }
    template<> float variant::lexical_cast<float>(const std::string& arg)
    {
        if (arg=="NaN")
        {
            return std::numeric_limits<float>::quiet_NaN();
        }
        else if (arg=="-INF")
        {
            return -std::numeric_limits<float>::infinity();
        }
        else if (arg=="INF")
        {
            return std::numeric_limits<float>::infinity();
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
    template<> double variant::lexical_cast<double>(const std::string& arg)
    {
        if (arg=="NaN")
        {
            return std::numeric_limits<double>::quiet_NaN();
        }
        else if (arg=="-INF")
        {
            return -std::numeric_limits<double>::infinity();
        }
        else if (arg=="INF")
        {
            return std::numeric_limits<double>::infinity();
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
    template<> variant::date_t variant::lexical_cast<variant::date_t>(const std::string& arg)
    {
        const boost::regex expr( "^\\d{4}-\\d{2}-\\d{2}$" );
        if ( boost::regex_match(arg, expr) )
        {
            variant::date_t value = boost::gregorian::from_string(arg);
            if (!value.is_special()) return value;
        }
        boost::throw_exception(variant_error(std::string("Bad lexical cast: invalid date '") + arg + "' specified, expecting YYYY-MM-DD"));
    }

    template<> variant::time_t variant::lexical_cast<variant::time_t>(const std::string& arg)
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

    template<> variant::date_time_t variant::lexical_cast<variant::date_time_t>(const std::string& arg)
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
        if (boost::math::fpclassify(arg)==FP_NAN)
        {
            return "NaN";
        }
        else if (boost::math::fpclassify(arg)==FP_INFINITE)
        {
			return (arg < .0 ? "-INF" : "INF");
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
        else if (boost::math::fpclassify(arg)==FP_INFINITE)
        {
			return (arg < .0 ? "-INF" : "INF");
        }
        else
        {
            try
            {
                // print extra precision
                std::ostringstream oss;
                oss << std::setprecision(20) << arg;
                return oss.str();
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

    // Primitive -> Any
    variant variant::any_cast() const
    {
        BEGIN_TRANSLATE_ERROR();

        CHECK_VARIANT_FUNCTION(Primitive, "any_cast()");

        std::string value_as_string;

        switch (m_type)
        {
        case Any:
        case String:
            value_as_string = as<std::string>();
            break;
        case Int32:
            value_as_string = lexical_cast<std::string>(m_value.get<Int32>());
            break;
        case UInt32:
            value_as_string = lexical_cast<std::string>(m_value.get<UInt32>());
            break;
        case Int64:
            value_as_string = lexical_cast<std::string>(m_value.get<Int64>());
            break;
        case UInt64:
            value_as_string = lexical_cast<std::string>(m_value.get<UInt64>());
            break;
        case Float:
            value_as_string = lexical_cast<std::string>(m_value.get<Float>());
            break;
        case Double:
            value_as_string = lexical_cast<std::string>(m_value.get<Double>());
            break;
        case Boolean:
            value_as_string = lexical_cast<std::string>(m_value.get<Boolean>());
            break;
        case Date:
            value_as_string = lexical_cast<std::string>(m_value.get<Date>());
            break;
        case Time:
            value_as_string = lexical_cast<std::string>(m_value.get<Time>());
            break;
        case DateTime:
            value_as_string = lexical_cast<std::string>(m_value.get<DateTime>());
            break;
        default:
            boost::throw_exception(variant_error("Case exhaustion: " + enum_to_string(m_type)));
        }

        variant result(value_as_string);
        result.m_type = Any;

        return result;

        END_TRANSLATE_ERROR();
    }

    // Any -> Primitive (DEPRECATED)
    variant variant::change_type(enum_type_t type) const
    {
        BEGIN_TRANSLATE_ERROR();

        CHECK_VARIANT_FUNCTION(Primitive, "change_type()");

        if ((type & Primitive)==0)
        {
            boost::throw_exception(
                variant_error("Attempt to convert primitive variant to non-primitive type (" + enum_to_string(m_type) + " -> " + enum_to_string(type) + ")")
            );
        }

        if (type==m_type)
        {
            return *this;
        }

        std::string value_as_string;

        switch (m_type)
        {
        case Any:
        case String:
            value_as_string = as<std::string>();
            break;
        case Int32:
            value_as_string = lexical_cast<std::string>(m_value.get<Int32>());
            break;
        case UInt32:
            value_as_string = lexical_cast<std::string>(m_value.get<UInt32>());
            break;
        case Int64:
            value_as_string = lexical_cast<std::string>(m_value.get<Int64>());
            break;
        case UInt64:
            value_as_string = lexical_cast<std::string>(m_value.get<UInt64>());
            break;
        case Float:
            value_as_string = lexical_cast<std::string>(m_value.get<Float>());
            break;
        case Double:
            value_as_string = (boost::format("%|.20|") % m_value.get<Double>() ).str();
            break;
        case Boolean:
            value_as_string = lexical_cast<std::string>(m_value.get<Boolean>());
            break;
        case Date:
            value_as_string = lexical_cast<std::string>(m_value.get<Date>());
            break;
        case Time:
            value_as_string = lexical_cast<std::string>(m_value.get<Time>());
            break;
        case DateTime:
            value_as_string = lexical_cast<std::string>(m_value.get<DateTime>());
            break;
        default:
            boost::throw_exception(variant_error("Case exhaustion: " + enum_to_string(m_type)));
        }

        variant result;

        switch (type)
        {
        case Any:
        case String:
            result = value_as_string;
            result.m_type = type;
            break;
        case Int32:
            result = lexical_cast<boost::int32_t>(value_as_string);
            break;
        case UInt32:
            result = lexical_cast<boost::uint32_t>(value_as_string);
            break;
        case Int64:
            result = lexical_cast<boost::int64_t>(value_as_string);
            break;
        case UInt64:
            result = lexical_cast<boost::uint64_t>(value_as_string);
            break;
        case Float:
            result = lexical_cast<float>(value_as_string);
            break;
        case Double:
            result = lexical_cast<double>(value_as_string);
            break;
        case Boolean:
            result = lexical_cast<bool>(value_as_string);
            break;
        case Date:
            result = lexical_cast<date_t>(value_as_string);
            break;
        case Time:
            result = lexical_cast<time_t>(value_as_string);
            break;
        case DateTime:
            result = lexical_cast<date_time_t>(value_as_string);
            break;
         default:
             boost::throw_exception(variant_error("Case exhaustion: " + enum_to_string(type)));
        }

        return result;

        END_TRANSLATE_ERROR();
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
    template<> bool variant::is<exception_data>()           const { return m_type==Exception; }
    template<> bool variant::is<typed_array>()              const { return m_type==Array; }

    /*
     * variant::as
     */
    template<> std::string variant::as<std::string>() const
    {
        BEGIN_TRANSLATE_ERROR();

        CHECK_VARIANT_FUNCTION(Any | String, "as<std::string>()");

        return m_value.get<String>().value();

        END_TRANSLATE_ERROR();
    }
    template<> bool variant::as<bool>() const
    {
        BEGIN_TRANSLATE_ERROR();

        CHECK_VARIANT_FUNCTION(Any | Boolean, "as<bool>()");

        if (is<Any>())
        {
            return lexical_cast<bool>(m_value.get<Any>().value());
        }
        else
        {
            return m_value.get<Boolean>();
        }

        END_TRANSLATE_ERROR();
    }
    template<> variant::date_t variant::as<variant::date_t>() const
    {
        BEGIN_TRANSLATE_ERROR();

        CHECK_VARIANT_FUNCTION(Any | Date, "as<date_t>()");

        if (is<Any>())
        {
            return lexical_cast<date_t>(m_value.get<Any>().value());
        }
        else
        {
            return m_value.get<Date>();
        }

        END_TRANSLATE_ERROR();
    }
    template<> variant::time_t variant::as<variant::time_t>() const
    {
        BEGIN_TRANSLATE_ERROR();

        CHECK_VARIANT_FUNCTION(Any | Time, "as<time_t>()");

        if (is<Any>())
        {
            return lexical_cast<time_t>(m_value.get<Any>().value());
        }
        else
        {
            return m_value.get<Time>();
        }

        END_TRANSLATE_ERROR();
    }
    template<> variant::date_time_t variant::as<variant::date_time_t>() const
    {
        BEGIN_TRANSLATE_ERROR();

        CHECK_VARIANT_FUNCTION(Any | DateTime, "as<date_time_t>()");

        if (is<Any>())
        {
            return lexical_cast<date_time_t>(m_value.get<Any>().value());
        }
        else
        {
            return m_value.get<DateTime>();
        }

        END_TRANSLATE_ERROR();
    }

    template<> const exception_data& variant::as<exception_data>() const
    {
        BEGIN_TRANSLATE_ERROR();

        CHECK_VARIANT_FUNCTION(Exception, "as<variant::exception_data>()");

        return m_value.get<Exception>();

        END_TRANSLATE_ERROR();
    }

    template<> const typed_array& variant::as<typed_array>() const
    {
        BEGIN_TRANSLATE_ERROR();

        CHECK_VARIANT_FUNCTION(Array, "as<variant::typed_array>()");

        return *m_value.get<Array>();

        END_TRANSLATE_ERROR();
    }

    template<> const object& variant::as<object>() const
    {
        BEGIN_TRANSLATE_ERROR();

        CHECK_VARIANT_FUNCTION(Object, "as<object>()");

        return *m_value.get<Object>();

        END_TRANSLATE_ERROR();
    }

    std::string variant::str(bool summarise, const std::string& indent) const
    {
        static const std::string tab = "   ";
        static const std::string no_indent = "";
        
        std::ostringstream oss;
        oss << std::setprecision(std::numeric_limits<double>::digits10);

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
                oss << any_cast().as<std::string>();
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

                    const unsigned char* byteArray(as<unsigned char*>());
                    if (byteArray!=NULL)
                    {
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
                const exception_data& x(as<exception_data>());
                oss << x.type() << "('" << x.message() << "')";

                if (!x.source().empty())
                {
                    oss << " in: " << x.source();
                }

                if (!x.stack().empty())
                {
                    oss << "\n" << x.stack();
                }

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
            case Array:
            {
                const typed_array& a(as<typed_array>());

                if (summarise)
                {
                    oss << "Array(size=" << a.size() << ", type=" << enum_to_string(a.type()) << ")";
                }
                else
                {
                    oss << "Array(\n";
                    for (size_t i=0; i<a.size();)
                    {
                        oss << variant(a[i]).str(false, indent + tab);
                        if (++i!=a.size())
                        {
                            oss << ",";
                        }
                        oss << "\n";
                    }
                    oss << indent << ")";
                }
                break;
            }
            default:
                oss << "UNKNOWN<" << type() << ">";
        }
        return oss.str();
    }

    boost::uint64_t variant::hash(boost::uint64_t seed) const
    {
        seed = detail::hash_value((boost::uint32_t)m_type, seed);
        seed = variant_base::hash(m_type, seed);
        return seed;
    }

} // namespace protean

#include <protean/detail/variant_macros_undef.hpp>
