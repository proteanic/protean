#include <protean/detail/data_table.hpp>
#include <protean/detail/hash.hpp>
#include <protean/detail/data_table_variant_iterator.hpp>

#include <algorithm>
#include <boost/foreach.hpp>
#include <boost/date_time/posix_time/ptime.hpp>

namespace protean { namespace detail {

    data_table::data_table(size_t capacity /* = 0 */)
        : m_capacity(capacity)
    {}

    /* Type-parameterised functor to add columns, used as operation performed */
    /* in enum_runtime_type_map dynamic enum mapping.                         */
    struct data_table::column_adder
    {
        typedef data_table& result_type;

        column_adder(data_table& dt, const std::string& name)
            : m_dt(dt),
              m_name(name)
        {}

        template <typename TypedEnum>
        result_type operator()(TypedEnum* = 0)
        {
            return m_dt.add_column<static_cast<variant_base::enum_type_t>(TypedEnum::value)>(m_name);
        }

    private:
        data_table&        m_dt;
        const std::string& m_name;
    };

    data_table& data_table::add_column(variant_base::enum_type_t type, const std::string& name)
    {
        column_adder ca(*this, name);
        return enum_runtime_map<detail::data_table_column_enums, detail::enum_equality_comparer>(
            type, ca, "Columns of type " + variant_base::enum_to_string(type) + " are not supported"
        );
    }

    data_table& data_table::add_column(variant_base::enum_type_t type)
    {
        return add_column(type, boost::str( boost::format("Column%d") % (m_columns.size() + 1) ));
    }

    data_table_column_base& data_table::get_column(size_t i)
    {
        if (i >= m_columns.size())
            boost::throw_exception(variant_error("Column out of range"));

        return m_columns[i];
    }

    data_table_column_base& data_table::get_column(const std::string& name)
    {
        BOOST_FOREACH(column_container_type::reference column, m_columns)
            if (column.name() == name)
                return column;

        boost::throw_exception(variant_error(boost::str(boost::format("No such column '%s'") % name)));
    }

    const data_table_column_base& data_table::get_column(size_t i) const
    {
        return const_cast<data_table*>(this)->get_column(i);
    }

    const data_table_column_base& data_table::get_column(const std::string& name) const
    {
        return const_cast<data_table*>(this)->get_column(name);
    }

    const data_table::column_container_type& data_table::columns() const
    {
        return m_columns;
    }

    data_table::column_container_type& data_table::columns()
    {
        return m_columns;
    }

    bool data_table::empty() const
    {
        return m_columns.empty() || get_column(0).empty();
    }

    size_t data_table::size() const
    {
        return empty() ? 0 : get_column(0).size();
    }

    void data_table::clear()
    {
        BOOST_FOREACH(column_container_type::reference column, m_columns)
            column.clear();
    }

    int data_table::compare(const collection& rhs) const
    {
        const data_table* cast_rhs = dynamic_cast<const data_table*>(&rhs);
        if (cast_rhs == 0)
            boost::throw_exception(variant_error("Unable to cast collection to data table"));

        // If two data tables have a different number of columns, they cannot be compared
        if (m_columns.size() != cast_rhs->m_columns.size())
            boost::throw_exception(variant_error("Cannot compare two data tables with different column types"));

        // If two data tables have the same number of columns but of differing types, they cannot be compared
        for (column_container_type::const_iterator this_column_iter = m_columns.begin(),
                                                   this_column_end  = m_columns.end(),
                                                   rhs_column_iter  = cast_rhs->m_columns.begin(),
                                                   rhs_column_end   = cast_rhs->m_columns.end()
                ; this_column_iter != this_column_end && rhs_column_iter != rhs_column_end
                ; ++this_column_iter, ++rhs_column_iter)
        {
            if (this_column_iter->type() != rhs_column_iter->type())
                boost::throw_exception(variant_error("Cannot compare two data tables with different column types"));
        }

        // If this data table has less rows than the RHS data table, return -1; if more, return 1
        if (size() != cast_rhs->size())
            return size() < cast_rhs->size() ? -1 : 1;

        // Else the data tables have the same number of rows, so compare each row (tuple)
        variant_iterator<const_iterator_traits> this_iter = begin(),
                                                this_end  = end(),
                                                rhs_iter  = cast_rhs->begin();
        while (this_iter != this_end)
        {
            int comparison = (this_iter++)->compare(*(rhs_iter++));
            if (comparison != 0)
                return comparison;
        }

        // If we made it here, all rows are equal
        return 0;
    }

    boost::uint64_t data_table::hash(boost::uint64_t seed) const
    {
        return hash_range(
            variant_iterator<const_iterator_traits>(begin()),
            variant_iterator<const_iterator_traits>(end()),
            seed
        );
    }

    variant_const_iterator_base* data_table::begin() const
    {
        std::vector<variant_iterator<const_iterator_traits> > column_iterators;
        for (column_container_type::const_iterator iter = m_columns.begin(); iter != m_columns.end(); ++iter)
            column_iterators.push_back(iter->begin());
        
        return new data_table_variant_iterator_interface<const_iterator_traits>(column_iterators);
    }

    variant_const_iterator_base* data_table::end() const
    {
        std::vector<variant_iterator<const_iterator_traits> > column_iterators;
        for (column_container_type::const_iterator iter = m_columns.begin(); iter != m_columns.end(); ++iter)
            column_iterators.push_back(iter->end());
        
        return new data_table_variant_iterator_interface<const_iterator_traits>(column_iterators);
    }

    variant_iterator_base* data_table::begin()
    {
        std::vector<variant_iterator<iterator_traits> > column_iterators;
        for (column_container_type::iterator iter = m_columns.begin(); iter != m_columns.end(); ++iter)
            column_iterators.push_back(iter->begin());

        return new data_table_variant_iterator_interface<iterator_traits>(column_iterators);
    }

    variant_iterator_base* data_table::end()
    {
        std::vector<variant_iterator<iterator_traits> > column_iterators;
        for (column_container_type::iterator iter = m_columns.begin(); iter != m_columns.end(); ++iter)
            column_iterators.push_back(iter->end());

        return new data_table_variant_iterator_interface<iterator_traits>(column_iterators);
    }

}} // namespace protean::detail
