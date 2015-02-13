//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

namespace Protean {

    public class XmlReader
    {
        public XmlReader(System.IO.TextReader stream, XmlMode mode, bool validateXsd, System.IO.TextReader xsdStream, string baseUri, bool reportValidationWarnings)
        {
            if ((mode & XmlMode.Preserve) != 0)
            {
                throw new VariantException("Preserve parser has not been implemented yet");
            }

            m_parser = new XmlDefaultParser(stream, mode, xsdStream, validateXsd, baseUri, reportValidationWarnings);
        }

        public Variant Read()
        {
            m_parser.Parse();
            return m_parser.Read();
        }

        public static XmlReader Create(
            System.IO.TextReader stream)
        {
            return new XmlReader(stream, XmlMode.Default, false, null, null, false);
        }

        public static XmlReader Create(
            System.IO.TextReader stream, XmlMode mode)
        {
            return new XmlReader(stream, mode, false, null, null, false);
        }

        public static XmlReader Create(
            System.IO.TextReader stream, XmlMode mode, bool validateXsd)
        {
            return new XmlReader(stream, mode, validateXsd, null, null, false);
        }

        public static XmlReader Create(
            System.IO.TextReader stream, XmlMode mode, bool validateXsd, string baseUri, bool reportValidationWarnings)
        {
            return new XmlReader(stream, mode, validateXsd, null, baseUri, reportValidationWarnings);
        }

        public static XmlReader Create(
            System.IO.TextReader stream, XmlMode mode, bool validateXsd, System.IO.TextReader xsdStream)
        {
            return new XmlReader(stream, mode, validateXsd, xsdStream, null, false);
        }

        public static XmlReader Create(
            System.IO.TextReader stream, XmlMode mode, bool validateXsd, System.IO.TextReader xsdStream,
            bool reportValidationWarnings)
        {
            return new XmlReader(stream, mode, validateXsd, xsdStream, null, reportValidationWarnings);
        }

        public XmlReader WithObjectFactory(IVariantObjectFactory factory)
        {
            m_parser.WithObjectFactory(factory);
            return this;
        }

        public static Variant FromString(string xml, XmlMode mode, IVariantObjectFactory factory, bool validateXsd = false)
        {
            using (var ms = new System.IO.StringReader(xml))
            {
                return Create(ms, mode, validateXsd).WithObjectFactory(factory).Read();
            }
        }

        public static Variant FromString(string xml, bool validateXsd = false)
        {
            using (var ms = new System.IO.StringReader(xml))
            {
                return Create(ms, XmlMode.Default, validateXsd).Read();
            }
        }

        private readonly XmlParserBase m_parser;
    }

} // Protean
