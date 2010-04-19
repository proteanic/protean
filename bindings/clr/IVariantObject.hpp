//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef PROTEAN_CLR_IVARIANT_OBJECT_HPP
#define PROTEAN_CLR_IVARIANT_OBJECT_HPP

namespace protean { namespace clr {

	public interface class IVariantObject
	{
	public:
		property System::String^ ClassName
		{
			virtual System::String^ get();
		}
		property System::UInt32 Version
		{
			virtual System::UInt32 get();
		}

		virtual Variant^ Deflate();

		virtual void Inflate(Variant^ params, System::UInt32 version);
	};

}} // protean::clr

#endif //PROTEAN_CLR_IVARIANT_OBJECT_HPP
