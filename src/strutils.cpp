/*
 * ARPCalc - Al's Reverse Polish Calculator (C++ Version)
 * Copyright (C) 2022 A. S. Budden
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include <algorithm>
#include <sstream>
#include "strutils.h"

bool startsWith(std::string const &fullString, std::string const &start) {
	if (fullString.length() >= start.length()) {
		return (0 == fullString.compare(0, start.length(), start));
	} else {
		return false;
	}
}

bool endsWith(std::string const &fullString, std::string const &ending) {
	if (fullString.length() >= ending.length()) {
		return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
	} else {
		return false;
	}
}

bool contains(std::string const &haystack, std::string const &needle) {
	return haystack.find(needle) != std::string::npos;
}

std::vector<std::string> split(std::string const &fullString, char splitOn) {
	std::vector<std::string> parts;
	std::string s;
	std::istringstream iss(fullString);
	while (std::getline(iss, s, splitOn)) {
		parts.push_back(s);
	}
	if ((fullString.length() > 0) && (fullString[fullString.length()-1] == splitOn)) {
		parts.push_back("");
	}
	return parts;
}

std::string toUpper(std::string original) {
	std::string output = "";
	std::transform(original.begin(), original.end(), std::back_inserter(output), (int (*)(int)) std::toupper);
	return output;
}

std::string toLower(std::string original) {
	std::string output = "";
	std::transform(original.begin(), original.end(), std::back_inserter(output), (int (*)(int)) std::tolower);
	return output;
}

std::string convertWithBase(intmax_t value, int base)
{
	std::string buf;

	// check that the base if valid
	if (base < 2 || base > 16) {
		return buf;
	}

	enum { kMaxDigits = 100 };
	buf.reserve( kMaxDigits ); // Pre-allocate enough space.

	intmax_t quotient = value;

	// Translating number to string with base:
	do {
		buf += "0123456789abcdef"[ std::abs( quotient % base ) ];
		quotient /= base;
	} while ( quotient );

	// Append the negative sign
	if ( value < 0) {
		buf += '-';
	}

	std::reverse( buf.begin(), buf.end() );
	return buf;
}
