//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

using System;
using System.Xml;
#if !DISABLE_XMLVALIDATION
using System.Collections.Generic;
using System.Xml.Schema;
#endif

namespace Protean {

    internal abstract class XmlParserBase
    {
        protected XmlParserBase(XmlReaderParameters parameters)
        {
            m_mode = parameters.Mode;
            m_inferTypesFromSchema = parameters.ValidationOptions.HasFlag(XmlReaderValidationFlags.InferTypesFromSchema);

            if (parameters.ValidationOptions.HasFlag(XmlReaderValidationFlags.ValidateXsd))
            {
#if DISABLE_XMLVALIDATION
                throw new NotSupportedException("XML validation is not supported on this platform.");
#else
                XmlReaderSettings settings = new XmlReaderSettings();

                if (parameters.XsdStream != null)
                {
                    XmlSchemaSet sc = new XmlSchemaSet();
                    sc.Add("", System.Xml.XmlReader.Create(parameters.XsdStream));
                    settings.Schemas = sc;
                }
                settings.ValidationType = ValidationType.Schema;
                settings.DtdProcessing = DtdProcessing.Ignore;
                settings.ValidationFlags |= XmlSchemaValidationFlags.ProcessInlineSchema;
                settings.ValidationFlags |= XmlSchemaValidationFlags.ProcessSchemaLocation;
                
                if (parameters.ValidationOptions.HasFlag(XmlReaderValidationFlags.ReportValidationWarnings))
                {
                    settings.ValidationFlags |= XmlSchemaValidationFlags.ReportValidationWarnings;
                }
                
                settings.ValidationEventHandler += (sender, e) =>
                                                   {
                                                       throw new VariantException($"Validation {e.Severity}: {e.Message}");
                                                   };
                m_reader = System.Xml.XmlReader.Create(parameters.XmlStream, settings, parameters.BaseUri);
#endif
            }
            else
            {
                m_reader = System.Xml.XmlReader.Create(parameters.XmlStream);
            }
        }

#if !DISABLE_XMLVALIDATION
        // Useful visual plot of how datatypes are derived in XML Schema:
        // http://www.w3.org/TR/xmlschema-2/#built-in-datatypes
        static Dictionary<XmlTypeCode, VariantBase.EnumType> XmlType2VariantType =
            new Dictionary<XmlTypeCode, VariantBase.EnumType>()
                {
                    { XmlTypeCode.Duration, VariantBase.EnumType.Time },
                    { XmlTypeCode.Date, VariantBase.EnumType.DateTime },
                    { XmlTypeCode.DateTime, VariantBase.EnumType.DateTime },
                    { XmlTypeCode.Time, VariantBase.EnumType.Time },
                    { XmlTypeCode.Boolean, VariantBase.EnumType.Boolean },
                    { XmlTypeCode.Base64Binary, VariantBase.EnumType.Buffer },
                    { XmlTypeCode.String, VariantBase.EnumType.String },
                    { XmlTypeCode.Float, VariantBase.EnumType.Double },
                    { XmlTypeCode.Double, VariantBase.EnumType.Double },
                    { XmlTypeCode.Decimal, VariantBase.EnumType.Double },
                    { XmlTypeCode.Integer, VariantBase.EnumType.Int64 },
                    { XmlTypeCode.NonPositiveInteger, VariantBase.EnumType.Int64 },
                    { XmlTypeCode.PositiveInteger, VariantBase.EnumType.UInt64 },
                    { XmlTypeCode.NonNegativeInteger, VariantBase.EnumType.UInt64 },
                    { XmlTypeCode.Long, VariantBase.EnumType.Int64 },
                    { XmlTypeCode.UnsignedLong, VariantBase.EnumType.UInt64 },
                    { XmlTypeCode.Int, VariantBase.EnumType.Int32 },
                    { XmlTypeCode.UnsignedInt, VariantBase.EnumType.UInt32 }
                };

#endif
        public VariantBase.EnumType GetVariantTypeFromSchema()
        {
#if !DISABLE_XMLVALIDATION
            if (!m_inferTypesFromSchema || m_reader.SchemaInfo == null)
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
#else
            return VariantBase.EnumType.Any;
#endif
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

                    VariantBase.EnumType elementType = GetVariantTypeFromSchema();
                    Variant attributes = new Variant(VariantBase.EnumType.Bag);
                    for (int i = 0; i<m_reader.AttributeCount; ++i)
                    {
                        m_reader.MoveToAttribute(i);
                        VariantBase.EnumType attrType = GetVariantTypeFromSchema();
                        if (attrType != VariantBase.EnumType.Buffer)
                        {
                            attributes.Add(XmlConvert.DecodeName(m_reader.Name), new Variant(attrType, m_reader.Value));
                        }
                        else
                        {
                            attributes.Add(XmlConvert.DecodeName(m_reader.Name), new Variant(Convert.FromBase64String(m_reader.Value)));
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

        public abstract void StartElement(string name, Variant attributes, VariantBase.EnumType variantType);
        public abstract void EndElement();
        public abstract void Characters(string text);
        public abstract void Comment(string comment);
        public abstract void ProcessingInstruction(string target, string data);

        public XmlParserBase WithObjectFactory(IVariantObjectFactory factory)
        {
            m_factory = factory;
            return this;
        }

        private readonly bool m_inferTypesFromSchema;
        private readonly System.Xml.XmlReader m_reader;
        protected XmlMode m_mode;
        protected IVariantObjectFactory m_factory;
    }

} // Protean
