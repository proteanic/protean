//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef PROTEAN_DETAIL_SEQUENCE_HPP
#define PROTEAN_DETAIL_SEQUENCE_HPP

#include <protean/config.hpp>

#include <protean/detail/collection.hpp>


namespace protean {

    class variant;

    namespace detail {

        class PROTEAN_DLLEXPORT sequence : public collection
        {
        public:
            virtual const variant& at(size_t index) const = 0;
            virtual variant& at(size_t index) = 0;
        };

    } // namespace protean::detail

} // namespace protean

#endif // PROTEAN_DETAIL_SEQUENCE_HPP
