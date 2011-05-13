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

        public XMLReader(System.IO.TextReader stream, XMLMode mode, IVariantObjectFactory factory, bool validateXsd, System.IO.TextReader xsdStream)
        {
            if ((mode & XMLMode.Preserve) != 0)
            {
                throw new VariantException("Preserve parser has not been implemented yet");
            }
            else
            {
                m_parser = new XMLDefaultParser(stream, mode, factory, xsdStream, validateXsd);
            }
        }

        public Variant Read()
        {
            m_parser.Parse();
            return m_parser.Read();
        }

        public static Variant Create(
            System.IO.TextReader stream, XMLMode mode = XMLMode.Default, IVariantObjectFactory factory = null,
            bool validateXsd = true, System.IO.TextReader xsdStream = null)
        {
            XMLReader reader = new XMLReader(stream, mode, factory, validateXsd, xsdStream);
            return reader.Read();
        }

        public static Variant FromString(string xml, XMLMode mode, IVariantObjectFactory factory)
        {
            using (System.IO.StringReader ms = new System.IO.StringReader(xml))
            {
                return Create(stream: ms, mode: mode, factory: factory);
            }
        }

        public static Variant FromString(string xml)
        {
            return FromString(xml, XMLMode.Default, null);
        }

        private XMLParserBase m_parser;
    }

} // protean
