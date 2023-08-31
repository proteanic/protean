#ifndef PROTEAN_TYPED_ARRAY_HPP
#define PROTEAN_TYPED_ARRAY_HPP

#ifdef PROTEAN_ARRAY_ITERATOR_HPP
#error array_iterator.ipp depends on the definition of class typed_array but \
  was probably included from array_iterator.hpp before that class was defined. \
  MSVC ignores the problem, but g++ does not.
#endif

#include <protean/config.hpp>

#include <protean/detail/ref_counted.hpp>
#include <protean/variant_base.hpp>

namespace protean {

    class variant;
    class variant_ref;
    class variant_cref;

    struct array_iterator_traits;
    struct array_const_iterator_traits;

    template<typename T>
    class PROTEAN_DECL array_iterator;

    class PROTEAN_DECL typed_array : public detail::ref_counted
    {
        template <typename> friend class range_array_iterator;

    public:
        typed_array(size_t size, variant_base::enum_type_t type);
        typed_array(size_t size, const variant& init);

        typed_array(const typed_array& rhs);
        typed_array& operator=(const typed_array& rhs);

        ~typed_array();

        size_t size() const;
        variant_base::enum_type_t type() const;

        variant_ref operator[](size_t n);
        variant_cref operator[](size_t n) const;

        int compare(const typed_array& rhs) const;
        boost::uint64_t hash(boost::uint64_t seed) const;

        void swap(typed_array& rhs);

        typedef array_iterator<array_const_iterator_traits> const_iterator;
        typedef array_iterator<array_iterator_traits> iterator;

        const_iterator begin() const;
        const_iterator end() const;
        iterator begin();
        iterator end();

    private:
        variant_base& at(size_t n);
        const variant_base& at(size_t n) const;

    private:
        friend class detail::xml_default_handler;
        friend class binary_reader;
        friend class binary_writer;

    private:
        size_t						m_size;
        variant_base*               m_data;
        variant_base::enum_type_t   m_type;
    };

} // namespace protean

#endif // PROTEAN_TYPED_ARRAY_HPP
