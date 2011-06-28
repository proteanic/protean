//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

using System;
using System.Collections.Generic;
using System.Xml;

namespace Protean {

    public class XmlWriter
    {
        public XmlWriter(System.IO.TextWriter stream, XmlMode mode)
        {
            m_mode = mode;
            m_writer = new XmlTextWriter(stream);

            m_writer.Indentation = 2;
            m_writer.Formatting = (mode & XmlMode.Indent) != 0 ? Formatting.Indented : Formatting.None;

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
                XmlWriter writer = new XmlWriter(ms, mode);
                writer.WriteDocument(value);
                return ms.ToString();
            }
        }

        public static string ToString(Variant value)
        {
            return ToString(value, XmlMode.Default);
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

        void WriteHeader()
        {
            m_writer.WriteProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
        }

        void WriteDocument(Variant document)
        {
            if ((m_mode & XmlMode.NoHeader) == 0)
            {
                WriteHeader();
            }
            if ((m_mode & XmlMode.Preserve)!=0)
            {
                string rootName = "";
                if (document.Is(Variant.EnumType.Mapping))
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

        void WriteInstruction(Variant instruction)
        {
            if (instruction.Is(Variant.EnumType.Mapping) && instruction.ContainsKey(XmlConst.Target) && instruction.ContainsKey(XmlConst.Data))
            {
                m_writer.WriteProcessingInstruction(instruction[XmlConst.Target].As<string>(), instruction[XmlConst.Data].As<string>());
            }
            else
            {
                throw new VariantException(string.Format("Expecting dictionary containing '{0}' and '{1}' for processing instruction", XmlConst.Target, XmlConst.Data));
            }
        }

        void WriteAttributes(Variant attribute)
        {
            foreach (VariantItem item in attribute)
            {
                m_writer.WriteAttributeString(item.Key, item.Value.AnyCast().As<string>());
            }
        }

        void WriteStartTag(string name)
        {
            if (m_stack.Count!=0)
            {
                ElementInfo context = m_stack.Peek();
                m_writer.WriteStartElement(context.m_name.Length==0 ? XmlConst.Default : context.m_name);
             }
        }

        void WriteEndTag()
        {
            m_writer.WriteEndElement();
        }

        void WriteComment(Variant comment)
        {
            m_writer.WriteComment(comment.As<string>());
        }

        void WriteText(Variant text)
        {
            switch(text.Type)
            {
            case Variant.EnumType.Any:
            case Variant.EnumType.String:
            {
                m_writer.WriteString(text.As<string>());
                break;
            }
            case Variant.EnumType.Int32:
            case Variant.EnumType.UInt32:
            case Variant.EnumType.Int64:
            case Variant.EnumType.UInt64:
            case Variant.EnumType.Float:
            case Variant.EnumType.Double:
            case Variant.EnumType.Boolean:
            case Variant.EnumType.Date:
            case Variant.EnumType.Time:
            case Variant.EnumType.DateTime:
            {
                m_writer.WriteString(text.AnyCast().As<string>());
                break;
            }
            default:
                throw new VariantException("Attempt to output non-primitive variant as text: " + text.Type); 
            }
        }

        void WriteVariant(Variant value)
        {
            if ((m_mode & XmlMode.Preserve)==0)
            {
                m_stack.Peek().m_attributes.Add("variant", new Variant(value.Type.ToString()));
            }

            WriteElement(value);
        }

        void WriteElement(Variant element)
        {
            WriteStartTag(m_stack.Peek().m_name);
            
            switch(element.Type)
            {
            case Variant.EnumType.None:
            {
                WriteAttributes(m_stack.Peek().m_attributes);
                break;
            }
            case Variant.EnumType.Any:
            case Variant.EnumType.String:
            case Variant.EnumType.Float:
            case Variant.EnumType.Double:
            case Variant.EnumType.Int32:
            case Variant.EnumType.UInt32:
            case Variant.EnumType.Int64:
            case Variant.EnumType.UInt64:
            case Variant.EnumType.Boolean:
            case Variant.EnumType.Date:
            case Variant.EnumType.Time:
            case Variant.EnumType.DateTime:
            {
                WriteAttributes(m_stack.Peek().m_attributes);
                WriteText(element);
                break;
            }
            case Variant.EnumType.Dictionary:
            case Variant.EnumType.Bag:
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
            case Variant.EnumType.List:
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
            case Variant.EnumType.Tuple:
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
            case Variant.EnumType.TimeSeries:
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
            case Variant.EnumType.Buffer:
            {
                WriteAttributes(m_stack.Peek().m_attributes);

                m_writer.WriteBase64(element.AsBuffer(), 0, element.AsBuffer().Length);
                break;
            }
            case Variant.EnumType.Object:
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
            case Variant.EnumType.Exception:
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
            case Variant.EnumType.Array:
            {
                TypedArray array = element.AsArray();

                m_stack.Peek().m_attributes.Add("size", new Variant(array.Count));
                m_stack.Peek().m_attributes.Add("elementType", new Variant(array.ElementType.ToString()));

                WriteAttributes(m_stack.Peek().m_attributes);

                WriteDelegate writer = null;
                switch (array.ElementType)
                {
                    case VariantBase.EnumType.Float:
                        writer = delegate(object o) { m_writer.WriteString(VariantBase.ToString((float)o)); };
                        break;
                    case VariantBase.EnumType.Double:
                        writer = delegate(object o) { m_writer.WriteString(VariantBase.ToString((double)o)); };
                        break;
                    case VariantBase.EnumType.String:
                        writer = delegate(object o) { m_writer.WriteString((string)o); };
                        break;
                    case VariantBase.EnumType.Boolean:
                        writer = delegate(object o) { m_writer.WriteString(VariantBase.ToString((bool)o)); };
                        break;
                    case VariantBase.EnumType.Int32:
                        writer = delegate(object o) { m_writer.WriteString(VariantBase.ToString((int)o)); };
                        break;
                    case VariantBase.EnumType.UInt32:
                        writer = delegate(object o) { m_writer.WriteString(VariantBase.ToString((uint)o)); };
                        break;
                    case VariantBase.EnumType.Int64:
                        writer = delegate(object o) { m_writer.WriteString(VariantBase.ToString((long)o)); };
                        break;
                    case VariantBase.EnumType.UInt64:
                        writer = delegate(object o) { m_writer.WriteString(VariantBase.ToString((ulong)o)); };
                        break;
                    case VariantBase.EnumType.Time:
                        writer = delegate(object o) { m_writer.WriteString(VariantBase.ToString((TimeSpan)o)); };
                        break;
                    case VariantBase.EnumType.DateTime:
                        writer = delegate(object o) { m_writer.WriteString(VariantBase.ToString((DateTime)o)); };
                        break;
                }

                for (int i=0; i<array.Count; ++i)
                {
                    m_writer.WriteStartElement(XmlConst.Default);
                    writer(array[i]);
                    m_writer.WriteEndElement();
                }
                break;
            }
            default:
                throw new VariantException("Case exhaustion: " + element.Type); 
            }

            WriteEndTag();
        }

        delegate void WriteDelegate(object arg);

        private XmlTextWriter m_writer;
        private XmlMode m_mode;

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

} // Protean
