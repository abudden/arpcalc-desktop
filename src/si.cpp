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
#include <assert.h>
#include <algorithm>
#include <iostream>
#include <iomanip> // std::setw
#include <cstdlib>
#include <cmath>
#include <regex>

#include "commands.h"
#include "strutils.h"

void CommandHandler::initialiseSIPrefixes()
{
	std::initializer_list<std::pair<const std::string, int> > binInit = {
		{"Kibi", 10},
		{"Mebi", 20},
		{"Gibi", 30},
		{"Tebi", 40},
		{"Pebi", 50},
		{"Exbi", 60}
	};
	SIBinaryPrefixes = binInit;

	std::initializer_list<std::pair<const std::string, int> > decInit = {
		{"Yocto", -24},
		{"Zepto", -21},
		{"Atto", -18},
		{"Femto", -15},
		{"Pico", -12},
		{"Nano", -9},
		{"Micro", -6},
		{"Milli", -3},
		{"Kilo", 3},
		{"Mega", 6},
		{"Giga", 9},
		{"Tera", 12},
		{"Peta", 15},
		{"Exa", 18},
		{"Zetta", 21},
		{"Yotta", 24}
	};
	SIDecimalPrefixes = decInit;

	std::initializer_list<std::pair<const std::string, std::string> > symInit = {
		{"Kibi", "Ki"},
		{"Mebi", "Mi"},
		{"Gibi", "Gi"},
		{"Tebi", "Ti"},
		{"Pebi", "Pi"},
		{"Exbi", "Ei"},
		{"Yocto", "y"},
		{"Zepto", "z"},
		{"Atto", "a"},
		{"Femto", "f"},
		{"Pico", "p"},
		{"Nano", "n"},
		{"Micro", "&mu;"},
		{"Milli", "m"},
		{"Kilo", "k"},
		{"Mega", "M"},
		{"Giga", "G"},
		{"Tera", "T"},
		{"Peta", "P"},
		{"Exa", "E"},
		{"Zetta", "Z"},
		{"Yotta", "Y"}
	};
	SISymbols = symInit;
}

ErrorCode CommandHandler::SI(std::string name)
{
	AF mult = 0;
	AF power;
	if (SIDecimalPrefixes.contains(name)) {
		power = SIDecimalPrefixes[name];
		mult = AF(10.0).pow(power);
	}
	else if (SIBinaryPrefixes.contains(name)) {
		power = SIBinaryPrefixes[name];
		mult = AF(2.0).pow(power);
	}
	else {
		bool found = false;
		for (const auto& [long_name, short_name]: SISymbols) {
			if (short_name == name) {
				if (SIDecimalPrefixes.contains(long_name)) {
					power = SIDecimalPrefixes[long_name];
					mult = AF(10.0).pow(power);
				}
				else if (SIBinaryPrefixes.contains(long_name)) {
					power = SIBinaryPrefixes[long_name];
					mult = AF(2.0).pow(power);
				}
				else {
				}
				found = true;
				break;
			}
		}

		if (! found) {
			return UnknownSI;
		}
	}

	st.push(st.pop() * mult);
	return NoError;
}

std::vector<std::string> CommandHandler::getBinaryPrefixSymbols()
{
	std::vector<std::string> result;
	for (const auto& [long_name, short_name]: SISymbols) {
		if (SIBinaryPrefixes.contains(long_name)) {
			result.push_back(short_name);
		}
	}
	return result;
}

std::vector<std::string> CommandHandler::getDecimalPrefixSymbols()
{
	std::vector<std::string> result;
	for (const auto& [long_name, short_name]: SISymbols) {
		if (SIDecimalPrefixes.contains(long_name)) {
			result.push_back(short_name);
		}
	}
	return result;
}

std::vector<std::string> CommandHandler::getBinaryPrefixNames()
{
	std::vector<std::string> result;
	for (const auto& [long_name, short_name]: SISymbols) {
		(void) short_name;
		if (SIBinaryPrefixes.contains(long_name)) {
			result.push_back(long_name);
		}
	}
	return result;
}

std::vector<std::string> CommandHandler::getDecimalPrefixNames()
{
	std::vector<std::string> result;
	for (const auto& [long_name, short_name]: SISymbols) {
		(void) short_name;
		if (SIDecimalPrefixes.contains(long_name)) {
			result.push_back(long_name);
		}
	}
	return result;
}
