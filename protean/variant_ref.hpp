#ifndef PROTEAN_VARIANT_REF_HPP
#define PROTEAN_VARIANT_REF_HPP

#include <protean/config.hpp>
#include <protean/variant_base.hpp>

namespace protean {

    class variant;
    class variant_cref;
    template<typename> class array_iterator;
    class typed_array;
    
    // mutable reference
    class PROTEAN_DECL variant_ref
    {
    private:
        variant_ref(variant_base* value, variant_base::enum_type_t type);

    public:
        variant_ref();
        explicit variant_ref(variant& arg);
        
        variant_ref(const variant_ref& rhs);
        variant_ref& operator=(const variant_ref& rhs);

        template<typename T>
        const variant_ref& operator=(const T& rhs) const;

        template<typename T>
        T as() const;

        operator bool() const;

    private:
        friend class variant;
        friend class variant_cref;
        template <typename> friend class array_iterator;
        friend class typed_array;

        variant_base*               m_value;
        variant_base::enum_type_t   m_type;
    };

    // const reference
    class PROTEAN_DECL variant_cref
    {
    private:
        variant_cref(const variant_base* value, variant_base::enum_type_t type);

    public:
        variant_cref();
        explicit variant_cref(const variant& arg);
        
        variant_cref(const variant_cref& rhs);
        variant_cref& operator=(const variant_cref& rhs);

        // non-const to const conversion
        variant_cref(const variant_ref& rhs);
        variant_cref& operator=(const variant_ref& rhs);

        template<typename T>
        T as() const;

        operator bool() const;

    private:
        friend class variant;
        template <typename> friend class array_iterator;
        friend class typed_array;

        const variant_base*         m_value;
        variant_base::enum_type_t   m_type;
    };

} // namespace protean

#endif // PROTEAN_VARIANT_REF_HPP
