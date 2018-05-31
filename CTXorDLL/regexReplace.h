#pragma once
#include <cstdlib>
#include <iostream>
#include <string>
#include <regex>

// https://stackoverflow.com/questions/22617209/regex-replace-with-callback-in-c11
namespace std
{
	template<class BidirIt, class Traits, class CharT, class UnaryFunction>
	std::basic_string<CharT> regex_replace(BidirIt first, BidirIt last, const std::basic_regex<CharT, Traits>& re, UnaryFunction f, bool& foundAny)
	{
		std::basic_string<CharT> s;

		typename std::match_results<BidirIt>::difference_type
			positionOfLastMatch = 0;
		auto endOfLastMatch = first;

		auto callback = [&](const std::match_results<BidirIt>& match)
		{
			auto positionOfThisMatch = match.position(0);
			auto diff = positionOfThisMatch - positionOfLastMatch;

			auto startOfThisMatch = endOfLastMatch;
			std::advance(startOfThisMatch, diff);

			s.append(endOfLastMatch, startOfThisMatch);
			s.append(f(match));

			auto lengthOfMatch = match.length(0);

			positionOfLastMatch = positionOfThisMatch + lengthOfMatch;

			endOfLastMatch = startOfThisMatch;
			std::advance(endOfLastMatch, lengthOfMatch);
		};

		std::regex_iterator<BidirIt> begin(first, last, re), end;
		foundAny = (begin != end); // iterator empty if begin == end
		std::for_each(begin, end, callback);

		s.append(endOfLastMatch, last);

		return s;
	}

	template<class Traits, class CharT, class UnaryFunction>
	std::basic_string<CharT> regex_replace(const std::basic_string<CharT>& s,
		const std::basic_regex<CharT, Traits>& re, UnaryFunction f, bool& foundAny)
	{
		return regex_replace(s.cbegin(), s.cend(), re, f, foundAny);
	}

} // namespace std

template<typename Callback>
std::wstring regexReplaceMacro(const std::wstring& input, Callback callback, bool& foundAny) {
	// matches ENC( newlines ( C-string capture ) newlines )
	//ENC\([\r\n|\r|\n]*(\"(?:\\.|[^"\\])*\")[\r\n|\r|\n]*\)
	static std::wregex enc_macro(L"ENC\\([\\r\\n|\\r|\\n]*\\\"((?:\\\\.|[^\"\\\\])*)\\\"[\\r\\n|\\r|\\n]*\\)");
	return std::regex_replace(input, enc_macro, callback, foundAny);
}