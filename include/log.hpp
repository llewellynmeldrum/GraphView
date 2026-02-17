#pragma once
//#------------------------------------
//# LOGGING
//#------------------------------------
#include <print>
#define OSTREAM __stderrp

#define c_assert(invariant)((invariant) ?\
	void() :  \
	LOG::err_exit("{}:{} -> Assertion failed: {}",__FILE__,__LINE__, #invariant))
struct LOG {

	template <class... Args>
	static void err(std::format_string<Args...> fmt, Args && ... args) {
		std::print(OSTREAM, "");
		std::println(OSTREAM, fmt, std::forward<Args>(args)...);
	}

	template <class... Args>
	static void err_exit(std::format_string<Args...> fmt, Args && ... args) {
		std::print(OSTREAM, "");
		std::println(OSTREAM, fmt, std::forward<Args>(args)...);
		std::exit(EXIT_FAILURE);
	}
};

#undef OSTREAM
//#------------------------------------
