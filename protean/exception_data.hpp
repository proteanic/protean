#ifndef PROTEAN_EXCEPTION_INFO_HPP
#define PROTEAN_EXCEPTION_INFO_HPP

#include <protean/config.hpp>

#include <string>

#include <boost/cstdint.hpp>

#if defined(_MSC_VER)
#    pragma warning(push)
#    pragma warning(disable:4512 4251)
#endif

namespace protean {

    class PROTEAN_DECL exception_data
    {
    public:
        exception_data(const std::string& type, const std::string& message);
        exception_data(const std::string& type, const std::string& message, const std::string& source);
        exception_data(const std::string& type, const std::string& message, const std::string& source, const std::string& stack);
        exception_data(const std::exception& e);

        const std::string& type() const;
        const std::string& message() const;
        const std::string& source() const;
        const std::string& stack() const;

        int compare(const exception_data& rhs) const;
        boost::uint64_t hash(boost::uint64_t seed) const;

        static std::string format(const char* class_name);

    private:
        const std::string m_type;
        const std::string m_message;
        const std::string m_source;
        const std::string m_stack;
    };

} // namespace protean

#if defined(_MSC_VER)
#    pragma warning(pop)
#endif

#endif // PROTEAN_EXCEPTION_INFO_HPP
