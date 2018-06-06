namespace Protean {

    internal class VariantObjectData : IVariantData
    {
        public VariantObjectData(IVariantObject obj)
        {
            Value = obj;
        }

        public VariantBase.EnumType _Type
        {
            get { return VariantBase.EnumType.Object; }
        }

        public int CompareTo(IVariantData rhs)
        {
            IVariantObject rhsObj = ((VariantObjectData)rhs).Value;
            if (Value.Class != rhsObj.Class)
            {
                return Value.Class.CompareTo(rhsObj.Class);
            }
            if (Value.Version != rhsObj.Version)
            {
                return Value.Version.CompareTo(rhsObj.Version);
            }
            return Value.Deflate().CompareTo(rhsObj.Deflate());
        }

        public IVariantObject Value { get; private set; }
    };

} // Protean
