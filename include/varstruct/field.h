
#ifndef __VARSTRUCT_FIELD_H__
#define __VARSTRUCT_FIELD_H__

#include <varstruct/literal.h>

#include <cstdint>
#include <limits>
#include <type_traits>
#include <utility>

namespace varstruct {

	template<typename T> struct is_struct;

	// Indicator that field is not initialized in constructor.
	struct NullValue { };
	constexpr NullValue Null_Value;

	// Indicator that field is default initialized in constructor.
	struct DefaultValue { };
	constexpr DefaultValue Default_Value;

	// Indicator that all fields of struct are default initialized.
	struct DefaultInitialized { };
	constexpr DefaultInitialized Default_Initialized;

	// Field null descriptor type.
	struct NullDescriptor { };

	// Variadic struct field traits.
	template<typename TName, typename TValue, typename TDescriptor>
	struct FieldTraits {
		using NameType = TName;
		using ValueType = TValue;
		using DescriptorType = TDescriptor;

		static constexpr const char* Name()
		{ return NameType::Value; }

		static constexpr uint64_t Id()
		{ return FnvHash(NameType::Value); }
	};

	// Variadic struct field.
	template<typename TName, typename TValue, typename TDescriptor = NullDescriptor>
	class Field {
	public:
		using Traits = FieldTraits<TName, TValue, TDescriptor>;

	public:
		constexpr Field():
			value_{},
			isSet_(false)
		{ }

		constexpr Field(const TValue& value):
			value_(value),
			isSet_(true)
		{ }

		constexpr Field(TValue&& value):
			value_(std::move(value)),
			isSet_(true)
		{ }

		constexpr Field(NullValue): Field{} { }
		constexpr Field(DefaultValue): Field(GetDefaultValue()) { }

		template<
			uint64_t FieldId,
			typename = typename std::enable_if<FieldId == Traits::Id()>::type>
		constexpr TValue& value() &
		{ return value_; }

		template<
			uint64_t FieldId,
			typename = typename std::enable_if<FieldId == Traits::Id()>::type>
		constexpr const TValue& value() const&
		{ return value_; }

		template<
			uint64_t FieldId,
			typename = typename std::enable_if<FieldId == Traits::Id()>::type>
		constexpr TValue&& value() &&
		{ return std::move(value_); }

		template<
			uint64_t FieldId,
			typename = typename std::enable_if<FieldId == Traits::Id()>::type>
		constexpr const TValue&& value() const&& // this is for consistency with std::ref for example
		{ return std::move(value_); }

		template<
			uint64_t FieldId,
			typename TArg,
			typename = typename std::enable_if<FieldId == Traits::Id()>::type>
		constexpr bool set(TArg&& value)
		{
			if constexpr (!std::is_convertible_v<TArg, TValue>) {
				static_assert(sizeof(TArg) == 0, "argument can't be converted to variadic struct field value");
			}

			using Type = std::decay_t<TArg>;
			bool result = true;

			if constexpr (std::is_arithmetic_v<Type> && std::is_arithmetic_v<TValue>) {
				if constexpr (std::is_integral_v<Type> && std::is_integral_v<TValue>) {
					if constexpr (std::is_signed_v<Type> != std::is_signed_v<TValue>) {
						result = value >= 0 && value <= std::numeric_limits<TValue>::max();
					} else if constexpr (sizeof(Type) >= sizeof(TValue)) {
						result = value >= std::numeric_limits<TValue>::min() && value <= std::numeric_limits<TValue>::max();
					}
				} else if constexpr (std::is_floating_point_v<Type> != std::is_floating_point_v<TValue> ||
				                     sizeof(Type) > sizeof(TValue)) {
					static_assert(sizeof(TArg) == 0, "dangerous argument conversion to variadic struct field value");
				}

				value_ = result ? static_cast<TValue>(value) : TValue{};
			} else {
				value_ = std::forward<TArg>(value);
			}

			return isSet_ = result;
		}

		template<
			uint64_t FieldId,
			typename = typename std::enable_if<FieldId == Traits::Id()>::type>
		constexpr bool& isSet()
		{
			return isSet_;
		}

		template<
			uint64_t FieldId,
			typename = typename std::enable_if<FieldId == Traits::Id()>::type>
		constexpr const bool& isSet() const
		{
			return isSet_;
		}

	private:
		static constexpr TValue GetDefaultValue()
		{
			if constexpr (is_struct<TValue>::value) {
				return TValue(Default_Initialized);
			} else {
				return TValue{};
			}
		}

	private:
		TValue value_;
		bool isSet_;
	};
}

#endif
