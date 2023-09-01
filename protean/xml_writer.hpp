#ifndef PROTEAN_XML_WRITER_HPP
#define PROTEAN_XML_WRITER_HPP

#ifndef PROTEAN_DISABLE_XML

#include <protean/config.hpp>
#include <protean/xml_common.hpp>
#include <protean/variant.hpp>

#if defined(_MSC_VER)
#    pragma warning(push)
#    pragma warning(disable:4512 4251)
#endif

namespace protean {

    class PROTEAN_DECL xml_writer
    {
    public:
        xml_writer(std::ostream &os, int mode=xml_mode::None);

    private:
        std::ostream&   m_os;
        int             m_mode;

        friend PROTEAN_DECL xml_writer& operator<<(xml_writer& writer, const variant& variant);
    };

} // namespace protean

#if defined(_MSC_VER)
#    pragma warning(pop)
#endif

#endif // #ifndef PROTEAN_DISABLE_XML

#endif // #ifndef PROTEAN_XML_WRITER_HPP
