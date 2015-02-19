using System;

namespace Protean
{
    [Flags]
    public enum XmlReaderValidationFlags
    {
        None = 0,
        ValidateXsd = 1,
        ReportValidationWarnings = 2,
        InferTypesFromSchema = 4
    }
}