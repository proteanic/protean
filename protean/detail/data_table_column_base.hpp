//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef PROTEAN_DETAIL_DATA_TABLE_COLUMN_BASE_HPP
#define PROTEAN_DETAIL_DATA_TABLE_COLUMN_BASE_HPP

#if defined(_MSC_VER)
#    pragma warning(push)
#    pragma warning(disable:4251)
#endif

#include <protean/config.hpp>
#include <protean/variant_error.hpp>
#include <protean/variant_iterator.hpp>
#include <protean/variant_base.hpp>
#include <protean/detail/data_table_types.hpp>
#include <protean/detail/data_table_column_base.hpp>

#include <vector>
#include <string>

namespace protean {	namespace detail {

	template <variant_base::enum_type_t E>
	struct column_traits
	{
		typedef boost::mpl::int_<E>                      enum_type;
		typedef typename data_table_type_map<E>::type    value_type;

		typedef std::vector<value_type>                  container_type;
		typedef typename container_type::iterator        iterator;
		typedef typename container_type::const_iterator  const_iterator;
		typedef typename container_type::difference_type difference_type;
	};

	class PROTEAN_DECL data_table_column_base : boost::noncopyable
	{
		/* Construction */
		/****************/
	public:
		data_table_column_base(const std::string& name, variant_base::enum_type_t type) : m_name(name), m_type(type) {}
		virtual ~data_table_column_base() {}

		virtual data_table_column_base* clone() const = 0;
		virtual void resize(size_t n) = 0;

	protected:
		data_table_column_base(const data_table_column_base& rhs) : m_name(rhs.m_name), m_type(rhs.m_type) {}
		void operator=(const data_table_column_base&);

		/* Attributes */
		/**************/
	public:
		virtual const std::string& name() const { return m_name; }
		variant_base::enum_type_t type() const  { return m_type; }

		/* Variant interface */
		/*********************/
	public:
		virtual bool empty() const = 0;
		virtual size_t size() const = 0;
		virtual void clear() = 0;

	public:
		// Variant iterators
		virtual variant_const_iterator_base* begin() const = 0;
		virtual variant_const_iterator_base* end() const = 0;
		virtual variant_iterator_base* begin() = 0;
		virtual variant_iterator_base* end() = 0;

		/* Typed method interfaces */
		/***************************/
	public:
		// Because C++ doesn't permit virtual templated methods, dispatch to 'templated' virtual impl using pointer
		template <typename V>
		void push_back(const V& value);

		template <variant_base::enum_type_t E>
		typename column_traits<E>::const_iterator begin() const;

		template <variant_base::enum_type_t E>
		typename column_traits<E>::const_iterator end() const;

		template <variant_base::enum_type_t E>
		typename column_traits<E>::iterator begin();

		template <variant_base::enum_type_t E>
		typename column_traits<E>::iterator end();

	protected:
		// Method implementations
	#define PROTEAN_DETAIL_DT_COLUMN_DECLARE_PURE_VIRTUAL_IMPLS(r, data, elem)                                                   \
		virtual std::vector<GET_TYPE(elem)>::const_iterator begin_impl(column_traits<GET_ENUM(elem)>::enum_type* = 0) const = 0; \
		virtual std::vector<GET_TYPE(elem)>::const_iterator end_impl(column_traits<GET_ENUM(elem)>::enum_type* = 0) const = 0; \
		virtual std::vector<GET_TYPE(elem)>::iterator begin_impl(column_traits<GET_ENUM(elem)>::enum_type* = 0) = 0; \
		virtual std::vector<GET_TYPE(elem)>::iterator end_impl(column_traits<GET_ENUM(elem)>::enum_type* = 0) = 0;

	#define PROTEAN_DETAIL_DT_COLUMN_DECLARE_PURE_VIRTUAL_PUSH_BACK_IMPL(r, data, type)  \
		virtual void push_back_impl(const type& value, type* = 0) = 0;                   \

		virtual void push_back_impl(const variant& value, variant* = 0) = 0;

		BOOST_PP_SEQ_FOR_EACH(PROTEAN_DETAIL_DT_COLUMN_DECLARE_PURE_VIRTUAL_PUSH_BACK_IMPL, , SUPPORTED_TYPES)
			BOOST_PP_SEQ_FOR_EACH(PROTEAN_DETAIL_DT_COLUMN_DECLARE_PURE_VIRTUAL_IMPLS, , COLUMN_TYPES)

			/* Member variables */
			/********************/
	private:
		const std::string               m_name;
		const variant_base::enum_type_t m_type;
	};

	// Free-standing clone function, required and used by boost::ptr_vector in data_table
	inline data_table_column_base* new_clone(const data_table_column_base& other)
	{
		return other.clone();
	}

}} // namespace protean::detail

#include <protean/detail/data_table_column_base.ipp>

#if defined(_MSC_VER)
#    pragma warning(pop)
#endif

#endif // PROTEAN_DETAIL_DATA_TABLE_COLUMN_BASE_HPP