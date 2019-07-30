#ifndef __VARSTRUCT_FIELD_H__
#define __VARSTRUCT_FIELD_H__

#include <cstddef>
#include <cstdint>
#include <limits>
#include <type_traits>
#include <utility>

namespace varstruct {

	template<typename T> struct is_struct;

	// Compile-time FNV-1a hash.
	constexpr uint64_t FnvHash(const char *str, uint64_t hash = 14695981039346656037ull)
	{
		return *str ? FnvHash(str + 1, (hash ^ *str) * 109951162821ull) : hash;
	}

	// Type representing variadic struct field name.
	template<char... chars>
	struct FieldName {
		static constexpr const char Value[sizeof...(chars) + 1] = {chars..., '\0'};
	};

	// Macros for defining field name type from c-style string literal.
#if VARSTRUCT_USE_STRING_LITERAL_OPERATOR_TEMPLATE

	template<typename T, T... chars>
	constexpr FieldName<chars...> operator""_fn()
	{
		return {};
	}

#define F_NAME(STR) decltype(STR##_fn)
#else
#define EXTRACT_CHAR(STR, IDX) (IDX < sizeof(STR)) ? STR[IDX] : '\0'
#define F_NAME(STR) FieldName< \
	EXTRACT_CHAR(STR, 0),  EXTRACT_CHAR(STR, 1),  EXTRACT_CHAR(STR, 2),  EXTRACT_CHAR(STR, 3),  EXTRACT_CHAR(STR, 4),  \
	EXTRACT_CHAR(STR, 5),  EXTRACT_CHAR(STR, 6),  EXTRACT_CHAR(STR, 7),  EXTRACT_CHAR(STR, 8),  EXTRACT_CHAR(STR, 9),  \
	EXTRACT_CHAR(STR, 10), EXTRACT_CHAR(STR, 11), EXTRACT_CHAR(STR, 12), EXTRACT_CHAR(STR, 13), EXTRACT_CHAR(STR, 14), \
	EXTRACT_CHAR(STR, 15), EXTRACT_CHAR(STR, 16), EXTRACT_CHAR(STR, 17), EXTRACT_CHAR(STR, 18), EXTRACT_CHAR(STR, 19), \
	EXTRACT_CHAR(STR, 20), EXTRACT_CHAR(STR, 21), EXTRACT_CHAR(STR, 22), EXTRACT_CHAR(STR, 23), EXTRACT_CHAR(STR, 24), \
	EXTRACT_CHAR(STR, 25), EXTRACT_CHAR(STR, 26), EXTRACT_CHAR(STR, 27), EXTRACT_CHAR(STR, 28), EXTRACT_CHAR(STR, 29), \
	EXTRACT_CHAR(STR, 30), EXTRACT_CHAR(STR, 31), EXTRACT_CHAR(STR, 32), EXTRACT_CHAR(STR, 33), EXTRACT_CHAR(STR, 34), \
	EXTRACT_CHAR(STR, 35), EXTRACT_CHAR(STR, 36), EXTRACT_CHAR(STR, 37), EXTRACT_CHAR(STR, 38), EXTRACT_CHAR(STR, 39), \
	EXTRACT_CHAR(STR, 40), EXTRACT_CHAR(STR, 41), EXTRACT_CHAR(STR, 42), EXTRACT_CHAR(STR, 43), EXTRACT_CHAR(STR, 44), \
	EXTRACT_CHAR(STR, 45), EXTRACT_CHAR(STR, 46), EXTRACT_CHAR(STR, 47), EXTRACT_CHAR(STR, 48), EXTRACT_CHAR(STR, 49)  \
	>
#endif

	// Literal for accessing fields in variadic struct.
	constexpr uint64_t operator""_(const char* fieldName, size_t)
	{
		return FnvHash(fieldName);
	}

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
