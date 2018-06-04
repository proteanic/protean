namespace Protean
{
    [System.Flags]
    public enum XmlMode
    {
        None                = 0x00000000,
        Preserve            = 0x00000001,
        NoHeader            = 0x00000002,
        Indent              = 0x00000004,
        StripSpace          = 0x00000008,
        CreateProxy         = 0x00000010,
        Default             = None
    }

    public abstract class XmlConst 
    {
        public const string Default = "Variant";
        public const string Text = "__text__";
        public const string Comment = "__comment__";
        public const string Instruction = "__instruction__";
        public const string Attributes = "__attributes__";
        public const string Target = "__target__";
        public const string Data = "__data__";
    };

} // Protean
