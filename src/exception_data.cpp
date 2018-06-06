#include <protean/exception_data.hpp>
#include <protean/detail/hash.hpp>

#include <typeinfo>

namespace protean {

    exception_data::exception_data(const std::string& type, const std::string& message) :
        m_type(type),
        m_message(message)
    {}

    exception_data::exception_data(const std::string& type, const std::string& message, const std::string& source) :
        m_type(type),
        m_message(message),
        m_source(source)
    {}

    exception_data::exception_data(const std::string& type, const std::string& message, const std::string& source, const std::string& stack) :
        m_type(type),
        m_message(message),
        m_source(source),
        m_stack(stack)
    {}

    exception_data::exception_data(const std::exception& e) :
        m_type(format(typeid(e).name())),
        m_message(e.what())
    {}

    const std::string& exception_data::type() const
    {
        return m_type;
    }
    const std::string& exception_data::message() const
    {
        return m_message;
    }

    const std::string& exception_data::source() const
    {
        return m_source;
    }

    const std::string& exception_data::stack() const
    {
        return m_stack;
    }

    int exception_data::compare(const exception_data& rhs) const
    {
        int typeCmp(type().compare(rhs.type()));
        if (typeCmp!=0)
        {
            return typeCmp;
        }
        int messageCmp(message().compare(rhs.message()));
        if (messageCmp!=0)
        {
            return typeCmp;
        }
        int sourceCmp(source().compare(rhs.source()));
        if (sourceCmp!=0)
        {
            return sourceCmp;
        }

        return stack().compare(rhs.stack());
    }

    boost::uint64_t exception_data::hash(boost::uint64_t seed) const
    {
        seed = detail::hash_value(m_type, seed);
        seed = detail::hash_value(m_message, seed);
        seed = detail::hash_value(m_source, seed);
        return detail::hash_value(m_stack, seed);
    }
    /*static*/ std::string exception_data::format(const char* class_name)
    {
        const std::string tmp(class_name);
        return tmp.substr(6);
    }


} // namespace protean
