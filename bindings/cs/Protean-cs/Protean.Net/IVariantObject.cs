namespace Protean {

    public interface IVariantObject
    {
        string Class { get; }

        int Version { get; }

        Variant Deflate();

        void Inflate(Variant param, int version);
    };

} // Protean
