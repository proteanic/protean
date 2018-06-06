#ifndef PROTEAN_ARRAY_ITERATOR_HPP
#define PROTEAN_ARRAY_ITERATOR_HPP

#include <protean/config.hpp>
#include <protean/variant_base.hpp>
#include <protean/variant_ref.hpp>

#include <boost/iterator/iterator_facade.hpp>

namespace protean {

    struct array_const_iterator_traits
    {
        typedef const variant_cref  value_type;
        typedef const variant_base* iterator_type;
        typedef std::ptrdiff_t difference_type;
    };

    struct array_iterator_traits
    {
        typedef const variant_ref value_type;
        typedef variant_base*     iterator_type;
        typedef std::ptrdiff_t difference_type;
    };

    template <typename ITERATOR_TRAITS>
    class PROTEAN_DECL array_iterator :
        public boost::iterator_facade<
                array_iterator<ITERATOR_TRAITS>
              , typename ITERATOR_TRAITS::value_type
              , boost::random_access_traversal_tag
              , typename ITERATOR_TRAITS::value_type&
              , typename ITERATOR_TRAITS::difference_type
            >
    {
        typedef typename ITERATOR_TRAITS::iterator_type iterator_type;

    public:
        array_iterator();
        array_iterator(iterator_type iterator, variant_base::enum_type_t type);

        template <typename OTHER_TRAITS>
        array_iterator(const array_iterator<OTHER_TRAITS>& rhs);

        array_iterator(const array_iterator& rhs);

        template <typename OTHER_TRAITS>
        const array_iterator& operator=(const array_iterator<OTHER_TRAITS>& rhs);

        const array_iterator& operator=(const array_iterator& rhs);

     private:
        friend class boost::iterator_core_access;
        friend class variant;

        template <class> friend class array_iterator;

        void increment();

        void decrement();

        bool equal(const array_iterator<array_const_iterator_traits>& other) const;

        typename ITERATOR_TRAITS::value_type & dereference() const;

        void advance( typename array_iterator<ITERATOR_TRAITS>::difference_type n );

    private:
        typename boost::remove_const<typename ITERATOR_TRAITS::value_type>::type m_ref;
    };




    template <typename Value>
    class range_array_iterator
      : public boost::iterator_facade<
            range_array_iterator<Value>
          , Value
          , boost::random_access_traversal_tag
        >
    {
        typedef typename boost::remove_cv<Value>::type no_cv_Value_t;
    public:
        typedef typename boost::iterator_facade<
            range_array_iterator<Value>,
            Value,
            boost::random_access_traversal_tag
        >::difference_type difference_type;
    private:
        struct enabler {};
    public:
        range_array_iterator();

        explicit range_array_iterator(const typed_array &t);
        explicit range_array_iterator(const typed_array &t, size_t offset);

        template <typename OtherValue>
        range_array_iterator(range_array_iterator<OtherValue> const& other, 
            typename boost::enable_if<
                boost::is_convertible<OtherValue*,Value*>, 
                enabler
            >::type = enabler()
        );

    private:
        friend class boost::iterator_core_access;
        template <typename> friend class range_array_iterator;

        template <typename OtherValue>
        bool equal(range_array_iterator<OtherValue> const& other) const;

        void increment();

        void decrement();

        Value &dereference() const;

        void advance( typename range_array_iterator<Value>::difference_type n );

    private:
        variant_base* m_data;
    };

} // protean

#include <protean/array_iterator.ipp>

#endif // PROTEAN_ARRAY_ITERATOR_HPP
