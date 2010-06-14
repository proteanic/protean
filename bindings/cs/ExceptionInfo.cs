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
        public ExceptionInfo(String type, String message)
        {
            Class = type;
            Message = message;
            Source = "";
            Stack = "";
        }

        public ExceptionInfo(String type, String message, String source, String stack)
        {
            Class = type;
            Message = message;
            Source = source;
            Stack = stack;
        }

        public ExceptionInfo(ExceptionInfo rhs)
        {
            Class = rhs.Class;
            Message = rhs.Message;
            Source = rhs.Source;
            Stack = rhs.Stack;
        }

        public ExceptionInfo(Exception e)
        {
            Class = e.GetType().ToString();
            Message = e.Message;
            Source = e.Source;
            Stack = e.StackTrace;
        }

        public VariantBase.EnumType Type
        {
            get { return VariantBase.EnumType.Exception; }
        }

        String Class { get; set; }
        String Message { get; set; }
        String Source { get; set; }
        String Stack { get; set; }
    }

} // protean
