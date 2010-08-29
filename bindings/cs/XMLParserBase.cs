//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

using System;
using System.Collections.Generic;
using System.Text;
using System.Xml;

namespace protean {

    internal abstract class XMLParserBase
    {
        public XMLParserBase(System.IO.TextReader stream, XMLMode mode, IVariantObjectFactory factory)
        {
            m_mode = mode;
            m_factory = factory;
            m_reader = new XmlTextReader(stream);
        }

        public void Parse()
        {
            while (m_reader.Read())
            {
                XmlNodeType nType = m_reader.NodeType;

                if (nType == XmlNodeType.Element)
                {
                    string name = m_reader.Name;
                    bool isEmptyElement = m_reader.IsEmptyElement;

                    Variant attributes = new Variant(Variant.EnumType.Bag);
                    for (int i = 0; i<m_reader.AttributeCount; ++i)
                    {
                        m_reader.MoveToAttribute(i);
                        attributes.Add(m_reader.Name, new Variant(Variant.EnumType.Any, m_reader.Value));
                    }
                    StartElement(name, attributes);

                    if (isEmptyElement)
                    {
                        EndElement();
                    }
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
        }

        public abstract Variant Read();

        public abstract void StartElement(string name, Variant attributes);
        public abstract void EndElement();
        public abstract void Characters(string text);
        public abstract void Comment(string comment);
        public abstract void ProcessingInstruction(string target, string data);

        private XmlTextReader m_reader;
        protected XMLMode m_mode;
        protected IVariantObjectFactory m_factory;
    }

} // protean
