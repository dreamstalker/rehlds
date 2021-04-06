#include <stdio.h>
#include <stdlib.h>

#if !defined(_WIN32) && !defined(BUILD_STATIC_LIBSTDC) // if build with static libstdc++ then ignore

// This file adds the necessary compatibility tricks to avoid symbols with
// version GLIBCXX_3.4.16 and bigger, keeping binary compatibility with libstdc++ 4.6.1.
namespace std
{

#if __cpp_exceptions
	logic_error::logic_error(const char *__arg) : exception(), _M_msg(__arg) {}
	out_of_range::out_of_range(const char *__arg) : logic_error(__arg) {}
	out_of_range::~out_of_range() _GLIBCXX_USE_NOEXCEPT {}
#endif // #if __cpp_exceptions

	// We shouldn't be throwing exceptions at all, but it sadly turns out we call STL (inline) functions that do.
	void __throw_out_of_range_fmt(const char *fmt, ...)
	{
	#if __cpp_exceptions
		va_list ap;
		char buf[1024]; // That should be big enough.

		va_start(ap, fmt);
		vsnprintf(buf, sizeof(buf), fmt, ap);
		buf[sizeof(buf) - 1] = '\0';
		va_end(ap);

		throw std::out_of_range(buf);
	#else
		abort();
	#endif
	}
}; // namespace std

// Was added in GCC 4.9
// Technically, this symbol is not in GLIBCXX_3.4.20, but in CXXABI_1.3.8, but that's equivalent, version-wise.
// Those calls are added by the compiler
// itself on `new Class[n]` calls.
extern "C"
void __cxa_throw_bad_array_new_length()
{
#if __cpp_exceptions
	throw std::bad_array_new_length();
#else
	abort();
#endif
}

#if defined(__INTEL_COMPILER) && __cplusplus >= 201402L
// This operator delete sized deallocations was added in c++14
// and required at least not less than CXXABI_1.3.9
// we should to keep CXXABI_1.3.8 for binary compatibility with oldest libstdc++.
// GCC and Clang allow to compile C++14 code with -fno-sized-deallocation to disable the new feature, but ICC isn't allow
// so that our C++14 library code would never call that version of operator delete,
// for ICC compiler we must override those operators for static linking to the library.
void operator delete[](void *ptr, std::size_t size) noexcept
{
	::operator delete(ptr);
}

void operator delete(void *ptr, std::size_t size) noexcept
{
	::operator delete(ptr);
}
#endif

#endif // !defined(_WIN32)
