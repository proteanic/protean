//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef PROTEAN_REF_COUNTED_HPP
#define PROTEAN_REF_COUNTED_HPP

#include <protean/config.hpp>

#include <string>

namespace protean {
 
    template<typename T> class handle;

    namespace detail {

        class PROTEAN_DLLEXPORT ref_counted
        {
        public:
            ref_counted();
            ref_counted(const ref_counted& rhs);
  
        // workaround for MSVC 2005 (maybe also needed for earlier MSVC versions?)
        #if !defined(_MSC_VER) || (_MSC_VER > 1400)
        private:

            template <typename T> friend class protean::handle;
        #endif
        
            size_t  m_ref_count;
        };

        inline ref_counted::ref_counted() :
            m_ref_count(0)
        {}

        inline ref_counted::ref_counted(const ref_counted& /*rhs*/) :
            m_ref_count(0)
        {}


}} // namespace protean::detail

#endif // PROTEAN_REF_COUNTED_HPP
