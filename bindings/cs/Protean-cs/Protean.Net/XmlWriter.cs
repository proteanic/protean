//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

using System;
using System.Collections.Generic;
#if !DISABLE_DATATABLE
using System.Data;
#endif
using System.Xml;

namespace Protean {

    public class XmlWriter : IDisposable
    {
        public XmlWriter(System.IO.TextWriter stream, XmlMode mode)
        {
            m_mode = mode;
            var xmlWriterSettings = new XmlWriterSettings
                                    {
                                        Indent = mode.HasFlag(XmlMode.Indent),
                                        OmitXmlDeclaration = mode.HasFlag(XmlMode.NoHeader),
                                        IndentChars = "  "
                                    };
            m_writer = System.Xml.XmlWriter.Create(stream, xmlWriterSettings);
            m_stack = new Stack<ElementInfo>();
        }

        public void Write(Variant value)
        {
            WriteDocument(value);
        }

        public static string ToString(Variant value, XmlMode mode)
        {
            using (System.IO.TextWriter ms = new System.IO.StringWriter())
            {
                using (XmlWriter writer = new XmlWriter(ms, mode))
                {
                    writer.WriteDocument(value);
                }

                ms.Flush();

                return ms.ToString();
            }
        }

        public static string ToString(Variant value)
        {
            return ToString(value, XmlMode.Default);
        }

        public void Dispose()
        {
            Dispose(true);
        }

        private void Dispose(bool disposing)
        {
            if (!m_disposed)
            {
                m_disposed = true;

                if (disposing)
                {
                    m_writer.Dispose();
                }
            }
        }

        private Variant Push(string elementName)
        {
            m_stack.Push(new ElementInfo(elementName));
            return m_stack.Peek().m_attributes;
        }

        private Variant Push()
        {
            return Push("");
        }

        private void Pop()
        {
            m_stack.Pop();
        }

        private void WriteHeader()
        {
            m_writer.WriteProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
        }

        private void WriteDocument(Variant document)
        {
            if ((m_mode & XmlMode.NoHeader) == 0)
            {
                WriteHeader();
            }
            if ((m_mode & XmlMode.Preserve)!=0)
            {
                string rootName = "";
                if (document.Is(VariantBase.EnumType.Mapping))
                {
                    foreach (VariantItem item in document)
                    {
                        if (item.Key == XmlConst.Text)
                        {
                            throw new VariantException("Encountered text in document node");
                        }
                        else if (item.Key == XmlConst.Attributes)
                        {
                            throw new VariantException("Encountered attributes in document node");
                        }
                        else if (item.Key == XmlConst.Instruction)
                        {
                            WriteInstruction(item.Value);
                        }
                        else if (item.Key == XmlConst.Comment)
                        {
                            WriteComment(item.Value);
                        }
                        else
                        {
                            if (rootName.Length==0)
                            {
                                rootName = item.Key;
                            }
                            else
                            {
                                throw new VariantException(string.Format("Illegal element {0} encountered in document, expecting single element {1} at root", item.Key, rootName));
                            }

                            Push(rootName);
                            WriteElement(item.Value);
                            Pop();
                        }
                    }
                }
                else
                {
                    throw new VariantException("Invalid document structure, root node must be a Dictionary or Bag");
                }
            }
            else
            {
                Push();
                WriteVariant(document);
                Pop();
            }
        }

        private void WriteInstruction(Variant instruction)
        {
            if (instruction.Is(VariantBase.EnumType.Mapping) && instruction.ContainsKey(XmlConst.Target) && instruction.ContainsKey(XmlConst.Data))
            {
                m_writer.WriteProcessingInstruction(instruction[XmlConst.Target].As<string>(), instruction[XmlConst.Data].As<string>());
            }
            else
            {
                throw new VariantException(string.Format("Expecting dictionary containing '{0}' and '{1}' for processing instruction", XmlConst.Target, XmlConst.Data));
            }
        }

        private void WriteAttributes(Variant attribute)
        {
            foreach (VariantItem item in attribute)
            {
                m_writer.WriteAttributeString(item.Key, item.Value.AnyCast().As<string>());
            }
        }

        private void WriteStartTag(string name)
        {
            if (m_stack.Count!=0)
            {
                ElementInfo context = m_stack.Peek();
                m_writer.WriteStartElement(XmlConvert.EncodeLocalName(context.m_name.Length == 0 ? XmlConst.Default : context.m_name));
             }
        }

        private void WriteEndTag()
        {
            m_writer.WriteEndElement();
        }

        private void WriteComment(Variant comment)
        {
            m_writer.WriteComment(comment.As<string>());
        }

        private void WriteText(Variant text)
        {
            switch(text.Type)
            {
            case VariantBase.EnumType.Any:
            case VariantBase.EnumType.String:
            {
                m_writer.WriteString(text.As<string>());
                break;
            }
            case VariantBase.EnumType.Int32:
            case VariantBase.EnumType.UInt32:
            case VariantBase.EnumType.Int64:
            case VariantBase.EnumType.UInt64:
            case VariantBase.EnumType.Float:
            case VariantBase.EnumType.Double:
            case VariantBase.EnumType.Boolean:
            case VariantBase.EnumType.Date:
            case VariantBase.EnumType.Time:
            case VariantBase.EnumType.DateTime:
            {
                m_writer.WriteString(text.AnyCast().As<string>());
                break;
            }
            default:
                throw new VariantException("Attempt to output non-primitive variant as text: " + text.Type); 
            }
        }

        private void WriteVariant(Variant value)
        {
            if ((m_mode & XmlMode.Preserve)==0)
            {
                m_stack.Peek().m_attributes.Add("variant", new Variant(value.Type.ToString()));
            }

            WriteElement(value);
        }

        private void WriteElement(Variant element)
        {
            WriteStartTag(m_stack.Peek().m_name);
            
            switch(element.Type)
            {
            case VariantBase.EnumType.None:
            {
                WriteAttributes(m_stack.Peek().m_attributes);
                break;
            }
            case VariantBase.EnumType.Any:
            case VariantBase.EnumType.String:
            case VariantBase.EnumType.Float:
            case VariantBase.EnumType.Double:
            case VariantBase.EnumType.Int32:
            case VariantBase.EnumType.UInt32:
            case VariantBase.EnumType.Int64:
            case VariantBase.EnumType.UInt64:
            case VariantBase.EnumType.Boolean:
            case VariantBase.EnumType.Date:
            case VariantBase.EnumType.Time:
            case VariantBase.EnumType.DateTime:
            {
                WriteAttributes(m_stack.Peek().m_attributes);
                WriteText(element);
                break;
            }
            case VariantBase.EnumType.Dictionary:
            case VariantBase.EnumType.Bag:
            {
                WriteAttributes(m_stack.Peek().m_attributes);
                if ((m_mode & XmlMode.Preserve)!=0)
                {
                    if (element.ContainsKey(XmlConst.Attributes))
                    {
                        WriteAttributes(element[XmlConst.Attributes]);
                    }

                    foreach(VariantItem item in element)
                    {
                        if (item.Key==XmlConst.Attributes)
                        {
                             continue;
                        }
                        else if (item.Key==XmlConst.Text)
                        {
                            WriteText(item.Value);
                        }
                        else if (item.Key==XmlConst.Instruction)
                        {
                            WriteInstruction(item.Value);
                        }
                        else if (item.Key==XmlConst.Comment)
                        {
                            WriteComment(item.Value);
                        }
                        else
                        {
                            Push(item.Key);
                            WriteElement(item.Value);
                            Pop();
                        }
                    }
                }
                else
                {
                    foreach (VariantItem item in element)
                    {
                        Push(item.Key);
                        WriteVariant(item.Value);
                        Pop();
                    }
                }

                break;
            }
            case VariantBase.EnumType.List:
            {
                WriteAttributes(m_stack.Peek().m_attributes);
                foreach(VariantItem item in element)
                {
                    Push();
                    WriteVariant(item.Value);
                    Pop();
                }

                break;
            }
            case VariantBase.EnumType.Tuple:
            {
                m_stack.Peek().m_attributes.Add("size", new Variant(element.Count));

                WriteAttributes(m_stack.Peek().m_attributes);

                foreach(VariantItem item in element)
                {
                    Push();
                    WriteVariant(item.Value);
                    Pop();
                }

                break;
            }
            case VariantBase.EnumType.TimeSeries:
            {
                WriteAttributes(m_stack.Peek().m_attributes);

                foreach(VariantItem item in element)
                {
                    Push().Add("time", new Variant(item.Time));
                    WriteVariant(item.Value);
                    Pop();
                }

                break;
            }
            case VariantBase.EnumType.Buffer:
            {
                WriteAttributes(m_stack.Peek().m_attributes);

                m_writer.WriteBase64(element.AsBuffer(), 0, element.AsBuffer().Length);
                break;
            }
            case VariantBase.EnumType.Object:
            {
                IVariantObject obj = element.AsObject();

                // write class name
                m_stack.Peek().m_attributes.Add("class", new Variant(obj.Class));
                m_stack.Peek().m_attributes.Add("version", new Variant(obj.Version));

                WriteAttributes(m_stack.Peek().m_attributes);

                // write parameter dictionary
                Push("params");
                WriteVariant(obj.Deflate());
                Pop();

                break;
            }
            case VariantBase.EnumType.Exception:
            {
                WriteAttributes(m_stack.Peek().m_attributes);

                VariantExceptionInfo e = element.AsException();

                Push("type");
                WriteElement(new Variant(e.Class));
                Pop();
                
                Push("message");
                WriteElement(new Variant(e.Message));
                Pop();

                if (e.Source.Length!=0)
                {
                    Push("source");
                    WriteElement(new Variant(e.Source));
                    Pop();
                }

                if (e.Stack.Length!=0)
                {
                    Push("stack");
                    WriteElement(new Variant(e.Stack));
                    Pop();
                }
                break;
            }
            case VariantBase.EnumType.Array:
            {
                TypedArray array = element.AsArray();

                m_stack.Peek().m_attributes.Add("size", new Variant(array.Count));
                m_stack.Peek().m_attributes.Add("elementType", new Variant(array.ElementType.ToString()));

                WriteAttributes(m_stack.Peek().m_attributes);

                WriteDelegate writer = null;
                switch (array.ElementType)
                {
                    case VariantBase.EnumType.Float:
                        writer = o => m_writer.WriteString(VariantBase.ToString((float) o));
                        break;
                    case VariantBase.EnumType.Double:
                        writer = o => m_writer.WriteString(VariantBase.ToString((double) o));
                        break;
                    case VariantBase.EnumType.String:
                        writer = o => m_writer.WriteString((string) o);
                        break;
                    case VariantBase.EnumType.Boolean:
                        writer = o => m_writer.WriteString(VariantBase.ToString((bool) o));
                        break;
                    case VariantBase.EnumType.Int32:
                        writer = o => m_writer.WriteString(VariantBase.ToString((int) o));
                        break;
                    case VariantBase.EnumType.UInt32:
                        writer = o => m_writer.WriteString(VariantBase.ToString((uint) o));
                        break;
                    case VariantBase.EnumType.Int64:
                        writer = o => m_writer.WriteString(VariantBase.ToString((long) o));
                        break;
                    case VariantBase.EnumType.UInt64:
                        writer = o => m_writer.WriteString(VariantBase.ToString((ulong) o));
                        break;
                    case VariantBase.EnumType.Time:
                        writer = o => m_writer.WriteString(VariantBase.ToString((TimeSpan) o));
                        break;
                    case VariantBase.EnumType.DateTime:
                        writer = o => m_writer.WriteString(VariantBase.ToString((DateTime) o));
                        break;
                    default:
                        throw new VariantException("Case exhaustion: " + array.ElementType); 
                }

                for (int i=0; i<array.Count; ++i)
                {
                    m_writer.WriteStartElement(XmlConst.Default);
                    writer(array[i]);
                    m_writer.WriteEndElement();
                }
                break;
            }
            case VariantBase.EnumType.DataTable:
            {
#if !DISABLE_DATATABLE
                DataTable dataTable = element.AsDataTable();

                m_stack.Peek().m_attributes.Add("rows", new Variant(dataTable.Rows.Count));
                m_stack.Peek().m_attributes.Add("columns", new Variant(dataTable.Columns.Count));

                WriteAttributes(m_stack.Peek().m_attributes);

                foreach (DataColumn col in dataTable.Columns)
                {
                    Push("Column");
                    m_stack.Peek().m_attributes.Add("name", new Variant(col.ColumnName));
                    m_stack.Peek().m_attributes.Add("type", new Variant(VariantPrimitiveBase.TypeToEnum(col.DataType).ToString()));

                    WriteElement(new Variant());
                    Pop();
                }

                foreach (DataColumn col in dataTable.Columns)
                {
                    Action<object> writer;
                    switch (VariantPrimitiveBase.TypeToEnum(col.DataType))
                    {
                        case VariantBase.EnumType.Float:
                            writer = o => m_writer.WriteString(VariantBase.ToString((float)o));
                            break;
                        case VariantBase.EnumType.Double:
                            writer = o => m_writer.WriteString(VariantBase.ToString((double)o));
                            break;
                        case VariantBase.EnumType.String:
                            writer = o => m_writer.WriteString((string)o);
                            break;
                        case VariantBase.EnumType.Boolean:
                            writer = o => m_writer.WriteString(VariantBase.ToString((bool)o));
                            break;
                        case VariantBase.EnumType.Int32:
                            writer = o => m_writer.WriteString(VariantBase.ToString((int)o));
                            break;
                        case VariantBase.EnumType.UInt32:
                            writer = o => m_writer.WriteString(VariantBase.ToString((uint)o));
                            break;
                        case VariantBase.EnumType.Int64:
                            writer = o => m_writer.WriteString(VariantBase.ToString((long)o));
                            break;
                        case VariantBase.EnumType.UInt64:
                            writer = o => m_writer.WriteString(VariantBase.ToString((ulong)o));
                            break;
                        case VariantBase.EnumType.Time:
                            writer = o => m_writer.WriteString(VariantBase.ToString((TimeSpan)o));
                            break;
                        case VariantBase.EnumType.DateTime:
                            writer = o => m_writer.WriteString(VariantBase.ToString((DateTime)o));
                            break;
                        default:
                            throw new VariantException("Case exhaustion: " + VariantPrimitiveBase.TypeToEnum(col.DataType));
                    }

                    Push("Column").Add("name", new Variant(col.ColumnName));
                    WriteStartTag("s");
                    WriteAttributes(m_stack.Peek().m_attributes);
                    foreach (DataRow row in dataTable.Rows)
                    {
                        m_writer.WriteStartElement(XmlConst.Default);
                        writer(row[col]);
                        m_writer.WriteEndElement();
                    }
                    WriteEndTag();
                    Pop();
                }
                        break;
#else
                throw new NotSupportedException("Datatables are not supported on this platform.");
#endif
            }
            default:
                throw new VariantException("Case exhaustion: " + element.Type); 
            }

            WriteEndTag();
        }

        private delegate void WriteDelegate(object arg);

        private readonly System.Xml.XmlWriter m_writer;
        private readonly XmlMode m_mode;
        private readonly Stack<ElementInfo>    m_stack;

        private bool m_disposed;

        private sealed class ElementInfo
        {
            public ElementInfo(string name)
            {
                m_name = name;
                m_attributes = new Variant(VariantBase.EnumType.Dictionary);
            }

            public string m_name;
            public Variant m_attributes;
        }

    }

} // Protean
