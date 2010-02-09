//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef PROTEAN_DETAIL_MAPPING_HPP
#define PROTEAN_DETAIL_MAPPING_HPP

#include <protean/config.hpp>

#include <protean/detail/collection.hpp>

#include <string>

namespace protean {

    class variant;

    namespace detail {

        class PROTEAN_DECL mapping : public collection
        {
        public:
            virtual variant& insert(const std::string &key, const variant& value) = 0;
            virtual bool has_key(const std::string& key ) const = 0;
            virtual const variant& at(const std::string& key) const = 0;
            virtual variant& at(const std::string& key) = 0;
            virtual void remove(const std::string& key) = 0;
            virtual variant range(const std::string& key) const = 0;
        };

    } // namespace protean::detail

} // namespace protean

#endif // PROTEAN_DETAIL_MAPPING_HPP
