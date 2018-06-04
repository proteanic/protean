#include <protean/object.hpp>
#include <protean/variant.hpp>

#include <protean/detail/hash.hpp>

namespace protean {

    object::~object()
    {
    }

    void object::coerce(const object& rhs)
    {
        if (name()!=rhs.name())
        {
            boost::throw_exception(variant_error(
                (boost::format("Attempt to coerce object of type %s into %s")
                    % rhs.name()
                    % name()).str()));
        }

        variant params;
        rhs.deflate(params);
        inflate(params, rhs.version());
    }

    int object::compare(const object& rhs) const
    {
        if (name()!=rhs.name())
        {
            return name().compare(rhs.name());
        }
        else if (version()!=rhs.version())
        {
			
            return version()<rhs.version() ? -1 : 1;
        }
        else
        {
            variant lhs_params, rhs_params;
            deflate(lhs_params);
            rhs.deflate(rhs_params);
            return lhs_params.compare(rhs_params);
        }
    }

    boost::uint64_t object::hash(boost::uint64_t seed) const
    {
        seed = detail::hash_value(name(), seed);

        variant params;
        deflate(params);
        seed = detail::hash_value(params, seed);

        return seed;
    }

} // namespace protean
