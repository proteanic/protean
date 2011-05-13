//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

using System;
using System.Collections.Generic;
using System.Text;
using System.Xml;
using System.Xml.Schema;

namespace protean {

    internal abstract class XMLParserBase
    {
        public XMLParserBase(System.IO.TextReader stream, XMLMode mode, IVariantObjectFactory factory, System.IO.TextReader xsdStream, bool validateXsd)
        {
            m_mode = mode;
            m_factory = factory;

            if (validateXsd)
            {
                XmlReaderSettings settings = new XmlReaderSettings();

                if (xsdStream != null)
                {
                    XmlSchemaSet sc = new XmlSchemaSet();
                    sc.Add("", XmlReader.Create(xsdStream));
                    settings.Schemas = sc;
                }
                settings.ValidationType = ValidationType.Schema;
                settings.ValidationFlags |= XmlSchemaValidationFlags.ProcessInlineSchema;
                settings.ValidationFlags |= XmlSchemaValidationFlags.ProcessSchemaLocation;
                settings.ValidationEventHandler += new ValidationEventHandler(ValidationCallBack);
                m_reader = XmlReader.Create(stream, settings);
            }
            else
            {
                m_reader = XmlReader.Create(stream);
            }
        }

        private static void ValidationCallBack(object sender, ValidationEventArgs e)
        {
            Console.WriteLine("Validation Error: {0}", e.Message);
        }

        // Useful visual plot of how datatypes are derived in XML Schema:
        // http://www.w3.org/TR/xmlschema-2/#built-in-datatypes
        static Dictionary<XmlTypeCode, Variant.EnumType> XmlType2VariantType =
            new Dictionary<XmlTypeCode, Variant.EnumType>()
                {
                    { XmlTypeCode.Duration, Variant.EnumType.Time },
                    { XmlTypeCode.Date, Variant.EnumType.DateTime },
                    { XmlTypeCode.DateTime, Variant.EnumType.DateTime },
                    { XmlTypeCode.Time, Variant.EnumType.Time },

                    { XmlTypeCode.Boolean, Variant.EnumType.Boolean },
                    { XmlTypeCode.Base64Binary, Variant.EnumType.Buffer },
                    { XmlTypeCode.String, Variant.EnumType.String },
                    { XmlTypeCode.Float, Variant.EnumType.Double },
                    { XmlTypeCode.Double, Variant.EnumType.Double },
                    
                    { XmlTypeCode.Decimal, Variant.EnumType.Double },
                    { XmlTypeCode.NonNegativeInteger, Variant.EnumType.UInt64 },
                    { XmlTypeCode.Integer, Variant.EnumType.Int64 },
                    { XmlTypeCode.Int, Variant.EnumType.Int32 },
                    { XmlTypeCode.UnsignedInt, Variant.EnumType.UInt32 }
                };

        public Variant.EnumType getVariantTypeFromSchema()
        {
            XmlSchemaType schemaType = m_reader.SchemaInfo.SchemaType;

            //null if validation not used
            if (schemaType == null)
                return VariantBase.EnumType.Any;

            while (true)
            {
                if (XmlType2VariantType.ContainsKey(schemaType.TypeCode))
                    return XmlType2VariantType[schemaType.TypeCode];

                schemaType = schemaType.BaseXmlSchemaType;
                if (schemaType.TypeCode == XmlTypeCode.Item || schemaType.TypeCode == XmlTypeCode.AnyAtomicType)
                    return Variant.EnumType.Any;
            }
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

                    Variant.EnumType elementType = getVariantTypeFromSchema();
                    Variant attributes = new Variant(Variant.EnumType.Bag);
                    for (int i = 0; i<m_reader.AttributeCount; ++i)
                    {
                        m_reader.MoveToAttribute(i);
                        Variant.EnumType attrType = getVariantTypeFromSchema();
                        if (attrType != Variant.EnumType.Buffer)
                        {
                            attributes.Add(m_reader.Name, new Variant(attrType, m_reader.Value));
                        }
                        else
                        {
                            attributes.Add(m_reader.Name, new Variant(System.Convert.FromBase64String(m_reader.Value)));
                        }
                    }

                    StartElement(name, attributes, elementType);

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

        public abstract void StartElement(string name, Variant attributes, Variant.EnumType variantType);
        public abstract void EndElement();
        public abstract void Characters(string text);
        public abstract void Comment(string comment);
        public abstract void ProcessingInstruction(string target, string data);

        private XmlReader m_reader;
        protected XMLMode m_mode;
        protected IVariantObjectFactory m_factory;
    }

} // protean
