//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

using System;
using System.Collections.Generic;
using System.Text;
using System.Xml;

namespace protean {

    internal class XMLDefaultParser : XMLParserBase
    {
        public XMLDefaultParser(System.IO.TextReader stream, XMLMode mode) :
            base(stream, mode)
        {
            m_stack = new Stack<ElementInfo>();
            m_result = new Variant();
        }

        public override void StartElement(string name, Variant attributes)
        {
            ElementInfo context = new ElementInfo(name, attributes);

            if (m_stack.Count!=0)
            {
                ElementInfo parentContext = m_stack.Peek();

                switch(parentContext.m_type)
                {
                case Variant.EnumType.Any:
                    if (!parentContext.m_isTyped)
                    {
                        // Untyped elements with children are converted to Bags
                        parentContext.m_type = Variant.EnumType.Bag;
                    }
                    break;
                case Variant.EnumType.None:
                case Variant.EnumType.String:
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
                case Variant.EnumType.Buffer:
                    throw new VariantException("Unexpected start element");
                case Variant.EnumType.TimeSeries:
                    if (context.m_attributes.ContainsKey("time"))
                    {
                        context.m_time = context.m_attributes["time"].As<DateTime>();
                        context.m_attributes.Remove("time");
                    }
                    else
                    {
                        throw new VariantException("Missing 'time' attribute for TimeSeries element");
                    }
                    break;
                }
            }

            m_stack.Push(context);

            // infer type from 'variant' attribute
            if (context.m_attributes.ContainsKey("variant"))
            {
                context.m_type = Variant.StringToEnum(context.m_attributes["variant"].As<string>());
                context.m_isTyped = true;
                context.m_attributes.Remove("variant");

            }

            switch (context.m_type)
            {
                case Variant.EnumType.Any:
                    if (!context.m_isTyped && !context.m_attributes.Empty)
                    {
                        // untyped elements with attributes are assumed
                        // to be bags
                        context.m_type = Variant.EnumType.Bag;
                    }
                    break;
                case Variant.EnumType.None:
                case Variant.EnumType.String:
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
                case Variant.EnumType.Buffer:
                    break;
                case Variant.EnumType.List:
                case Variant.EnumType.Dictionary:
                case Variant.EnumType.Bag:
                case Variant.EnumType.TimeSeries:
                    context.m_element = new Variant(context.m_type);
                    break;
                case Variant.EnumType.Exception:
                case Variant.EnumType.Object:
                    // Exception and Object are parsed via a dictionary for convenience.
                    context.m_element = new Variant(Variant.EnumType.Dictionary);
                    break;
                case Variant.EnumType.Tuple:
                    {
                        if (context.m_attributes.ContainsKey("size"))
                        {
                            context.m_element = new Variant(Variant.EnumType.Tuple, context.m_attributes["size"].As<int>());
                            context.m_attributes.Remove("size");
                        }
                        else
                        {
                            throw new VariantException("Missing 'size' attribute for Tuple variant");
                        }
                        break;
                    }
                default:
                    throw new VariantException("Case exhaustion: " + context.m_type.ToString());
            }
        }
        public override void EndElement()
        {
            if (m_stack.Count==0)
            {
                throw new VariantException("End tag does not match start tag");
            }

            ElementInfo context = m_stack.Peek();

            if (!context.m_attributes.Empty)
            {
                if (context.m_element.Is(Variant.EnumType.Mapping))
                {
                    foreach(VariantItem item in context.m_attributes)
                    {
                        context.m_element.Add(item.Key, item.Value);
                    }
                }
                else
                {
                    throw new VariantException("No attributes allowed in XML element of "+ context.m_type);
                }
            }

            switch (context.m_type)
            {
                case Variant.EnumType.Any:
                case Variant.EnumType.String:
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
                    context.m_element = new Variant(context.m_type, context.m_data);
                    break;
                case Variant.EnumType.Buffer:
                    context.m_element = new Variant(System.Convert.FromBase64String(context.m_data));
                    break;
                case Variant.EnumType.Exception:
                {
                    string typeStr = "", messageStr = "", sourceStr = "", stackStr = "";

                    if (context.m_element.ContainsKey("type")) {
                            typeStr = context.m_element["type"].As<string>();
                    }
                    else {
                        throw new VariantException("Missing 'type' element in Exception variant"); 
                    }

                    if (context.m_element.ContainsKey("message")) {
                            typeStr = context.m_element["message"].As<string>();
                    }
                    else {
                        throw new VariantException("Missing 'message' element in Exception variant"); 
                    }

                    if (context.m_element.ContainsKey("source")) {
                        sourceStr = context.m_element["source"].As<string>();
                    }

                    if (context.m_element.ContainsKey("stack")) {
                        stackStr = context.m_element["stack"].As<string>();
                    }

                    context.m_element = new Variant(new VariantExceptionInfo(typeStr, messageStr, sourceStr, stackStr));

                    break;
                }
                case Variant.EnumType.Object:
                {
                    string className = "";
                    int version;
                    Variant param;

                    if (context.m_element.ContainsKey("class"))
                    {
                        className = context.m_element["class"].As<string>();
                    }
                    else
                    {
                        throw new VariantException("Missing 'class' element in Object variant");
                    }

                    if (context.m_element.ContainsKey("version"))
                    {
                        version = context.m_element["version"].As<int>();
                    }
                    else
                    {
                        throw new VariantException("Missing 'version' element in Object variant");
                    }

                    if (context.m_element.ContainsKey("params"))
                    {
                        param = context.m_element["params"];
                    }
                    else
                    {
                        throw new VariantException("Missing 'params' element in Object variant");
                    }

                    IVariantObject obj = new VariantObjectProxy(className);
                    obj.Inflate(param, version);

                    context.m_element = new Variant(obj);

                    break;
                }
            }

            if (m_stack.Count > 0)
            {
                m_stack.Pop();
            }

            if (m_stack.Count == 0)
            {
                m_result = context.m_element;
            }
            else
            {
                ElementInfo parentContext = m_stack.Peek();

                switch (parentContext.m_type)
                {
                    case Variant.EnumType.Exception:
                    case Variant.EnumType.Object:
                    case Variant.EnumType.Dictionary:
                    case Variant.EnumType.Bag:
                    {
                        parentContext.m_element.Add(context.m_name, context.m_element);
                        break;
                    }
                    case Variant.EnumType.List:
                    {
                        parentContext.m_element.Add(context.m_element);
                        break;
                    }
                    case Variant.EnumType.Tuple:
                    {
                        parentContext.m_element[parentContext.m_rowIndex++] = context.m_element;
                        break;
                    }
                    case Variant.EnumType.TimeSeries:
                    {
                        parentContext.m_element.Add(context.m_time, context.m_element);
                        break;
                    }
                }
            }
        }
        public override void Characters(string chars)
        {
            if (m_stack.Count==0)
            {
               return;
            }

            ElementInfo context = m_stack.Peek();
            switch (context.m_type)
            {
                case Variant.EnumType.Any:
                case Variant.EnumType.String:
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
                case Variant.EnumType.Buffer:
                    context.m_data += chars;
                    break;
                case Variant.EnumType.None:
                case Variant.EnumType.List:
                case Variant.EnumType.Tuple:
                case Variant.EnumType.Dictionary:
                case Variant.EnumType.Bag:
                case Variant.EnumType.TimeSeries:
                case Variant.EnumType.Exception:
                case Variant.EnumType.Object:
                    if (!Char.IsWhiteSpace(chars, 0))
                    {
                        throw new VariantException("Unexpected characters encountered");
                    };
                    break;
                default:
                    throw new VariantException("Case exhaustion: " + context.m_type);
            }
        }
        public override void Comment(string comment)
        {
            // ignore comments
        }
        public override void ProcessingInstruction(string target, string data)
        {
            // ignore processing instructions
        }
        public override Variant Read()
        {
            return m_result;
        }

        class ElementInfo
        {
            public ElementInfo(string name, Variant attributes)
            {
                m_name = name;
                m_attributes = attributes;
                m_element = new Variant();
                m_isTyped = false;
                m_type = Variant.EnumType.Any;
                m_data = "";
                m_rowIndex = 0;
            }

            public string m_name;
            public DateTime m_time;
            public string m_data;
            public Variant m_attributes;
            public Variant m_element;
            public bool m_isTyped;
            public Variant.EnumType m_type;
            public int m_rowIndex;
        }

        Stack<ElementInfo>    m_stack;
        Variant m_result;
    }

} // protean
