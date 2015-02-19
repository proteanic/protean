using System.IO;

namespace Protean
{
    public class XmlReaderParameters
    {
        public XmlReaderParameters(TextReader xmlStream)
        {
            XmlStream = xmlStream;
        }

        public TextReader XmlStream { get; private set; }

        public TextReader XsdStream { get; set; }

        public XmlMode Mode { get; set; }

        public string BaseUri { get; set; }

        public XmlReaderValidationFlags ValidationOptions { get; set; }
    }
}