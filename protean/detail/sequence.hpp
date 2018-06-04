#ifndef PROTEAN_DETAIL_SEQUENCE_HPP
#define PROTEAN_DETAIL_SEQUENCE_HPP

#include <protean/config.hpp>

#include <protean/detail/collection.hpp>


namespace protean {

    class variant;

    namespace detail {

        class PROTEAN_DECL sequence : public collection
        {
        public:
            virtual const variant& at(size_t index) const = 0;
            virtual variant& at(size_t index) = 0;
        };

    } // namespace protean::detail

} // namespace protean

#endif // PROTEAN_DETAIL_SEQUENCE_HPP
