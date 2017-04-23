#pragma once

#include <iostream>
#include <functional>
#include <string>
#include <memory>
#include <vector>
#include <array>
#include <utility>

#if defined(HOOK_ENGINE) || defined(HOOK_HLTV)

#define private public
#define protected public

template <typename T, typename ... TArgs>
class MethodThunk {
public:
	void Constructor(TArgs ... args) {
		new(this) T(args ...);
	}

	void Destructor() {
		(*(T *)this).~T();
	}
};

namespace MsvcMethod {
	namespace Detail {
		using Counter = std::size_t(*)();

		template <std::size_t N>
		std::size_t GetIndex() {
			return N;
		}

		template <std::size_t ... TIndices>
		constexpr auto GenerateCounters_Helper(std::index_sequence<TIndices ...>) {
			// There is no make_array (and/or deduction guides), so we need to explicitly define array template params
			return std::array<Counter, sizeof ... (TIndices)> { &GetIndex<TIndices> ... };
		}

		template<std::size_t N>
		auto counters = GenerateCounters_Helper(std::make_index_sequence<N>{});

		struct VIndexGetter {};
		struct ThisGetter {
			decltype(auto) GetThis() const {
				return this;
			}

			decltype(auto) GetThis(...) const {
				return this;
			}
		};

		template <class T>
		class Singleton {
		public:
			static T &GetInstance() {
				static T instance = {};
				return instance;
			}
		};

		// primary template
		template<class>
		struct is_variadic_function : std::false_type {};
		template<class R, class ... TArgs>
		struct is_variadic_function<R (TArgs ..., ...)> : std::true_type {};
		template<class R, class ... TArgs>
		struct is_variadic_function<R (TArgs ..., ...) const> : std::true_type {};
		template<class R, class ... TArgs>
		struct is_variadic_function<R (TArgs ..., ...) volatile> : std::true_type {};
		template<class R, class ... TArgs>
		struct is_variadic_function<R (TArgs ..., ...) const volatile> : std::true_type {};
		template<class R, class ... TArgs>
		struct is_variadic_function<R (TArgs ..., ...) &> : std::true_type {};
		template<class R, class ... TArgs>
		struct is_variadic_function<R (TArgs ..., ...) const &> : std::true_type {};
		template<class R, class ... TArgs>
		struct is_variadic_function<R (TArgs ..., ...) volatile &> : std::true_type {};
		template<class R, class ... TArgs>
		struct is_variadic_function<R (TArgs ..., ...) const volatile &> : std::true_type {};
		template<class R, class ... TArgs>
		struct is_variadic_function<R (TArgs ..., ...) &&> : std::true_type {};
		template<class R, class ... TArgs>
		struct is_variadic_function<R (TArgs ..., ...) const &&> : std::true_type {};
		template<class R, class ... TArgs>
		struct is_variadic_function<R (TArgs ..., ...) volatile &&> : std::true_type {};
		template<class R, class ... TArgs>
		struct is_variadic_function<R (TArgs ..., ...) const volatile &&> : std::true_type {};

		template<class T>
		constexpr bool is_variadic_function_v = is_variadic_function<T>::value;

		template<class T>
		constexpr bool is_function_v = std::is_function<T>::value;
	} // namespace Detail

	static constexpr auto& counters = Detail::counters<256>;

	template <typename TMethod, typename T>
	std::enable_if_t<Detail::is_function_v<TMethod>, std::uintptr_t>
	GetVirtualIndex(TMethod T::*method)
	{
		decltype(auto) pcounters = counters.data();
		decltype(auto) vIndexGetter = (Detail::VIndexGetter *)&pcounters;

		using VIndexGetterFunction = std::conditional_t<Detail::is_variadic_function_v<TMethod>, std::size_t (Detail::VIndexGetter::*)(...) const, std::size_t(Detail::VIndexGetter::*)() const>;
		VIndexGetterFunction vIndexGetterFunction;
		{
			*(std::uintptr_t *)&vIndexGetterFunction = *(std::uintptr_t *)&method;
		}

		return (vIndexGetter->*vIndexGetterFunction)();
	}

	template <typename TMethod, typename T>
	std::enable_if_t<Detail::is_function_v<TMethod>, std::uintptr_t>
	GetVirtualAddress(TMethod T::*method)
	{
		using ThisGetterFunction = std::conditional_t<Detail::is_variadic_function_v<TMethod>, const T *(T::*)(...) const, const T *(T::*)() const>;
		ThisGetterFunction thisGetterFunction = *(ThisGetterFunction *)&method;
		{
			decltype(auto) m = static_cast<std::conditional_t<Detail::is_variadic_function_v<TMethod>, const Detail::ThisGetter *(Detail::ThisGetter::*)(...) const, const Detail::ThisGetter *(Detail::ThisGetter::*)() const>>(&Detail::ThisGetter::GetThis);
			*(std::uintptr_t *)&thisGetterFunction = *(std::uintptr_t *)&m;
		}

		return *(*(std::uintptr_t **)(Detail::Singleton<T>::GetInstance().*thisGetterFunction)() + GetVirtualIndex(method));
	}

	template <typename TMethod, typename T>
	std::enable_if_t<Detail::is_function_v<TMethod>, std::uintptr_t>
	GetAddress(TMethod (T::*method)) {
		return (std::uintptr_t &)method;
	}

	template <typename TMethod>
	std::enable_if_t<Detail::is_function_v<TMethod>, std::uintptr_t>
	GetAddress(TMethod (*method)) {
		return (std::uintptr_t &)method;
	}

} // namespace MsvcMethod

#ifdef _MSC_VER
	#define GLOBALVAR_LINK(offset, symbol, var, ...)             { offset, #symbol, (size_t)&##var, __VA_ARGS__ }
	#define HOOK_SYMBOLDEF(offset, symbol, func, ...)            { offset, #symbol, MsvcMethod::GetAddress<__VA_ARGS__>(&func) }
	#define HOOK_SYMBOL_VIRTUAL_DEF(offset, symbol, func, ...)   { offset, #symbol, MsvcMethod::GetVirtualAddress<__VA_ARGS__>(&func) }

	#define HOOK_DEF(offset, func, ...)                          HOOK_SYMBOLDEF(offset, func, func, __VA_ARGS__)
	#define HOOK_VIRTUAL_DEF(offset, func, ...)                  HOOK_SYMBOL_VIRTUAL_DEF(offset, func, func, __VA_ARGS__)

#else
	#error Hooking stuff is only available using MSVC compiler.
#endif // _MSC_VER

#endif // #if defined(HOOK_ENGINE) || defined(HOOK_HLTV)
