//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef PROTEAN_CLR_VARIANT_EXCEPTION_HPP
#define PROTEAN_CLR_VARIANT_EXCEPTION_HPP

#include <protean/exception_data.hpp>

namespace protean { namespace clr {

    public ref class VariantException : public System::ApplicationException
    {
    public:
        VariantException(System::String^ message);
    };

}} // protean::clr

#ifndef BEGIN_TRANSLATE_ERROR
#define BEGIN_TRANSLATE_ERROR                                                                           \
        try                                                                                             \
        {                                                                                               \
          void
#endif // BEGIN_TRANSLATE_ERROR

#ifndef END_TRANSLATE_ERROR

#define END_TRANSLATE_ERROR                                                                             \
        }                                                                                               \
        catch(std::exception& e)                                                                        \
        {                                                                                               \
            throw gcnew VariantException(gcnew System::String(e.what()));                               \
        }                                                                                               \
		catch(System::Exception^)																		\
        {                                                                                               \
            throw;																						\
        }                                                                                               \
        catch(...)                                                                                      \
        {                                                                                               \
            throw gcnew VariantException(gcnew System::String("Unhandled Exception."));                 \
        }                                                                                               \
        void
#endif // END_TRANSLATE_ERROR

#endif // PROTEAN_CLR_VARIANT_EXCEPTION_HPP
