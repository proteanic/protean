//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef PROTEAN_CLR_VARIANT_ENUMERATOR_HPP
#define PROTEAN_CLR_VARIANT_ENUMERATOR_HPP

#include "Variant.hpp"

namespace Protean { namespace CLR {

	public ref class VariantEnumerator :
		public System::Collections::IEnumerator
	{
	internal:
		enum class EnumState
		{
			ResetState,
			InUseState,
			PastEndState
		};
	public:
		VariantEnumerator(Variant^ parent);
		~VariantEnumerator();
		!VariantEnumerator();

		virtual bool MoveNext();

		virtual void Reset();

		virtual property System::Object^ Current
		{
			System::Object^ get();
		}

	private:
		Variant^					m_parent;
		EnumState					m_currentState;
		protean::variant::iterator*	m_itCurrent;
		protean::variant::iterator*	m_itEnd;
	};

}} // Protean::CLR

#endif // PROTEAN_CLR_VARIANT_ENUMERATOR_HPP
