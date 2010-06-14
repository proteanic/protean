//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#include <protean/xml_writer.hpp>
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


