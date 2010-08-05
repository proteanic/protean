//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

using System;
using System.Collections.Generic;
using System.Text;
using System.Xml;

namespace protean {

    public class XMLReader
    {
        public XMLReader(System.IO.TextReader stream, XMLMode mode)
        {
            m_mode = mode;
            m_reader = new XmlTextReader(stream);

            m_stack = new Stack<ElementInfo>();
        }

        public Variant Read()
        {
            while (m_reader.Read())
            {
                XmlNodeType nType = m_reader.NodeType;

                if (nType == XmlNodeType.Element)
                {
                    string name = m_reader.Name;
                    Variant attributes = new Variant(Variant.EnumType.Dictionary);
                    for (int i = 0; i<m_reader.AttributeCount; ++i)
                    {
                        m_reader.MoveToAttribute(i);
                        attributes.Add(m_reader.Name, new Variant(m_reader.Value));
                    }
                    StartElement(name, attributes);
                }
                else if (nType == XmlNodeType.Text)
                {
                    Characters(m_reader.Value);
                }
                else if (nType == XmlNodeType.Comment)
                {
                    Comment(m_reader.Value);
                }
                else if (nType == XmlNodeType.ProcessingInstruction)
                {
                    ProcessingInstruction(m_reader.Name, m_reader.Value);
                }
                else if (nType == XmlNodeType.EndElement)
                {
                    EndElement();
                }
            }

            return new Variant();
        }

        private abstract void StartElement(string name, Variant attributes);
        private abstract void EndElement();
        private abstract void Characters(string text);
        private abstract void Comment(string comment);
        private abstract void ProcessingInstruction(string target, string data);

        public static Variant FromString(string value, XMLMode mode)
        {
            using (System.IO.StringReader ms = new System.IO.StringReader(value))
            {
                XMLReader reader = new XMLReader(ms, mode);
                return reader.Read();
            }
        }

        public static Variant FromString(string xml)
        {
            return FromString(xml, XMLMode.Default);
        }

        private XmlTextReader m_reader;
        private XMLMode m_mode;

        class ElementInfo
        {
            public ElementInfo(string name)
            {
                m_name = name;
                m_attributes = new Variant(Variant.EnumType.Dictionary);
            }

            public string m_name;
            public Variant m_attributes;
        }

        Stack<ElementInfo>    m_stack;
    }

} // protean
