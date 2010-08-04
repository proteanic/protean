//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

using System;
using System.Collections.Generic;
using System.Text;

namespace protean {

    public class ExceptionInfo : IVariantData
    {
        public ExceptionInfo(string type, string message, string source, string stack)
        {
            Class = type;
            Message = message;
            Source = source;
            Stack = stack;
        }

        public ExceptionInfo(string type, string message) :
            this(type, message, "", "")
        { }

        public ExceptionInfo(ExceptionInfo rhs) :
            this(rhs.Class, rhs.Message, rhs.Source, rhs.Stack)
        { }

        public ExceptionInfo(Exception e) :
            this(e.GetType().ToString(), e.Message, e.Source, e.StackTrace)
        { }

        public VariantBase.EnumType Type
        {
            get { return VariantBase.EnumType.Exception; }
        }

        public string Class { get; set; }
        public string Message { get; set; }
        public string Source { get; set; }
        public string Stack { get; set; }
    }

} // protean
