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
#ifndef STRUTILS_H
#define STRUTILS_H

#include <string>
#include <vector>

bool startsWith(std::string const &fullString, std::string const &start);
bool endsWith(std::string const &fullString, std::string const &ending);
bool contains(std::string const &haystack, std::string const &needle);
std::vector<std::string> split(std::string const &fullString, char splitOn);
std::string toUpper(std::string original);
std::string toLower(std::string original);
std::string convertWithBase(intmax_t value, int base);

#endif
