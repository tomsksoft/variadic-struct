#ifndef __VARSTRUCT_TRAITS_H__
#define __VARSTRUCT_TRAITS_H__

#include <varstruct/field.h>

#include <tuple>
#include <type_traits>
#include <utility>

namespace varstruct {

	template<typename... TFields> class Varstruct;

	// Provide 'value' member. Also constants with '_v' suffix are defined for corresponding trait::value.
	template<typename T> struct is_struct;
	template<typename TStruct> struct size;
	template<typename TName, typename TStruct> struct has_field;
	template<uint64_t Id, typename TStruct> struct has_field_by_id;

	// Provide 'type' member. Also types 'trait_t' are defined for corresponding trait::type.
	template<typename TStruct> struct parent;
	template<size_t I, typename TStruct> struct get_field;
	template<typename TName, typename TStruct> struct find_field;
	template<uint64_t Id, typename TStruct> struct find_field_by_id;

	template<typename T>
	struct is_struct: public std::false_type { };

	template<typename... TFields>
	struct is_struct<Varstruct<TFields...>>: public std::true_type { };

	template<typename T>
	constexpr bool is_struct_v = is_struct<T>::value;

	template<typename TStruct>
	struct size {
		static_assert(sizeof(TStruct) == 0, "not a variadic struct");
	};

	template<typename... TFields>
	struct size<Varstruct<TFields...>>: public std::integral_constant<size_t, sizeof...(TFields)>
	{ };

	template<typename... TParentFields, typename... TFields>
	struct size<Varstruct<Varstruct<TParentFields...>, TFields...>>:
		public std::integral_constant<size_t, size<Varstruct<TParentFields...>>::value + sizeof...(TFields)>
	{ };

	template<typename TStruct>
	constexpr size_t size_v = size<TStruct>::value;

	template<typename TName, typename TStruct> struct has_field {
		static_assert(sizeof(TStruct) == 0, "not a variadic struct");
	};

	template<uint64_t Id, typename TStruct> struct has_field_by_id {
		static_assert(sizeof(TStruct) == 0, "not a variadic struct");
	};

	template<uint64_t Id, typename TName, typename TValue, typename TDescriptor, typename... TFields>
	struct has_field_by_id<Id, Varstruct<Field<TName, TValue, TDescriptor>, TFields...>>:
		public std::conditional<Id == FnvHash(TName::Value),
		                        std::true_type,
		                        has_field_by_id<Id, Varstruct<TFields...>>>::type
	{ };

	template<uint64_t Id, typename... TParentFields, typename... TFields>
	struct has_field_by_id<Id, Varstruct<Varstruct<TParentFields...>, TFields...>>:
		public std::conditional<has_field_by_id<Id, Varstruct<TParentFields...>>::value,
		                        has_field_by_id<Id, Varstruct<TParentFields...>>,
		                        has_field_by_id<Id, Varstruct<TFields...>>>::type
	{ };

	template<uint64_t Id>
	struct has_field_by_id<Id, Varstruct<>>: public std::false_type
	{ };

	template<char... chars, typename... TFields>
	struct has_field<FieldName<chars...>, Varstruct<TFields...>>:
		public has_field_by_id<FnvHash(FieldName<chars...>::Value), Varstruct<TFields...>>
	{ };

	template<uint64_t Id, typename TStruct>
	constexpr bool has_field_by_id_v = has_field_by_id<Id, TStruct>::value;

	template<typename TName, typename TStruct>
	constexpr bool has_field_v = has_field<TName, TStruct>::value;

	template<typename TStruct> struct parent {
		static_assert(sizeof(TStruct) == 0, "not a variadic struct");
	};

	template<typename... TFields>
	struct parent<Varstruct<TFields...>> {
		static_assert(sizeof...(TFields) < 0, "variadic struct has no parent");
	};

	template<typename... TParentFields, typename... TFields>
	struct parent<Varstruct<Varstruct<TParentFields...>, TFields...>> {
		using type = Varstruct<TParentFields...>;
	};

	template<typename TStruct>
	using parent_t = typename parent<TStruct>::type;

	template<size_t I, typename TStruct> struct get_field {
		static_assert(sizeof(TStruct) == 0, "not a variadic struct");
	};

	template<size_t I, typename TName, typename TValue, typename TDescriptor, typename... TFields>
	struct get_field<I, Varstruct<Field<TName, TValue, TDescriptor>, TFields...>> {
		using type = typename get_field<I - 1, Varstruct<TFields...>>::type;
	};

	template<typename TName, typename TValue, typename TDescriptor, typename... TFields>
	struct get_field<0, Varstruct<Field<TName, TValue, TDescriptor>, TFields...>> {
		using type = FieldTraits<TName, TValue, TDescriptor>;
	};

	template<size_t I, typename... TParentFields, typename... TFields>
	struct get_field<I, Varstruct<Varstruct<TParentFields...>, TFields...>> {
	private:
		static constexpr auto EvalType()
		{
			if constexpr (I < size_v<Varstruct<TParentFields...>>) {
				return typename get_field<I, Varstruct<TParentFields...>>::type{};
			} else {
				return typename get_field<I - size_v<Varstruct<TParentFields...>>, Varstruct<TFields...>>::type{};
			}
		}

	public:
		using type = decltype(EvalType());
	};

	template<size_t I> struct get_field<I, Varstruct<>> {
		static_assert(I < 0, "variadic struct field index is out of range");
	};

	template<size_t I, typename TStruct>
	using get_field_t = typename get_field<I, TStruct>::type;

	template<size_t I, typename TStruct>
	constexpr uint64_t field_id_v = get_field_t<I, TStruct>::Id();

	template<size_t I, typename TStruct>
	constexpr const char* field_name_v = get_field_t<I, TStruct>::Name();

	template<size_t I, typename TStruct>
	using field_name_t = typename get_field_t<I, TStruct>::NameType;

	template<size_t I, typename TStruct>
	using field_type_t = typename get_field_t<I, TStruct>::ValueType;

	template<size_t I, typename TStruct>
	using field_descriptor_t = typename get_field_t<I, TStruct>::DescriptorType;

	template<typename TName, typename TStruct> struct find_field {
		static_assert(sizeof(TStruct) == 0, "not a variadic struct");
	};

	template<uint64_t Id, typename TStruct> struct find_field_by_id {
		static_assert(sizeof(TStruct) == 0, "not a variadic struct");
	};

	template<uint64_t Id, typename TName, typename TValue, typename TDescriptor, typename... TFields>
	struct find_field_by_id<Id, Varstruct<Field<TName, TValue, TDescriptor>, TFields...>> {
	private:
		static constexpr auto EvalType()
		{
			if constexpr (Id == FnvHash(TName::Value)) {
				return FieldTraits<TName, TValue, TDescriptor>{};
			} else {
				return typename find_field_by_id<Id, Varstruct<TFields...>>::type{};
			}
		}

	public:
		using type = decltype(EvalType());
	};

	template<uint64_t Id, typename... TParentFields, typename... TFields>
	struct find_field_by_id<Id, Varstruct<Varstruct<TParentFields...>, TFields...>> {
	private:
		static constexpr auto EvalType()
		{
			if constexpr (has_field_by_id_v<Id, Varstruct<TParentFields...>>) {
				return typename find_field_by_id<Id, Varstruct<TParentFields...>>::type{};
			} else {
				return typename find_field_by_id<Id, Varstruct<TFields...>>::type{};
			}
		}

	public:
		using type = decltype(EvalType());
	};

	template<uint64_t Id>
	struct find_field_by_id<Id, Varstruct<>> {
		static_assert(Id < 0, "variadic struct field is not found");
	};

	template<char... chars, typename... TFields>
	struct find_field<FieldName<chars...>, Varstruct<TFields...>> {
		using type = typename find_field_by_id<FnvHash(FieldName<chars...>::Value), Varstruct<TFields...>>::type;
	};

	template<uint64_t Id, typename TStruct>
	using find_field_by_id_t = typename find_field_by_id<Id, TStruct>::type;

	template<typename TName, typename TStruct>
	using find_field_t = typename find_field<TName, TStruct>::type;

	// Added for structured binding support.

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
