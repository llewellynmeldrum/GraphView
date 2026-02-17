#pragma once
#include "ansi_colors.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <queue>
#include <source_location>
#include <string>
#include <utility>
#include <array>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <sys/ioctl.h>
#include <print>



namespace LM {
class CFG {
  public:
	std::ostream &log_ostream = std::cerr;
	static CFG &get() {
		static CFG cfg;
		return cfg;
	};

  private:
	CFG() = default;
	CFG(const CFG &) = delete;
	CFG &operator=(const CFG &) = delete;
};


// C++ STL SUCKS AT NAMING THINGS:
template<typename T>
using hash_set = std::unordered_set<T>;

template<typename T, typename U>
using hash_map = std::unordered_map<T, U>;

template<typename T>
using max_heap = std::priority_queue<T, std::vector<T>, std::less<T >>;

template<typename T>
using min_heap = std::priority_queue<T, std::vector<T>, std::greater<T >>;

// how pop shouldve been.
template <typename T>
T pop_front(std::queue<T> &q) {
	assert(!q.empty());
	T value = std::move(q.front());
	q.pop();
	return value;
}

template <typename T>
T pop_top(LM::min_heap<T> &q) {
	assert(!q.empty());
	T value = std::move(q.top());
	q.pop();
	return value;
}

template <typename... Args>
inline void cerr(Args&&... args) {
	(std::cerr << ... << args);
	std::cerr << std::endl;
}

template <typename... Args>
inline void log(Args&&... args) {
	std::cerr << ansi::fg_red;
	(std::cerr << ... << args);
	std::cerr << ansi::reset << std::endl;
}

template <typename... Args>
inline void log_bold(Args&&... args) {
	std::cerr << ansi::bold;
	(std::cerr << ... << args);
	std::cerr << ansi::reset;
}


static constexpr int ERROR = -1;

class TERM {
  public:
// term utils
	static inline ssize_t columnCount() {
		struct winsize w;
		ioctl(0, TIOCGWINSZ, &w);
		return (ssize_t)w.ws_col;
	}


	static inline std::string getLineBreak(const std::string & s) {
		std::ostringstream oss;
		for (int64_t i = 0; i < columnCount(); i++) {
			oss << s;
		}
		return oss.str();
	}
};

class string {
  public:
	static inline std::string toHex(const std::string & s) {
		std::ostringstream hex;
		for (unsigned char c : s) hex << ' ' << std::hex << (int)c;
		return hex.str();
	}

	// apparently std::array requires two sets of curly braces for initializatin, no idea why
	static constexpr std::array<std::pair<char, const char *>, 100> matches = {
		{
			{'\r', "\\r"},
			{'\n', "\\n"},
			{'\t', "\\t"},
			{' ', "␣"},
			{'\e', "\\e"},
		}
	};


	static inline std::string toLiterals(const std::string & s, bool prettyPrint = true) {
		std::ostringstream oss;
		std::string fmt_norepl = ansi::reset;
		std::string fmt_repl = ansi::reset;

		if (prettyPrint) {
			fmt_repl += ansi::bg_grey5;
			fmt_repl += ansi::fg_white;
			fmt_repl += ansi::underline;
		}

		oss << fmt_norepl;

		for (unsigned char ch : s) {
			bool replaced = false;
			for (const auto&[match, replacement] : matches) {
				if (match == ch) {
					replaced = true;
					oss << fmt_repl  << replacement;
				}
			}

			if (!replaced) oss << fmt_norepl << ch;

		}
		return oss.str();
	}

/// Returns the substring within `s` that starts at `left`, whose last character is `s[right-1]`
	static inline std::string between(const std::string & s, size_t left, size_t right) {
		long int pos = left;
		long int count = right - left;
		return s.substr(pos, count);
	}

// Trims whitespace
	static inline std::string trim(std::string & s, const char c) {
		return trim(s, &""[c]);
	}

	static inline std::string trim(std::string & s, const std::string targets = " \t\r\n") {
		for (int i = 0; i < s.size(); i++) {
			for (char target : targets) {
				if (s[i] == target)
					s.erase(i, 1);

			}
		}
		return s;
	}
// returns a vector of 'tokens' split by delimiter
	static inline std::vector<std::string> &split(std::string & s, char delim) {
		auto tokens = new std::vector<std::string>({});
		if (s.find(delim) == std::string::npos) {
			return *tokens;
		}
		size_t end = 0;
		size_t start = 0;
//		std::cout << "splitting: '" << s << "'\n";
		size_t iter = 0;
		while(end >= start) {
			start = end;
//			printf("[%02zu]start| end\n", iter++);
//			printf("    %02zu   | %02zu\n", start, end);
			end = s.find(delim, start) + 1;
			std::string tok = "";
			if (end == std::string::npos) {
				// capture the remaining token
				tok = LM::string::between(s, start, s.size() - 1);
//				printf("    %02zu   | %02zu --> %s \n", start, s.size() - 1, tok.c_str());
				tokens->push_back(tok);
				break;
			}
			tok = LM::string::between(s, start, end - 1);
//			printf("    %02zu   | %02zu --> '%s' \n", start, end, tok.c_str());
			tokens->push_back(tok);
		}
		return *tokens;
	}
};

inline constexpr std::size_t NOT_FOUND = std::string::npos;


inline int get_line_number(const char* filename, const char* fn_name) {
	std::ifstream file(filename);
	if (!file.is_open()) {
		std::println("Error opening file '{}'. ", filename);
		perror("System error message");
		return LM::ERROR;
	}
	std::string line_buf;
	int line_n = 0;
	while (std::getline(file, line_buf)) {
		++line_n;
		if (line_buf.find(fn_name) != NOT_FOUND ) {
			return line_n;
		}
	}
	return LM::ERROR;
}


}
