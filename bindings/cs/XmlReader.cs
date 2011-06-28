//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

namespace Protean {

    public class XmlReader
    {
        public XmlReader(System.IO.TextReader stream, XmlMode mode, IVariantObjectFactory factory, bool validateXsd, System.IO.TextReader xsdStream)
        {
            if ((mode & XmlMode.Preserve) != 0)
            {
                throw new VariantException("Preserve parser has not been implemented yet");
            }
            else
            {
                m_parser = new XmlDefaultParser(stream, mode, factory, xsdStream, validateXsd);
            }
        }

        public Variant Read()
        {
            m_parser.Parse();
            return m_parser.Read();
        }

        public static XmlReader Create(
            System.IO.TextReader stream, XmlMode mode = XmlMode.Default, IVariantObjectFactory factory = null,
            bool validateXsd = true, System.IO.TextReader xsdStream = null)
        {
            return new XmlReader(stream, mode, factory, validateXsd, xsdStream);
        }

        public static Variant FromString(string xml, XmlMode mode, IVariantObjectFactory factory)
        {
            using (System.IO.StringReader ms = new System.IO.StringReader(xml))
            {
                return Create(stream: ms, mode: mode, factory: factory).Read();
            }
        }

        public static Variant FromString(string xml)
        {
            return FromString(xml, XmlMode.Default, null);
        }

        private XmlParserBase m_parser;
    }

} // Protean
