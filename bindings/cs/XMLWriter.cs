//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

using System;
using System.Collections.Generic;
using System.Text;
using System.Xml;

namespace protean {

    public class XMLWriter
    {
        public XMLWriter(System.IO.TextWriter stream, XMLMode mode)
        {
            m_mode = mode;
            m_writer = new XmlTextWriter(stream);

            m_writer.Indentation = 2;
            m_writer.Formatting = (mode & XMLMode.Indent) != 0 ? Formatting.Indented : Formatting.None;

            m_stack = new Stack<ElementInfo>();
        }

        public void Write(Variant value)
        {
            WriteDocument(value);
        }

        public static string ToString(Variant value, XMLMode mode)
        {
            using (System.IO.TextWriter ms = new System.IO.StringWriter())
            {
                XMLWriter writer = new XMLWriter(ms, mode);
                writer.WriteDocument(value);
                return ms.ToString();
            }
        }

        public static string ToString(Variant value)
        {
            return ToString(value, XMLMode.Default);
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
            if ((m_mode & XMLMode.NoHeader) == 0)
            {
                WriteHeader();
            }
            if ((m_mode & XMLMode.Preserve)!=0)
            {
                string rootName = "";
                if (document.Is(Variant.EnumType.Mapping))
                {
                    foreach (VariantItem item in document)
                    {
                        if (item.Key == XMLConst.Text)
                        {
                            throw new VariantException("Encountered text in document node");
                        }
                        else if (item.Key == XMLConst.Attributes)
                        {
                            throw new VariantException("Encountered attributes in document node");
                        }
                        else if (item.Key == XMLConst.Instruction)
                        {
                            WriteInstruction(item.Value);
                        }
                        else if (item.Key == XMLConst.Comment)
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
            if (instruction.Is(Variant.EnumType.Mapping) && instruction.ContainsKey(XMLConst.Target) && instruction.ContainsKey(XMLConst.Data))
            {
                m_writer.WriteProcessingInstruction(instruction[XMLConst.Target].As<string>(), instruction[XMLConst.Data].As<string>());
            }
            else
            {
                throw new VariantException(string.Format("Expecting dictionary containing '{0}' and '{1}' for processing instruction", XMLConst.Target, XMLConst.Data));
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
                m_writer.WriteStartElement(context.m_name.Length==0 ? XMLConst.Default : context.m_name);
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
            if ((m_mode & XMLMode.Preserve)==0)
            {
                m_stack.Peek().m_attributes.Add("variant", new Variant(value.Type.ToString()));
            }

            WriteElement(value);
        }

        void WriteElement(Variant element)
        {
            WriteStartTag(m_stack.Peek().m_name);
            WriteAttributes(m_stack.Peek().m_attributes);

            switch(element.Type)
            {
            case Variant.EnumType.None:
            {
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
                WriteText(element);
                break;
            }
            case Variant.EnumType.Dictionary:
            case Variant.EnumType.Bag:
            {
                if ((m_mode & XMLMode.Preserve)!=0)
                {
                    if (element.ContainsKey(XMLConst.Attributes))
                    {
                        WriteAttributes(element[XMLConst.Attributes]);
                    }

                    foreach(VariantItem item in element)
                    {
                        if (item.Key==XMLConst.Attributes)
                        {
                             continue;
                        }
                        else if (item.Key==XMLConst.Text)
                        {
                            WriteText(item.Value);
                        }
                        else if (item.Key==XMLConst.Instruction)
                        {
                            WriteInstruction(item.Value);
                        }
                        else if (item.Key==XMLConst.Comment)
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
                WriteText(new Variant(System.Convert.ToBase64String(element.AsBuffer())));
                break;
            }
            case Variant.EnumType.Object:
            {
                VariantObjectBase obj = element.AsObject();

                // write class name
                m_stack.Peek().m_attributes.Add("class", new Variant(obj.Class));
                m_stack.Peek().m_attributes.Add("version", new Variant(obj.Version));

                // write parameter dictionary
                Push("params");
                WriteVariant(obj.Deflate());
                Pop();

                break;
            }
            case Variant.EnumType.Exception:
            {
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
            default:
                throw new VariantException("Case exhaustion: " + element.Type); 
            }

            WriteEndTag();
        }

        private XmlTextWriter m_writer;
        private XMLMode m_mode;

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

} // protean
