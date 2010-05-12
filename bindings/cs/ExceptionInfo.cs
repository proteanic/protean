//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

using System;
using System.Collections.Generic;
using System.Text;

namespace protean {

    public class ExceptionInfo
    {
	    public ExceptionInfo(String type, String message)
        {
            Type = type;
            Message = message;
            Source = "";
            Stack = "";
        }

	    public ExceptionInfo(String type, String message, String source, String stack)
        {
            Type = type;
            Message = message;
            Source = source;
            Stack = stack;
        }

	    public ExceptionInfo(Exception e)
        {
            Type = e.GetType().ToString();
            Message = e.Message;
            Source = e.Source;
            Stack = e.StackTrace;
        }

        String Type { get; set; }
        String Message { get; set; }
        String Source { get; set; }
        String Stack { get; set; }
    }

} // protean
