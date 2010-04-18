//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef PROTEAN_CLR_VARIANT_ITEM_HPP
#define PROTEAN_CLR_VARIANT_ITEM_HPP

#include "Variant.hpp"

namespace protean { namespace clr {

	public ref class VariantItem 
	{
		enum class EnumType
		{
			Sequence,
			Mapping,
			TimeSeries
		};

	public:
		VariantItem(Variant^ value);
		VariantItem(System::String^ name, Variant^ value);
		VariantItem(System::DateTime index, Variant^ value);

	public:

		property System::String^ Key
		{
			System::String^ get();
		}
		property System::DateTime Time
		{
			System::DateTime get();
		}
		property Variant^ Value
		{
			Variant^ get();
			void set(Variant^ value);
		}
	
		virtual System::String^ ToString() override;

	private:
		EnumType			m_type;
		System::String^		m_name;
		System::DateTime 	m_index;
		Variant^			m_value;
	};

}}

#endif // PROTEAN_CLR_VARIANT_ITEM_HPP
