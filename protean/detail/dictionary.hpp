#ifndef PROTEAN_DETAIL_DICTIONARY_HPP
#define PROTEAN_DETAIL_DICTIONARY_HPP

#include <protean/config.hpp>
#include <protean/detail/mapping.hpp>

#if defined(_MSC_VER)
#    pragma warning(push)
#    pragma warning(disable:4251)
#endif

namespace protean { namespace detail {

    class PROTEAN_DECL dictionary : public mapping
    {
        typedef std::map<std::string, variant> container_t;

    public:
        int compare(const collection& rhs) const;
        boost::uint64_t hash(boost::uint64_t seed) const;
        bool empty() const;
        size_t size() const;
        void clear();

        variant& insert(const std::string& key, const variant& value);
        bool has_key(const std::string& key) const;
        const variant& at(const std::string& key) const;
        variant& at(const std::string& key);
        void remove(const std::string& key);
        variant range(const std::string& key) const;

        variant_const_iterator_base* begin() const;
        variant_const_iterator_base* end() const;
        variant_iterator_base* begin();
        variant_iterator_base* end();

    private:
        container_t m_value;
    };

}} // namespace protean::detail

#if defined(_MSC_VER)
#    pragma warning(pop)
#endif

#endif // PROTEAN_DETAIL_DICTIONARY_HPP
