//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

using System;
using System.Collections.Generic;
using System.Xml;
using System.Xml.Schema;

namespace Protean {

    internal abstract class XmlParserBase
    {
        protected XmlParserBase(System.IO.TextReader stream, XmlMode mode, System.IO.TextReader xsdStream, bool validateXsd, string baseUri, bool reportValidationWarnings)
        {
            m_mode = mode;

            if (validateXsd)
            {
                XmlReaderSettings settings = new XmlReaderSettings();

                if (xsdStream != null)
                {
                    XmlSchemaSet sc = new XmlSchemaSet();
                    sc.Add("", System.Xml.XmlReader.Create(xsdStream));
                    settings.Schemas = sc;
                }
                settings.ValidationType = ValidationType.Schema;
                settings.DtdProcessing = DtdProcessing.Ignore;
                settings.ValidationFlags |= XmlSchemaValidationFlags.ProcessInlineSchema;
                settings.ValidationFlags |= XmlSchemaValidationFlags.ProcessSchemaLocation;
                
                if (reportValidationWarnings)
                {
                    settings.ValidationFlags |= XmlSchemaValidationFlags.ReportValidationWarnings;
                }
                
                settings.ValidationEventHandler += ValidationCallBack;
                m_reader = System.Xml.XmlReader.Create(stream, settings, baseUri);
            }
            else
            {
                m_reader = System.Xml.XmlReader.Create(stream);
            }
        }

        private static void ValidationCallBack(object sender, ValidationEventArgs e)
        {
		    throw new VariantException(String.Format("Validation {0}: {1}" , e.Severity, e.Message));
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
                    { XmlTypeCode.Integer, Variant.EnumType.Int64 },
                    { XmlTypeCode.NonPositiveInteger, Variant.EnumType.Int64 },
                    { XmlTypeCode.PositiveInteger, Variant.EnumType.UInt64 },
                    { XmlTypeCode.NonNegativeInteger, Variant.EnumType.UInt64 },
                    { XmlTypeCode.Long, Variant.EnumType.Int64 },
                    { XmlTypeCode.UnsignedLong, Variant.EnumType.UInt64 },
                    { XmlTypeCode.Int, Variant.EnumType.Int32 },
                    { XmlTypeCode.UnsignedInt, Variant.EnumType.UInt32 }
                };

        public Variant.EnumType GetVariantTypeFromSchema()
        {
            if (m_reader.SchemaInfo == null)
            {
                return VariantBase.EnumType.Any;
            }

            XmlSchemaType schemaType = m_reader.SchemaInfo.SchemaType;

            if (schemaType == null)
            {
                return VariantBase.EnumType.Any;
            }

            while (true)
            {
                if (XmlType2VariantType.ContainsKey(schemaType.TypeCode))
                {
                    return XmlType2VariantType[schemaType.TypeCode];
                }

                schemaType = schemaType.BaseXmlSchemaType;
                if (schemaType == null || schemaType.TypeCode == XmlTypeCode.Item ||
                    schemaType.TypeCode == XmlTypeCode.AnyAtomicType)
                {
                    return VariantBase.EnumType.Any;
                }
            }
        }

        public void Parse()
        {
            while (m_reader.Read())
            {
                XmlNodeType nType = m_reader.NodeType;

                if (nType == XmlNodeType.Element)
                {
                    string name = XmlConvert.DecodeName(m_reader.Name);
                    bool isEmptyElement = m_reader.IsEmptyElement;

                    Variant.EnumType elementType = GetVariantTypeFromSchema();
                    Variant attributes = new Variant(Variant.EnumType.Bag);
                    for (int i = 0; i<m_reader.AttributeCount; ++i)
                    {
                        m_reader.MoveToAttribute(i);
                        Variant.EnumType attrType = GetVariantTypeFromSchema();
                        if (attrType != Variant.EnumType.Buffer)
                        {
                            attributes.Add(XmlConvert.DecodeName(m_reader.Name), new Variant(attrType, m_reader.Value));
                        }
                        else
                        {
                            attributes.Add(XmlConvert.DecodeName(m_reader.Name), new Variant(System.Convert.FromBase64String(m_reader.Value)));
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
                    ProcessingInstruction(XmlConvert.DecodeName(m_reader.Name), m_reader.Value);
                }
                else if (nType == XmlNodeType.EndElement)
                {
                    EndElement();
                }
				else if (nType == XmlNodeType.CDATA)
				{
					Characters(m_reader.Value);
				}
            }
        }

        public abstract Variant Read();

        public abstract void StartElement(string name, Variant attributes, Variant.EnumType variantType);
        public abstract void EndElement();
        public abstract void Characters(string text);
        public abstract void Comment(string comment);
        public abstract void ProcessingInstruction(string target, string data);

        public XmlParserBase WithObjectFactory(IVariantObjectFactory factory)
        {
            m_factory = factory;
            return this;
        }

        private readonly System.Xml.XmlReader m_reader;
        protected XmlMode m_mode;
        protected IVariantObjectFactory m_factory;
    }

} // Protean
