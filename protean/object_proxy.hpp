#ifndef PROTEAN_OBJECT_PROXY_HPP
#define PROTEAN_OBJECT_PROXY_HPP

#include <protean/config.hpp>

#include <protean/object.hpp>
#include <protean/handle.hpp>

#if defined(_MSC_VER)
#    pragma warning(push)
#    pragma warning(disable:4251)
#endif

namespace protean {

    class variant;

    class PROTEAN_DECL object_proxy : public object
    {
    public:
        object_proxy(const std::string& class_name);

        std::string name() const;
        int version() const;
        void deflate(variant& params) const;
        void inflate(const variant& params, int version);
        handle<object> clone() const;

    private:
        friend class variant;
        object_proxy();

        std::string    m_class_name;
        int            m_version;
        variant        m_params;
    };

} // namespace protean

#if defined(_MSC_VER)
#    pragma warning(pop)
#endif

#endif // PROTEAN_OBJECT_PROXY_HPP
