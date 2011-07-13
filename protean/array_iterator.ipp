//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).


namespace protean {

    // Copy constructor definitions
    template<typename ITERATOR_TRAITS>
    template<typename OTHER_TRAITS>
    array_iterator<ITERATOR_TRAITS>::array_iterator(const array_iterator<OTHER_TRAITS>& rhs) :
        m_ref(rhs.m_ref)
    {
    }

    // Assignment operator definitions
    template<typename ITERATOR_TRAITS>
    template<typename OTHER_TRAITS>
    const array_iterator<ITERATOR_TRAITS>& array_iterator<ITERATOR_TRAITS>::operator=(const array_iterator<OTHER_TRAITS>& rhs)
    {
        m_ref = rhs.m_ref;
        return *this;
    }




    template <typename Value>
    range_array_iterator<Value>::range_array_iterator()
        : m_data(0) {}

    template <typename Value>
    range_array_iterator<Value>::range_array_iterator(const typed_array &t)
        : m_data(t.m_data) {}

    template <typename Value>
    range_array_iterator<Value>::range_array_iterator(const typed_array &t, size_t offset)
        : m_data(t.m_data+offset) {}

    template <typename Value>
    template<typename OtherValue>
    range_array_iterator<Value>::range_array_iterator(range_array_iterator<OtherValue> const& other, 
        typename boost::enable_if<
            boost::is_convertible<OtherValue*,Value*>, 
            typename range_array_iterator<Value>::enabler
        >::type
    ) : m_data(other.m_data) {}

    template <typename Value>
    template<typename OtherValue>
    bool range_array_iterator<Value>::equal(range_array_iterator<OtherValue> const& other) const
    {
        return this->m_data == other.m_data;
    }

    template <typename Value>
    void range_array_iterator<Value>::increment()
    {
        ++m_data;
    }

    template <typename Value>
    void range_array_iterator<Value>::decrement()
    {
        --m_data;
    }

    template <typename Value>
    Value &range_array_iterator<Value>::dereference() const
    {
        return m_data->m_value.get<type_to_enum<no_cv_Value_t>::value>();
    }

    template <typename Value>
    void range_array_iterator<Value>::advance(typename super::difference_type n)
    {
        m_data += n;
    }

} // protean
