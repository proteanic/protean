//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef PROTEAN_CLR_STRONG_REFERENCE_HPP
#define PROTEAN_CLR_STRONG_REFERENCE_HPP

namespace protean { namespace clr {

    using namespace System::Runtime::InteropServices;

    class StrongReference
    {
    public:
        StrongReference(System::Object^ obj)
        {
            m_handle = GCHandle::Alloc(obj);
        }

        ~StrongReference()
        {
            m_handle.Free();
        }

    private:
        GCHandle m_handle;
    };
}}

#define STRONG_REFERENCE(x) \
    StrongReference context_ ## x ## (x)

#endif //PROTEAN_CLR_STRONG_REFERENCE_HPP
