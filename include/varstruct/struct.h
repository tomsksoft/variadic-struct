#ifndef __VARSTRUCT_STRUCT_H__
#define __VARSTRUCT_STRUCT_H__

#include <varstruct/field.h>
#include <varstruct/traits.h>

#include <any>
#include <tuple>
#include <type_traits>
#include <utility>

namespace varstruct {

	// Tuple-like variadic struct.
	template<typename... TFields>
	class Varstruct: private TFields... {
	public:
		using Type = Varstruct<TFields...>;

		using TFields::value...;
		using TFields::set...;
		using TFields::isSet...;

		// Overloads that produce readable compiler error when none of inherited methods fit.
		// Note that we use '...' as methods parameter because such overloads are picked last.

		template<
			uint64_t FieldId,
			typename = typename std::enable_if<((TFields::Traits::Id() != FieldId) && ...)>::type>
		[[noreturn]] std::any value(...) const
		{
			static_assert(FieldId < 0, "unknown variadic struct field");
		}

		template<
			uint64_t FieldId,
			typename = typename std::enable_if<((TFields::Traits::Id() != FieldId) && ...)>::type>
		[[noreturn]] bool set(...)
		{
			static_assert(FieldId < 0, "unknown variadic struct field");
		}

		template<
			uint64_t FieldId,
			typename = typename std::enable_if<((TFields::Traits::Id() != FieldId) && ...)>::type>
		[[noreturn]] bool& isSet(...)
		{
			static_assert(FieldId < 0, "unknown variadic struct field");
		}

		template<
			uint64_t FieldId,
			typename = typename std::enable_if<((TFields::Traits::Id() != FieldId) && ...)>::type>
		[[noreturn]] const bool& isSet(...) const
		{
			static_assert(FieldId < 0, "unknown variadic struct field");
		}

	public:
		Varstruct() = default;

		constexpr Varstruct(DefaultInitialized): TFields(Default_Value)...
		{ }

		template<
			typename... TArgs,
			typename = typename std::enable_if<sizeof...(TArgs) <= size_v<Type>>::type,
			typename = typename std::enable_if<sizeof...(TArgs) != 1 || // should not hide default copy and move!
			                                   !(std::is_same_v<std::decay_t<TArgs>, Type> && ...)>::type>
		constexpr Varstruct(TArgs&&... args):
			Varstruct(std::tuple_cat(std::forward_as_tuple(std::forward<TArgs>(args)...),
			                         MakeTail(std::make_index_sequence<size_v<Type> - sizeof...(TArgs)>{})),
			          std::make_index_sequence<size_v<Type>>{})
		{ }

		template<
			typename... TArgs,
			typename = typename std::enable_if<(sizeof...(TArgs) > size_v<Type>)>::type,
			typename = typename std::enable_if<sizeof...(TArgs) != 1 || // should not hide default copy and move!
			                                   !(std::is_same_v<std::decay_t<TArgs>, Type> && ...)>::type,
			typename = void> // to distinguish from previous constructor
		constexpr Varstruct(TArgs&&... args)
		{ static_assert(sizeof...(TArgs) < 0, "too many arguments in variadic struct constructor"); }

	private:
		template<typename... TArgs, size_t... Idx>
		constexpr Varstruct(std::tuple<TArgs...> args, std::index_sequence<Idx...>):
			TFields(std::forward<TArgs>(std::get<Idx>(args)))...
		{ }

	private:
		template<size_t I>
		using TailType = NullValue;

		template<size_t... Idx>
		static constexpr auto MakeTail(std::index_sequence<Idx...>)
		{
			return std::forward_as_tuple(TailType<Idx>{}...);
		}
	};

	// Tuple-like variadic struct which inherits from another variadic struct.
	template<typename... TParentFields, typename... TFields>
	class Varstruct<Varstruct<TParentFields...>, TFields...>: public Varstruct<TParentFields...>,
	                                                          private TFields... {
	public:
		using Type = Varstruct<Varstruct<TParentFields...>, TFields...>;
		using ParentType = Varstruct<TParentFields...>;

		using TFields::value...;
		using TFields::set...;
		using TFields::isSet...;

		// Methods to access fields declared in parent. We do not add methods from parent with 'using'
		// because we will no be able to generate readable compiler errors in that case.

		template<
			uint64_t FieldId,
			typename = typename std::enable_if<((TFields::Traits::Id() != FieldId) && ...)>::type>
		constexpr auto& value(...) &
		{
			return ParentType::template value<FieldId>();
		}

		template<
			uint64_t FieldId,
			typename = typename std::enable_if<((TFields::Traits::Id() != FieldId) && ...)>::type>
		constexpr const auto& value(...) const&
		{
			return ParentType::template value<FieldId>();
		}

		template<
			uint64_t FieldId,
			typename = typename std::enable_if<((TFields::Traits::Id() != FieldId) && ...)>::type>
		constexpr auto&& value(...) &&
		{
			return std::move(*this).ParentType::template value<FieldId>();
		}

		template<
			uint64_t FieldId,
			typename = typename std::enable_if<((TFields::Traits::Id() != FieldId) && ...)>::type>
		constexpr const auto&& value(...) const&&
		{
			return std::move(*this).ParentType::template value<FieldId>();
		}

		template<
			uint64_t FieldId,
			typename TArg,
			typename = typename std::enable_if<((TFields::Traits::Id() != FieldId) && ...)>::type>
		constexpr bool set(TArg&& arg, ...)
		{
			return ParentType::template set<FieldId>(std::forward<TArg>(arg));
		}

		template<
			uint64_t FieldId,
			typename = typename std::enable_if<((TFields::Traits::Id() != FieldId) && ...)>::type>
		constexpr bool& isSet(...)
		{
			return ParentType::template isSet<FieldId>();
		}

		template<
			uint64_t FieldId,
			typename = typename std::enable_if<((TFields::Traits::Id() != FieldId) && ...)>::type>
		constexpr const bool& isSet(...) const
		{
			return ParentType::template isSet<FieldId>();
		}

	public:
		Varstruct() = default;

		constexpr Varstruct(DefaultInitialized):
			ParentType(Default_Initialized),
			TFields(Default_Value)...
		{ }

		template<
			typename... TArgs,
			typename = typename std::enable_if<sizeof...(TArgs) <= size_v<Type>>::type,
			typename = typename std::enable_if<sizeof...(TArgs) != 1 || // should not hide default copy and move!
			                                   !(std::is_same_v<std::decay_t<TArgs>, Type> && ...)>::type>
		constexpr Varstruct(TArgs&&... args):
			Varstruct(std::tuple_cat(std::forward_as_tuple(std::forward<TArgs>(args)...),
			                         MakeTail(std::make_index_sequence<size_v<Type> - sizeof...(TArgs)>{})),
			          std::make_index_sequence<size_v<ParentType>>{},
			          std::make_index_sequence<sizeof...(TFields)>{})
		{ }

		template<
			typename... TArgs,
			typename = typename std::enable_if<(sizeof...(TArgs) > size_v<Type>)>::type,
			typename = typename std::enable_if<sizeof...(TArgs) != 1 || // should not hide default copy and move!
			                                   !(std::is_same_v<std::decay_t<TArgs>, Type> && ...)>::type,
			typename = void> // to distinguish from previous constructor
		constexpr Varstruct(TArgs&&... args)
		{ static_assert(sizeof...(TArgs) < 0, "invalid arguments count in variadic struct constructor"); }

	private:
		template<typename... TArgs, size_t... ParentIdx, size_t... FieldIdx>
		constexpr Varstruct(std::tuple<TArgs...> args,
		                    std::index_sequence<ParentIdx...> parentIdx,
		                    std::index_sequence<FieldIdx...> fieldIdx):
			Varstruct(SplitArgs<0>(args, parentIdx), parentIdx,
			          SplitArgs<sizeof...(ParentIdx)>(args, fieldIdx), fieldIdx)
		{ }

		template<typename... TParentArgs, size_t... ParentIdx,
		         typename... TFieldArgs,  size_t... FieldIdx>
		Varstruct(std::tuple<TParentArgs...> parentArgs, std::index_sequence<ParentIdx...>,
		          std::tuple<TFieldArgs...>  fieldArgs,  std::index_sequence<FieldIdx...>):
			ParentType(std::forward<TParentArgs>(std::get<ParentIdx>(parentArgs))...),
			TFields(std::forward<TFieldArgs>(std::get<FieldIdx>(fieldArgs)))...
		{ }

	private:
		template<size_t I>
		using TailType = NullValue;

		template<size_t... Idx>
		static constexpr auto MakeTail(std::index_sequence<Idx...>)
		{
			return std::forward_as_tuple(TailType<Idx>{}...);
		}

		template<size_t Start, typename... TArgs, size_t... Idx>
		static constexpr auto SplitArgs(const std::tuple<TArgs...>& args, std::index_sequence<Idx...>)
		{
			return std::forward_as_tuple(std::forward<typename std::tuple_element<Start + Idx, std::tuple<TArgs...>>::type>(
				std::get<Start + Idx>(args))...);
		}
	};

	// Empty variadic struct.
	using EmptyStruct = Varstruct<>;
}

#endif
