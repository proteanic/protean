namespace Protean
{
    [System.Flags]
    public enum BinaryMode
    {
        None            = 0x00000000,
        Compress        = 0x00000001,   // BinaryWriter: compress data
        ZlibHeader      = 0x00000002,   // BinaryWriter: output zlib header
        CreateProxy     = 0x00000004,   // binary_reader: create proxy object if class has not been registered in factory
        DateTimeAsTicks = 0x00000008,   // binary_writer: serialise DateTime/Date/Time as milliseconds/days since 1/1/1400
        Default = DateTimeAsTicks
    }
}
