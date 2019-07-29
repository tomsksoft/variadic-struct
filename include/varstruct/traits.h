#ifndef __VARSTRUCT_TRAITS_H__
#define __VARSTRUCT_TRAITS_H__

#include <varstruct/literal.h>
#include <varstruct/field.h>

#include <type_traits>
#include <utility>

namespace varstruct {

	template<typename... TFields> class Varstruct;

	// Provide bool 'value' member.
	template<typename T> struct is_struct;
	template<uint64_t Id, typename TStruct> struct has_field_with_id;
	template<typename TName, typename TStruct> struct has_field_with_name;

	// Provides size_t 'value' member equal to number of fields in variadic struct.
	template<typename TStruct> struct size;

	// Provides Varstruct<...> 'type' member.
	template<typename TStruct> struct parent;

	// Inherit from FieldTraits<...> of requested field.
	template<size_t I, typename TStruct> struct get_field;
	template<uint64_t Id, typename TStruct> struct find_field_by_id;
	template<typename TName, typename TStruct> struct find_field_by_name;

	template<typename T>
	struct is_struct: public std::false_type { };

	template<typename... TFields>
	struct is_struct<Varstruct<TFields...>>: public std::true_type { };

	template<typename T>
	constexpr bool is_struct_v = is_struct<T>::value;

	template<uint64_t Id, typename TName, typename TValue, typename TDescriptor, typename... TFields>
	struct has_field_with_id<Id, Varstruct<Field<TName, TValue, TDescriptor>, TFields...>>:
		public std::conditional<Id == FnvHash(TName::Value),
		                        std::true_type,
		                        has_field_with_id<Id, Varstruct<TFields...>>>::type
	{ };

	template<uint64_t Id, typename... TParentFields, typename... TFields>
	struct has_field_with_id<Id, Varstruct<Varstruct<TParentFields...>, TFields...>>:
		public std::conditional<has_field_with_id<Id, Varstruct<TParentFields...>>::value,
		                        has_field_with_id<Id, Varstruct<TParentFields...>>,
		                        has_field_with_id<Id, Varstruct<TFields...>>>::type
	{ };

	template<uint64_t Id>
	struct has_field_with_id<Id, Varstruct<>>: public std::false_type
	{ };

	template<char... chars, typename... TFields>
	struct has_field_with_name<StrLiteral<chars...>, Varstruct<TFields...>>:
		public has_field_by_id<FnvHash(StrLiteral<chars...>::Value), Varstruct<TFields...>>
	{ };

	template<uint64_t Id, typename TStruct>
	constexpr bool has_field_with_id_v = has_field_with_id<Id, TStruct>::value;

	template<typename TName, typename TStruct>
	constexpr bool has_field_with_name_v = has_field_with_name<TName, TStruct>::value;

	template<typename... TFields>
	struct size<Varstruct<TFields...>>: public std::integral_constant<size_t, sizeof...(TFields)>
	{ };

	template<typename... TParentFields, typename... TFields>
	struct size<Varstruct<Varstruct<TParentFields...>, TFields...>>:
		public std::integral_constant<size_t, size<Varstruct<TParentFields...>>::value + sizeof...(TFields)>
	{ };

	template<typename TStruct>
	constexpr size_t size_v = size<TStruct>::value;

	template<typename... TFields>
	struct parent<Varstruct<TFields...>> {
		static_assert(sizeof...(TFields) < 0, "variadic struct has no parent");
	};

	template<typename... TParentFields, typename... TFields>
	struct parent<Varstruct<Varstruct<TParentFields...>, TFields...>> {
		using type = Varstruct<TParentFields...>;
	};

	template<typename TStruct>
	using parent_t = parent<TStruct>::type;

	template<size_t I, typename TName, typename TValue, typename TDescriptor, typename... TFields>
	struct get_field<I, Varstruct<Field<TName, TValue, TDescriptor>, TFields...>>:
		public get_field<I - 1, Varstruct<TFields...>>
	{ };

	template<typename TName, typename TValue, typename TDescriptor, typename... TFields>
	struct get_field<0, Varstruct<Field<TName, TValue, TDescriptor>, TFields...>>:
		public FieldTraits<TName, TValue, TDescriptor>
	{ };

	template<size_t I, typename... TParentFields, typename... TFields>
	struct get_field<I, Varstruct<Varstruct<TParentFields...>, TFields...>>:
		public std::conditional<I < size_v<Varstruct<TParentFields...>>,
		                        get_field<I, Varstruct<TParentFields...>>,
		                        get_field<I - size_v<Varstruct<TParentFields...>>, Varstruct<TFields...>>>::type
	{ };

	template<size_t I> struct get_field<I, Varstruct<>> {
		static_assert(I < 0, "variadic struct field index is out of range");
	};

	template<size_t I, typename TStruct>
	constexpr uint64_t field_id_v = get_field<I, TStruct>::Id();

	template<size_t I, typename TStruct>
	constexpr const char* field_name_v = get_field<I, TStruct>::Name();

	template<size_t I, typename TStruct>
	using field_name_t = typename get_field<I, TStruct>::NameType;

	template<size_t I, typename TStruct>
	using field_type_t = typename get_field<I, TStruct>::ValueType;

	template<size_t I, typename TStruct>
	using field_descriptor_t = typename get_field<I, TStruct>::DescriptorType;

	template<uint64_t Id, typename TName, typename TValue, typename TDescriptor, typename... TFields>
	struct find_field_by_id<Id, Varstruct<Field<TName, TValue, TDescriptor>, TFields...>>:
		public std::conditional<Id == FnvHash(TName::Value),
		                        FieldTraits<TName, TValue, TDescriptor>,
		                        find_field_by_id<Id, Varstruct<TFields...>>>::type
	{ };

	template<uint64_t Id, typename... TParentFields, typename... TFields>
	struct find_field_by_id<Id, Varstruct<Varstruct<TParentFields...>, TFields...>>:
		public std::conditional<has_field_with_id_v<Id, Varstruct<TParentFields...>>,
		                        find_field_by_id<Id, Varstruct<TParentFields...>>,
		                        find_field_by_id<Id, Varstruct<TFields...>>>::type
	{ };

	template<uint64_t Id>
	struct find_field_by_id<Id, Varstruct<>> {
		static_assert(Id < 0, "variadic struct has no field with specified id");
	};

	template<char... chars, typename... TFields>
	struct find_field_by_name<StrLiteral<chars...>, Varstruct<TFields...>>:
		public find_field_by_id<FnvHash(StrLiteral<chars...>::Value), Varstruct<TFields...>>
	{ };

	// Added for consistency with std::tuple.

	template<size_t I, typename... TFields>
	constexpr auto get(const Varstruct<TFields...>& obj) ->
		typename std::tuple_element<I, Varstruct<TFields...>>::type const&
	{
		return obj.template value<field_id_v<I, Varstruct<TFields...>>>();
	}

	template<size_t I, typename... TFields>
	constexpr auto get(Varstruct<TFields...>& obj) ->
		typename std::tuple_element<I, Varstruct<TFields...>>::type&
	{
		return obj.template value<field_id_v<I, Varstruct<TFields...>>>();
	}

	template<size_t I, typename... TFields>
	constexpr auto get(const Varstruct<TFields...>&& obj) ->
		typename std::tuple_element<I, Varstruct<TFields...>>::type const&&
	{
		return (std::move(obj)).template value<field_id_v<I, Varstruct<TFields...>>>();
	}

	template<size_t I, typename... TFields>
	constexpr auto get(Varstruct<TFields...>&& obj) ->
		typename std::tuple_element<I, Varstruct<TFields...>>::type&&
	{
		return (std::move(obj)).template value<field_id_v<I, Varstruct<TFields...>>>();
	}
}

namespace std {

	template<typename... TFields>
	class tuple_size<varstruct::Varstruct<TFields...>>:
		public std::integral_constant<size_t, varstruct::size_v<varstruct::Varstruct<TFields...>>>
	{ };

	template<size_t I, typename... TFields>
	struct tuple_element<I, varstruct::Varstruct<TFields...>> {
		using type = varstruct::field_type_t<I, varstruct::Varstruct<TFields...>>;
	};
}

#endif
