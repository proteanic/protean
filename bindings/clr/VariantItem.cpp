//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#include "VariantItem.hpp"
#include "VariantException.hpp"

namespace protean { namespace clr {

	VariantItem::VariantItem(Variant^ value) :
		m_type(EnumType::Sequence),
		m_value(value)
	{
	}

	VariantItem::VariantItem(System::String^ name, Variant^ value) :
		m_type(EnumType::Mapping),
		m_name(name),
		m_value(value)
	{
	}

	VariantItem::VariantItem(System::DateTime index, Variant^ value) :
		m_type(EnumType::TimeSeries),
		m_index(index),
		m_value(value)
	{
	}

	System::String^ VariantItem::Key::get()
	{
		if (m_type!=EnumType::Mapping)
		{
			throw gcnew VariantException("Attempt to get Key on item that is not a child of an associative container.");
		}
		return m_name;
	}

	System::DateTime VariantItem::Time::get()
	{
		if (m_type!=EnumType::TimeSeries)
		{
			throw gcnew VariantException("Attempt to get Time on item that is not a child of a timeseries container.");
		}
		return m_index;
	}

	Variant^ VariantItem::Value::get()
	{
		return m_value;
	}

	void VariantItem::Value::set(Variant^ value)
	{
		m_value->get_internals() = value->get_internals();
	}

	System::String^ VariantItem::ToString()
	{
		if (m_type==EnumType::Mapping)
		{
			return System::String::Format(" '{0}':{1} ", m_name, m_value);
		}
		else if (m_type==EnumType::TimeSeries)
		{
			return System::String::Format(" '{0}':{1} ", m_index, m_value);
		}
		else
		{
			return System::String::Format(" {0} ", m_value);
		}
	}

}} // protean::clr
