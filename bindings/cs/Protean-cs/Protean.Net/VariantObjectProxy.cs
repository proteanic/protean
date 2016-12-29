//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

using System;

namespace Protean {

    public class VariantObjectProxy : IVariantObject
    {
        public VariantObjectProxy(string classname)
        {
            m_classname = classname;
            m_params = new Variant(Variant.EnumType.None);
            m_version = 0;
        }

        public VariantObjectProxy() :
            this("VariantObjectProxy")
        { }

        public string Class
        {
            get { return m_classname; }
        }

        public int Version
        {
            get { return m_version; }
        }

        public Variant Deflate()
        {
            return m_params;
        }

        public void Inflate(Variant param, int version)
        {
            m_params = param;
            m_version = version;
        }

        public override String ToString()
        {
            return m_classname; 
        }

        int m_version;
        string m_classname;
        Variant m_params;
    }

} // Protean
