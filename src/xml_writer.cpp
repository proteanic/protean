#include <protean/xml_writer.hpp>

#ifndef PROTEAN_DISABLE_XML

#include <protean/detail/xml_writer_impl.hpp>

namespace protean {

    xml_writer::xml_writer(std::ostream &os, int mode) :
        m_os(os),
        m_mode(mode)
    { }

    xml_writer& operator<<(xml_writer& writer, const variant& v)
    {
        detail::xml_writer_impl impl(writer.m_os, writer.m_mode);
        impl.write_document(v);
        return writer;
    }

} // namespace protean

#endif // #ifndef PROTEAN_DISABLE_XML
