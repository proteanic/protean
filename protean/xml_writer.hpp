//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef PROTEAN_XML_WRITER_HPP
#define PROTEAN_XML_WRITER_HPP

#include <protean/config.hpp>
#include <protean/variant.hpp>

#include <stack>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4512 4251)
#endif

namespace protean {

    static const std::string xml_default_element = "Variant";

    class PROTEAN_DECL xml_writer
    {
    public:
        enum enum_flag_t { None=0, Preserve=1, NoHeader=2 };

    public:
        xml_writer(std::ostream &os, int flags=None);
        void write_document(const variant& document);
        void write_header();
        void write_instructions(const variant& instructions);
        void write_node(const variant& node);
        void write_value(const variant& node);
        void start_tag();
        void end_tag();
        void indent();

        variant& push(const std::string& element_name="");
        void pop();

        void start_content();
        void end_content();

    private:
        static std::string escape_chars(const std::string& arg);
        static void check_invalid_chars(const std::string& arg);

    private:
        size_t            m_indentation;
        std::ostream&    m_os;

        struct element_info
        {
            element_info(const std::string& name) :
                m_name(name),
                m_attributes(variant::Dictionary)
            {
            }
            std::string m_name;
            variant     m_attributes;
        };
        std::stack<element_info> m_stack;
        int m_flags;

        friend PROTEAN_DECL xml_writer& operator<<(xml_writer& writer, const variant& variant);
    };

} // namespace protean

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif // PROTEAN_XML_WRITER_HPP
