//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#include "VariantEnumerator.hpp"
#include "VariantItem.hpp"
#include "VariantException.hpp"

namespace protean { namespace clr {

    VariantEnumerator::VariantEnumerator(Variant^ parent) :
		m_parent(parent),
		m_itCurrent(NULL),
		m_itEnd(NULL)
	{
        BEGIN_TRANSLATE_ERROR();

		Reset();

        END_TRANSLATE_ERROR();
	}

	VariantEnumerator::~VariantEnumerator()
	{
		this->!VariantEnumerator();
	}

	VariantEnumerator::!VariantEnumerator()
	{
		Reset();
	}

	bool VariantEnumerator::MoveNext()
	{
        BEGIN_TRANSLATE_ERROR();

		if (m_currentState==EnumState::ResetState)
		{
			m_itCurrent = new protean::variant::iterator(m_parent->get_internals().begin());
			m_itEnd = new protean::variant::iterator(m_parent->get_internals().end());
			
			m_currentState = EnumState::InUseState;
		}
		else if (m_currentState==EnumState::InUseState)
		{
			++(*m_itCurrent);
		}

		if (*m_itCurrent==*m_itEnd)
		{
			m_currentState = EnumState::PastEndState;
			return false;
		}

		return true;

        END_TRANSLATE_ERROR();
	}

	void VariantEnumerator::Reset()
	{
        BEGIN_TRANSLATE_ERROR();

		if (m_itCurrent!=NULL)
		{
			delete m_itCurrent;
			m_itCurrent = NULL;
		}

		if (m_itEnd!=NULL)
		{
			delete m_itEnd;
			m_itEnd = NULL;
		}

		m_currentState = EnumState::ResetState;

        END_TRANSLATE_ERROR();
	}

    System::Object^ VariantEnumerator::Current::get()
	{
        BEGIN_TRANSLATE_ERROR();

		if(m_currentState==EnumState::InUseState)
		{
			Variant^ localValue = gcnew Variant(&m_itCurrent->value());
			if (m_parent->get_internals().is<protean::variant::Mapping>())
			{
				System::String^ localName = gcnew System::String(m_itCurrent->key().c_str());
				return gcnew VariantItem(localName, localValue);
			}
			else if (m_parent->get_internals().is<protean::variant::TimeSeries>())
			{
				protean::variant::date_time_t index(m_itCurrent->time());
				System::DateTime localName = System::DateTime(index.date().year(), index.date().month(), index.date().day(), index.time_of_day().hours(), index.time_of_day().minutes(), index.time_of_day().seconds() );
				return gcnew VariantItem(localValue);
			}
			else
			{
				return gcnew VariantItem(localValue);
			}
		}
		else
		{
			System::String^ msg = System::String::Format("Attempt to call VariantEnumerator::MoveNext with invalid EnumeratorState [value={0}]", m_currentState);
			throw gcnew System::InvalidOperationException(msg);
		}

        END_TRANSLATE_ERROR();
	}

}} // protean::clr
