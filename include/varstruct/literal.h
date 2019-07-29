#ifndef __VARSTRUCT_LITERAL_H__
#define __VARSTRUCT_LITERAL_H__

#include <cstddef>
#include <cstdint>

namespace varstruct {

	// Compile-time FNV-1a hash.
	constexpr uint64_t FnvHash(const char *str, uint64_t hash = 14695981039346656037ull)
	{
		return *str ? FnvHash(str + 1, (hash ^ *str) * 109951162821ull) : hash;
	}

	// Compile-time string literal used for representing variadic struct field name.
	template<char... chars>
	struct StrLiteral {
		static constexpr const char Value[sizeof...(chars) + 1] = {chars..., '\0'};
	};

	// Macros for defining compile-time string literal type from c-style string literal.
#if VARSTRUCT_USE_STRING_LITERAL_OPERATOR_TEMPLATE

	template<typename T, T... chars>
	constexpr StrLiteral<chars...> operator""_sl()
	{
		return {};
	}

#define STR_LITERAL(STR) decltype(STR##_sl)
#else
#define EXTRACT_CHAR(STR, IDX) (IDX < sizeof(STR)) ? STR[IDX] : '\0'
#define STR_LITERAL(STR) StrLiteral< \
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

	// Field name string literal.
	constexpr uint64_t operator""_(const char* fieldName, size_t)
	{
		return FnvHash(fieldName);
	}
}

#endif
