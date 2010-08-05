//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

using System;
using System.Collections.Generic;
using System.Text;

namespace protean {

    public class XMLWriter
    {
        public XMLWriter(System.IO.TextWriter stream, XMLMode mode)
        {
            m_mode = mode;
            m_stream = stream;

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
            int level = 0;
            if (m_stack.Count != 0)
            {
                level = m_stack.Peek().m_level + 1;
            }
            m_stack.Push(new ElementInfo(elementName, level));
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
            m_stream.Write("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");
        }

        private string Indent()
        {
            if ((m_mode & XMLMode.Indent) != 0)
            {
                string result = "\n";
                if (m_stack.Count != 0)
                {
                    for (int i = 0; i < m_stack.Peek().m_level; ++i)
                    {
                        result += "  ";
                    }
                }
                return result;
            }
            return "";
        }

        void WriteEmptyElement()
        {
            if (m_stack.Count!=0)
            {
                ElementInfo context = m_stack.Peek();

                m_stream.Write('<' + (context.m_name.Length==0 ? XMLConst.Default : context.m_name));
                foreach(VariantItem item in context.m_attributes)
                {
                    m_stream.Write(string.Format(@" {0}=""{1}""", item.Key, item.Value.AnyCast().As<string>()));
                }
                m_stream.Write("/>");
            }
        }

        string StartTag()
        {
            string result = "";

            if (m_stack.Count!=0)
            {
                ElementInfo context = m_stack.Peek();
            
                m_stream.Write('<' + (context.m_name.Length==0 ? XMLConst.Default : context.m_name));
                foreach(VariantItem item in context.m_attributes)
                {
                    m_stream.Write(string.Format(@" {0}=""{1}""", item.Key, item.Value.AnyCast().As<string>()));
                }
                result += '>';
            }

            return result;
        }

        string EndTag()
        {
            string result = "";

            if (m_stack.Count!=0)
            {
                ElementInfo context = m_stack.Peek();

                m_stream.Write(string.Format("</{0}>", context.m_name.Length==0 ? XMLConst.Default : context.m_name));
            }

            return result;
        }

        void WriteDocument(Variant document)
        {
            if ((m_mode & XMLMode.NoHeader)==0)
            {
                // output the XML header
                WriteHeader();
            }

            if ((m_mode & XMLMode.Preserve)!=0)
            {
                bool first = true;
                if (document.Is(Variant.EnumType.Mapping))
                {
                    string elementName = "";
                    foreach(VariantItem item in document)
                    {
                        if (item.Key==XMLConst.Text)
                        {
                            throw new VariantException("Encountered text in document node");
                        }
                        else if (item.Key==XMLConst.Attributes)
                        {
                            throw new VariantException("Encountered attributes in document node");
                        }
                        else if (item.Key==XMLConst.Instruction)
                        {
                            if (!first)
                            {
                                m_stream.Write(Indent());
                            }
                            WriteInstruction(item.Value);
                        }
                        else if (item.Key==XMLConst.Comment)
                        {
                            if (!first)
                            {
                                m_stream.Write(Indent());
                            }
                            WriteComment(item.Value);
                        }
                        else
                        {
                            if (elementName.Length==0)
                            {
                                elementName = item.Key;
                            }
                            else
                            {
                                throw new VariantException(string.Format("Illegal element {0} encountered in document, expecting single element {1} at root", item.Key, elementName));
                            }

                            Push(item.Key);
                            if (!first)
                            {
                                m_stream.Write(Indent());
                            }
                            WriteElement(item.Value);
                            Pop();
                        }
                        first = false;
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
                m_stream.Write(string.Format("<?{0} {1}?>", instruction[XMLConst.Target].As<string>(), instruction[XMLConst.Data].As<string>()));
            }
            else
            {
                throw new VariantException(string.Format("Expecting dictionary containing '{0}' and '{1}' for processing instruction", XMLConst.Target, XMLConst.Data));
            }        
        }

        void WriteComment(Variant comment)
        {
            m_stream.Write("<!--" + comment.As<string>() + "-->");
        }

        void WriteText(Variant text)
        {
            switch(text.Type)
            {
            case Variant.EnumType.Any:
            case Variant.EnumType.String:
            {
                m_stream.Write(text.As<string>());
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
                m_stream.Write(text.AnyCast().As<string>());
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
            if (element.Is(Variant.EnumType.Collection) && element.Empty)
            {
                WriteEmptyElement();
                return;
            }

            switch(element.Type)
            {
            case Variant.EnumType.None:
            {
                WriteEmptyElement();
                break;
            }
            case Variant.EnumType.Any:
            case Variant.EnumType.String:
            {
                if (element.As<string>().Length == 0)
                {
                    WriteEmptyElement();
                }
                else
                {
                    m_stream.Write(StartTag());
                    WriteText(element);
                    m_stream.Write(EndTag());
                }
                break;
            }
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
                m_stream.Write(StartTag());
                WriteText(element);
                m_stream.Write(EndTag());
                break;
            }
            case Variant.EnumType.Dictionary:
            case Variant.EnumType.Bag:
            {
                if ((m_mode & XMLMode.Preserve)!=0)
                {
                    if (element.ContainsKey(XMLConst.Attributes))
                    {
                        m_stack.Peek().m_attributes = element[XMLConst.Attributes];

                        if (element.Count==1)
                        {
                            WriteEmptyElement();
                            break;
                        }
                    }

                    m_stream.Write(StartTag());

                    bool prev_is_text = false;
                    foreach(VariantItem item in element)
                    {
                        if (item.Key==XMLConst.Attributes)
                        {
                            continue;
                        }
                        else if (item.Key==XMLConst.Text)
                        {
                            WriteText(item.Value);
                            prev_is_text = true;
                        }
                        else if (item.Key==XMLConst.Instruction)
                        {
                            Push(item.Key);

                            if (!prev_is_text)
                            {
                                m_stream.Write(Indent());
                            }

                            WriteInstruction(item.Value);

                            prev_is_text = false;

                            Pop();
                        }
                        else if (item.Key==XMLConst.Comment)
                        {
                            Push(item.Key);

                            if (!prev_is_text)
                            {
                                m_stream.Write(Indent());
                            }

                            WriteComment(item.Value);

                            prev_is_text = false;

                            Pop();
                        }
                        else
                        {
                            Push(item.Key);

                            if (!prev_is_text)
                            {
                                m_stream.Write(Indent());
                            }

                            WriteElement(item.Value);

                            prev_is_text = false;

                            Pop();
                        }
                    }

                    if (!prev_is_text)
                    {
                        m_stream.Write(Indent());
                    }

                    m_stream.Write(EndTag());
                }
                else
                {
                    m_stream.Write(StartTag());

                    foreach (VariantItem item in element)
                    {
                        Push(item.Key);

                        m_stream.Write(Indent());
                        WriteVariant(item.Value);

                        Pop();
                    }

                    m_stream.Write(Indent());
                    m_stream.Write(EndTag());
                }

                break;
            }
            case Variant.EnumType.List:
            {
                m_stream.Write(StartTag());

                foreach(VariantItem item in element)
                {
                    Push();
                    m_stream.Write(Indent());
                    WriteVariant(item.Value);
                    Pop();
                }

                m_stream.Write(Indent());
                m_stream.Write(EndTag());

                break;
            }
            case Variant.EnumType.Tuple:
            {
                m_stack.Peek().m_attributes.Add("size", new Variant(element.Count));
                m_stream.Write(StartTag());

                foreach(VariantItem item in element)
                {
                    Push();
                    m_stream.Write(Indent());
                    WriteVariant(item.Value);
                    Pop();
                }

                m_stream.Write(Indent());
                m_stream.Write(EndTag());
                break;
            }
            case Variant.EnumType.TimeSeries:
            {
                m_stream.Write(StartTag());

                foreach(VariantItem item in element)
                {
                    Push().Add("time", new Variant(item.Time));
                    m_stream.Write(Indent());
                    WriteVariant(item.Value);
                    Pop();
                }

                m_stream.Write(Indent());
                m_stream.Write(EndTag());
                break;
            }
            case Variant.EnumType.Buffer:
            {
                m_stream.Write(StartTag());
                m_stream.Write(System.Convert.ToBase64String(element.AsBuffer()));
                m_stream.Write(EndTag());
                break;
            }
            case Variant.EnumType.Object:
            {
                VariantObjectBase obj = element.AsObject();

                // write class name
                m_stack.Peek().m_attributes.Add("class", new Variant(obj.Class));
                m_stack.Peek().m_attributes.Add("version", new Variant(obj.Version));

                m_stream.Write(StartTag());

                // write parameter dictionary
                Push("params");
                m_stream.Write(Indent());
                WriteVariant(obj.Deflate());
                Pop();

                m_stream.Write(Indent());
                m_stream.Write(EndTag());

                break;
            }
            case Variant.EnumType.Exception:
            {
                m_stream.Write(StartTag());

                VariantExceptionInfo e = element.AsException();

                Push("type");
                m_stream.Write(Indent());
                WriteElement(new Variant(e.Class));
                Pop();
                
                Push("message");
                m_stream.Write(Indent());
                WriteElement(new Variant(e.Message));
                Pop();

                if (e.Source.Length!=0)
                {
                    Push("source");
                    m_stream.Write(Indent());
                    WriteElement(new Variant(e.Source));
                    Pop();
                }

                if (e.Stack.Length!=0)
                {
                    Push("stack");
                    m_stream.Write(Indent());
                    WriteElement(new Variant(e.Stack));
                    Pop();
                }

                m_stream.Write(Indent());
                m_stream.Write(EndTag());
                break;
            }
            default:
                throw new VariantException("Case exhaustion: " + element.Type); 
            }

        }

        private System.IO.TextWriter m_stream;
        private XMLMode m_mode;

        class ElementInfo
        {
            public ElementInfo(string name, int level)
            {
                m_name = name;
                m_attributes = new Variant(Variant.EnumType.Dictionary);
                m_level = level;
            }

            public string m_name;
            public Variant m_attributes;
            public int m_level;
        }

        Stack<ElementInfo>    m_stack;
    }

} // protean
