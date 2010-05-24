//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef PROTEAN_CLR_VARIANT_HPP
#define PROTEAN_CLR_VARIANT_HPP

#include <protean/variant.hpp>

namespace protean { namespace clr {

    interface class IVariantObject;
    ref class VariantObjectProxy;
    ref class ExceptionInfo;
    ref class VariantItem;

    [System::Reflection::DefaultMember("Item")]
    public ref class Variant :
        public System::Collections::Generic::IEnumerable<VariantItem^>,
        public System::IConvertible,
        public System::IComparable,
        public System::IEquatable<Variant^>

    {
    public:

        enum class EnumType
        {
            None                    = protean::variant::None,
            Any                     = protean::variant::Any,
            String                  = protean::variant::String,
            Boolean                 = protean::variant::Boolean,
            Int32                   = protean::variant::Int32,
            UInt32                  = protean::variant::UInt32,
            Int64                   = protean::variant::Int64,
            UInt64                  = protean::variant::UInt64,
            Float                   = protean::variant::Float,
            Double                  = protean::variant::Double,
            Date                    = protean::variant::Date,
            Time                    = protean::variant::Time,
            DateTime                = protean::variant::DateTime,
            List                    = protean::variant::List,
            Dictionary              = protean::variant::Dictionary,
            Bag                     = protean::variant::Bag,
            Buffer                  = protean::variant::Buffer,
            Tuple                   = protean::variant::Tuple,
            Exception               = protean::variant::Exception,
            TimeSeries              = protean::variant::TimeSeries,
            Object                  = protean::variant::Object,
            Collection              = protean::variant::Collection,
            Mapping                 = protean::variant::Mapping,
            Sequence                = protean::variant::Sequence
        };

    public:
        Variant();
        ~Variant();
        !Variant();

        explicit Variant(Variant^ arg);
        explicit Variant(EnumType arg);
		explicit Variant(EnumType arg, System::Int32 size);
        explicit Variant(System::String^ arg);
        explicit Variant(bool arg);
        explicit Variant(System::Int32 arg);
        explicit Variant(System::UInt32 arg);
        explicit Variant(System::Int64 arg);
        explicit Variant(System::UInt64 arg);
        explicit Variant(float arg);
        explicit Variant(double arg);
        explicit Variant(System::TimeSpan arg);
        explicit Variant(System::DateTime arg);
        explicit Variant(System::DateTime arg, bool ignoreTime);
        explicit Variant(array<System::Byte>^ arg);
        explicit Variant(IVariantObject^ arg);
        explicit Variant(VariantObjectProxy^ arg);
        explicit Variant(System::Exception^ arg);
        explicit Variant(ExceptionInfo^ arg);


        /**
         * Type inspection methods
         */
        bool Is(EnumType type);

        /**
         * Type query methods
         */
        System::String^         AsString();
        bool                    AsBoolean();
        System::Int32           AsInt32();
        System::UInt32          AsUInt32();
        System::Int64           AsInt64();
        System::UInt64          AsUInt64();
        float                   AsFloat();
        double                  AsDouble();
        System::DateTime        AsDate();
        System::TimeSpan        AsTime();
        System::DateTime        AsDateTime();
        array<System::Byte>^    AsBuffer();

        generic <class T>
        where T : IVariantObject, gcnew()
        T AsObject();

        VariantObjectProxy^     AsObject();

        ExceptionInfo^          AsException();

        // Lists
        void Add(Variant^ value);

        // Bags/Dictionaries
        bool ContainsKey(System::String^ key);
        void Add(System::String^ key, Variant^ value);
        void Remove(System::String^ key);
		Variant^ Range(System::String^ key);
		Variant^ Select(System::String^ path);

        // TimeSeries
        void Add(System::DateTime time, Variant^ value);

        property int Count  { int get(); }
        property bool Empty { bool get(); }

        property Variant^ Item [System::String^]
        { 
            Variant^ get(System::String^ key); 
            void set(System::String^ key, Variant^ value); 
        }

        property Variant^ Item [System::Int32]
        { 
            Variant^ get(System::Int32 index); 
            void set(System::Int32 index, Variant^ value); 
        }

        virtual System::String^ ToString() override;

        System::String^ ToString(bool summarise);

        virtual System::Collections::Generic::IEnumerator<VariantItem^>^ GetEnumerator_Generic() sealed
            = System::Collections::Generic::IEnumerable<VariantItem^>::GetEnumerator;
        
        virtual System::Collections::IEnumerator^ GetEnumerator_NonGeneric() sealed
            = System::Collections::IEnumerable::GetEnumerator;
        
        property EnumType Type { EnumType get(); }

        static System::String^ TypeToString(EnumType type);

        // System::IConvertible
        virtual System::TypeCode GetTypeCode();
        virtual bool ToBoolean(System::IFormatProvider^ /*provider*/ ) = System::IConvertible::ToBoolean;
        virtual System::Byte ToByte(System::IFormatProvider^ /*provider*/) = System::IConvertible::ToByte;
        virtual System::Char ToChar(System::IFormatProvider^ /*provider*/) = System::IConvertible::ToChar;
        virtual System::DateTime ToDateTime(System::IFormatProvider^ /*provider*/) = System::IConvertible::ToDateTime;
        virtual System::Decimal ToDecimal(System::IFormatProvider^ /*provider*/) = System::IConvertible::ToDecimal;
        virtual double ToDouble(System::IFormatProvider^ /*provider*/) = System::IConvertible::ToDouble;
        virtual short ToInt16(System::IFormatProvider^ /*provider*/) = System::IConvertible::ToInt16;
        virtual int ToInt32(System::IFormatProvider^ /*provider*/) = System::IConvertible::ToInt32;
        virtual System::Int64 ToInt64(System::IFormatProvider^ /*provider*/) = System::IConvertible::ToInt64;
        virtual signed char ToSByte(System::IFormatProvider^ /*provider*/) = System::IConvertible::ToSByte;
        virtual float ToSingle(System::IFormatProvider^ /*provider*/) = System::IConvertible::ToSingle;
        virtual System::String^ ToString(System::IFormatProvider^ /*provider*/) = System::IConvertible::ToString;
        virtual System::Object^ ToType(System::Type^ conversionType, System::IFormatProvider^ /*provider*/) = System::IConvertible::ToType;
        virtual System::UInt16 ToUInt16(System::IFormatProvider^ /*provider*/) = System::IConvertible::ToUInt16;
        virtual System::UInt32 ToUInt32(System::IFormatProvider^ /*provider*/) = System::IConvertible::ToUInt32;
        virtual System::UInt64 ToUInt64(System::IFormatProvider^ /*provider*/) = System::IConvertible::ToUInt64;

        // System::IComparable
        virtual System::Int32 CompareTo(System::Object^ obj);
        System::Int32 CompareTo(Variant^ obj);

        // System::IEquatable
        virtual bool Equals(Variant^ obj);

    internal:
        protean::variant& get_internals()
        {
            return *m_variant;
        }

        // rhs will be copied
        explicit Variant(const protean::variant& rhs)
        {
            m_variant = new protean::variant(rhs);
        }

    private:
        protean::variant*    m_variant;
    };

}}

#endif //PROTEAN_CLR_VARIANT_HPP
