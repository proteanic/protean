//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

using System;
using System.Collections.Generic;
using System.Text;

namespace protean {

    class VariantObjectProxy : VariantObjectBase
    {
	    public VariantObjectProxy(String classname)
        {
            m_classname = classname;
        }

        public override String ClassName
        {
            get { return m_classname; }
        }

        public override UInt32 Version
        {
            get { return m_version; }
        }

        public override Variant Deflate()
        {
            return m_params;
        }

        public override void Inflate(Variant param, UInt32 version)
        {
		    m_params = param;
		    m_version = version;
        }

        public override String ToString()
        {
		    return m_classname; 
        }

        UInt32 m_version;
        String m_classname;
        Variant m_params;
    }

} // protean
