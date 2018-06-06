#ifndef PROTEAN_VARIANT_ERROR_HPP
#define PROTEAN_VARIANT_ERROR_HPP

#include <protean/config.hpp>

#include <boost/throw_exception.hpp>
#include <boost/format.hpp>
#include <boost/utility.hpp>
#include <stdexcept>
#include <sstream>

namespace protean {

    class variant_error : public std::runtime_error
    {
    public:
        variant_error(const std::string& what_arg) :
            std::runtime_error(what_arg.c_str())
        {}
    };

} // namespace protean

#endif // PROTEAN_VARIANT_ERROR_HPP
