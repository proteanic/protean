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
