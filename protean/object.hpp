#ifndef PROTEAN_OBJECT_HPP
#define PROTEAN_OBJECT_HPP

#include <protean/config.hpp>

#include <protean/detail/ref_counted.hpp>

#include <string>

#include <boost/cstdint.hpp>

namespace protean {

    class variant;
    template<typename T> class handle;

    class PROTEAN_DECL object : public detail::ref_counted
    {
    public:
        virtual ~object();

    // abstract interface that must be implemented by all variant objects
    public:
        virtual std::string name() const = 0;
        virtual int version() const = 0;
        virtual void deflate(variant& params) const = 0;
        virtual void inflate(const variant& params, int version) = 0;
        virtual handle<object> clone() const = 0;

    public:
        void coerce(const object& rhs);

        int compare(const object& rhs) const;
        boost::uint64_t hash(boost::uint64_t seed) const;
    };

} // namespace protean

#endif // PROTEAN_OBJECT_HPP
