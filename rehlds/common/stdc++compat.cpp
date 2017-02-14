#include <stdio.h>

#if !defined(_WIN32)
void NORETURN Sys_Error(const char *error, ...);

// This file adds the necessary compatibility tricks to avoid symbols with
// version GLIBCXX_3.4.16 and bigger, keeping binary compatibility with libstdc++ 4.6.1.
namespace std
{
	// We shouldn't be throwing exceptions at all, but it sadly turns out we call STL (inline) functions that do.
	void __throw_out_of_range_fmt(const char *fmt, ...)
	{
		va_list ap;
		char buf[1024]; // That should be big enough.

		va_start(ap, fmt);
		vsnprintf(buf, sizeof(buf), fmt, ap);
		buf[sizeof(buf) - 1] = '\0';
		va_end(ap);

		Sys_Error(buf);
	}
}; // namespace std

// Technically, this symbol is not in GLIBCXX_3.4.20, but in CXXABI_1.3.8,
// but that's equivalent, version-wise. Those calls are added by the compiler
// itself on `new Class[n]` calls.
extern "C"
void __cxa_throw_bad_array_new_length()
{
	Sys_Error("Bad array new length.");
}
#endif // !defined(_WIN32)
