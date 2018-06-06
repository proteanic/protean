#ifndef PROTEAN_DETAIL_XML_WRITER_IMPL_HPP
#define PROTEAN_DETAIL_XML_WRITER_IMPL_HPP

#include <protean/config.hpp>
#include <protean/xml_common.hpp>
#include <protean/variant.hpp>

#include <stack>

namespace protean { namespace detail {

    class PROTEAN_DECL xml_writer_impl
    {
    public:
        xml_writer_impl(std::ostream& os, int mode);

        void write_document(const variant& document);

    private:
        void write_header();
        void write_empty_element();
        void write_instruction(const variant& instruction);
        void write_element(const variant& element);
        void write_comment(const variant& comment);
        void write_text(const variant& text);
        void write_variant(const variant& value);

        std::string start_tag() const;
        std::string end_tag() const;
        std::string indent() const;

        variant& push(const std::string& element_name="");
        void pop();

    private:

        std::ostream&               m_os;        
        int                         m_mode;

        struct element_info
        {
            element_info(const std::string& name, size_t level) :
                m_name(name),
                m_attributes(variant::Dictionary),
                m_level(level)
            { }

            std::string m_name;
            variant     m_attributes;
            size_t      m_level;
        };

        std::stack<element_info>    m_stack;
    };

}} // namespace protean::detail


#endif // PROTEAN_DETAIL_XML_WRITER_IMPL_HPP
