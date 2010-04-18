#include "VariantException.hpp"

namespace protean { namespace clr {

    VariantException::VariantException(System::String^ message)
        : System::ApplicationException(message)
    {
    }

}} // protean::clr
