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
#include <iostream>

#include "commands.h"
#include "strutils.h"

void CommandHandler::populateKeyMaps()
{
	std::initializer_list<std::pair<const std::string, KeyMap> > keyMapInit = {
		{"Esc", { .plainCmd = "EXT-Quit" }},
		{"Left", { .plainCmd = "EngL" }},
		{"Up", { .plainCmd = "rollUp", .shiftCmd = "ceiling" }},
		{"Right", { .plainCmd = "EngR" }},
		{"Down", { .plainCmd = "rollDown", .shiftCmd = "floor" }},
		{"0", { .plainCmd = "0", .shiftCmd = "tentox", .ctrlCmd = "log10" }},
		{")", { .plainCmd = "tentox", .shiftCmd = "tentox" }},
		{"1", { .plainCmd = "1" }},
		{"2", { .plainCmd = "2", .shiftCmd = "twotox", .ctrlCmd = "log2", .ctrlShiftCmd = "inversetan2" }},
		{"\"", { .plainCmd = "twotox", .shiftCmd = "twotox" }},
		{"3", { .plainCmd = "3", .shiftCmd = "Convert_Currency_US Dollars_GB Pounds", .ctrlAltCmd = "Convert_Currency_Euros_GB Pounds" }},
		{"\xa3", { .plainCmd = "Convert_Currency_US Dollars_GB Pounds", .shiftCmd = "Convert_Currency_US Dollars_GB Pounds" }},
		{"sterling", { .plainCmd = "Convert_Currency_US Dollars_GB Pounds", .shiftCmd = "Convert_Currency_US Dollars_GB Pounds" }},
		{"4", { .plainCmd = "4", .shiftCmd = "Convert_Currency_GB Pounds_US Dollars", .ctrlAltCmd = "Convert_Currency_GB Pounds_Euros" }},
		{"$", { .plainCmd = "Convert_Currency_GB Pounds_US Dollars", .shiftCmd = "Convert_Currency_GB Pounds_US Dollars" }},
		{"euro", { .plainCmd = "Convert_Currency_GB Pounds_Euros", .ctrlAltCmd = "Convert_Currency_GB Pounds_Euros" }},
		{"5", { .plainCmd = "5", .shiftCmd = "percent", .ctrlShiftCmd = "percentchange" }},
		{"%", { .plainCmd = "percent", .shiftCmd = "percent", .ctrlShiftCmd = "percentchange" }},
		{"6", { .plainCmd = "6", .shiftCmd = "bitwisexor" }},
		{"^", { .plainCmd = "bitwisexor", .shiftCmd = "bitwisexor" }},
		{"7", { .plainCmd = "7", .shiftCmd = "bitwiseand" }},
		{"&", { .plainCmd = "bitwiseand", .shiftCmd = "bitwiseand" }},
		{"8", { .plainCmd = "8", .shiftCmd = "times" }},
		{"*", { .plainCmd = "times", .shiftCmd = "times" }},
		{"9", { .plainCmd = "9" }},
		{"\\", { .shiftCmd = "bitwiseor" }},
		{"|", { .plainCmd = "bitwiseor", .shiftCmd = "bitwiseor" }},
		{"Backspace", { .plainCmd = "backspace" }},
		{".", { .plainCmd = "." }},
		{"`", { .plainCmd = "clear" }}, // Backtick
		{"Enter", { .plainCmd = "enter" }},
		{"Return", { .plainCmd = "enter" }},
		{"Tab", { .plainCmd = "EXT-NextTab" }},
		{"/", { .plainCmd = "divide", .shiftCmd = "integerdivide" }},
		{"-", { .plainCmd = "minus" }},
		{"+", { .plainCmd = "plus", .shiftCmd = "plus" }},
		{"=", { .shiftCmd = "plus" }},
		{"Del", { .plainCmd = "clear" }},
		{"A", { .plainHandler = [](CommandHandler *c) { c->hex_key("a"); }, .shiftHandler = [](CommandHandler *c) { c->shift_hex_key("a"); } }},
		{"B", { .plainHandler = [](CommandHandler *c) { c->hex_key("b"); }, .shiftHandler = [](CommandHandler *c) { c->shift_hex_key("b"); }, .ctrlCmd = "base" }},
		{"C", { .plainHandler = [](CommandHandler *c) { c->hex_key("c"); }, .shiftHandler = [](CommandHandler *c) { c->shift_hex_key("c"); }, .ctrlCmd = "EXT-CopyToClipboard", .altCmd = "cos", .altShiftCmd = "inversecos"  }},
		{"D", { .plainHandler = [](CommandHandler *c) { c->hex_key("d"); }, .shiftHandler = [](CommandHandler *c) { c->shift_hex_key("d"); } }},
		{"E", { .plainHandler = [](CommandHandler *c) { c->hex_key("e"); }, .shiftHandler = [](CommandHandler *c) { c->shift_hex_key("e"); } }},
		{"F", { .plainHandler = [](CommandHandler *c) { c->hex_key("f"); }, .shiftHandler = [](CommandHandler *c) { c->shift_hex_key("f"); } }},
		{"H", { .ctrlShiftCmd = "debugHistory" }},
		{"I", { .plainCmd = "reciprocal", .shiftCmd = "integerpart" }},
		{"K", { .plainCmd = "EXT-ConstByName", .shiftCmd = "EXT-DensityByName" }},
		{"L", { .shiftCmd = "loge" }},
		{"M", { .plainCmd = "Convert_Distance_Inches_Millimetres", .shiftCmd = "Convert_Distance_Millimetres_Inches", .ctrlCmd = "EXT-MoreConversions" }},
		{"N", { .plainCmd = "invert" }},
		{"P", { .plainCmd = "pi" }},
		{"Q", { .plainCmd = "sqrt", .shiftCmd = "square" }},
		{"R", { .plainCmd = "EXT-Recall", .shiftCmd = "random", .ctrlCmd = "round" }},
		{"S", { .plainCmd = "EXT-Store", .shiftCmd = "EXT-SI", .ctrlCmd = "ShowAll", .altCmd = "sin", .altShiftCmd = "inversesin" }},
		{"T", { .shiftCmd = "EXT-NextTab", .altCmd = "tan", .altShiftCmd = "inversetan" }},
		{"U", { .plainCmd = "undo" }},
		{"V", { .ctrlCmd = "EXT-PasteFromClipboard" }},
		{"W", { .plainCmd = "swap" }},
		{"X", { .plainCmd = "xrooty" }},
		{"Y", { .plainCmd = "power" }}
	};
	keyMap = keyMapInit;

	std::initializer_list<std::pair<const std::string, KeyMap> > siKeyMapInit = {
		{"A", { .plainCmd = "SI-Atto" }},
		{"E", { .shiftCmd = "SI-Exa", .ctrlCmd = "SI-Exbi" }},
		{"F", { .plainCmd = "SI-Femto" }},
		{"G", { .plainCmd = "SI-Giga", .shiftCmd = "SI-Giga", .ctrlCmd = "SI-Gibi" }},
		{"K", { .plainCmd = "SI-Kilo", .shiftCmd = "SI-Kilo", .ctrlCmd = "SI-Kibi" }},
		{"M", { .plainCmd = "SI-Milli", .shiftCmd = "SI-Mega", .ctrlCmd = "SI-Mebi", .altCmd = "SI-Micro" }},
		{"N", { .plainCmd = "SI-Nano", .shiftCmd = "SI-Nano" }},
		{"P", { .plainCmd = "SI-Pico", .shiftCmd = "SI-Peta", .ctrlCmd = "SI_Pebi" }},
		{"T", { .shiftCmd = "SI-Tera", .ctrlCmd = "SI_Tebi" }},
		{"U", { .plainCmd = "SI-Micro", .shiftCmd = "SI_Micro" }},
		{"Y", { .plainCmd = "SI-Yocto", .shiftCmd = "SI-Yotta" }},
		{"Z", { .plainCmd = "SI-Zepto", .shiftCmd = "SI-Zetta" }},
	};
	siKeyMap = siKeyMapInit;
}

keyHandlerResult CommandHandler::handleKey(std::string key, std::string modifiers, std::string tab, ErrorCode &ec)
{
	static bool last_store_mode = false;
	ec = NoError;

	if (startsWith(tab, "roman") || startsWith(tab, "greek")) {
		if (key == "Esc") {
			return Key_Cancel;
		}
		else if (((key == "Tab") && (modifiers == "plain"))
				|| ((key == "T") && (modifiers == "shift"))) {
			return Key_NextTab;
		}
		else if (modifiers != "plain") {
			return Key_NotHandled;
		}
		else {
			std::string prefix = "StoreRomanUpper";
			bool is_greek = false;
			if (startsWith(tab, "greek")) {
				prefix = "StoreGreekUpper";
				is_greek = true;
			}
			if (contains(tab, "lower")) {
				prefix = prefix.substr(0, 10) + "Lower";
			}

			if ((key.length() == 1) && (key[0] >= 'A') && (key[0] <= 'Z')) {
				std::string command = key;
				if (is_greek) {
					command = roman_to_greek.at(command);
				}

				if (last_store_mode) {
					store(prefix + command);
				}
				else {
					recall(prefix + command);
				}
				return Key_Cancel;
			}
			return Key_NotHandled;
		}
	}
	else if (tab == "sipad") {
		if (key == "Esc") {
			return Key_Cancel;
		}
		else if (((key == "Tab") && (modifiers == "plain"))
				|| ((key == "T") && (modifiers == "shift"))) {
			return Key_NextTab;
		}
		else {
			if (siKeyMap.contains(key)) {
				return runHandler(modifiers, siKeyMap[key], ec);
			}
			return Key_NotHandled;
		}
	}
	else if (keyMap.contains(key)) {
		keyHandlerResult result = runHandler(modifiers, keyMap[key], ec);
		if (result == Key_Store) {
			last_store_mode = true;
		}
		else if (result == Key_Recall) {
			last_store_mode = false;
		}
		else {
		}
		return result;
	}
	else {
		return Key_NotHandled;
	}
}

std::vector<std::string> CommandHandler::getShortcutKeys(std::string name)
{
	std::vector<std::string> result;
	std::vector<std::pair<int, std::string> > r;
	std::vector<std::map<std::string, KeyMap> > keyMaps = {
		keyMap, siKeyMap
	};

	if (name == "NOP") {
		return result;
	}

	if (name == "store") {
		name = "EXT-Store";
	}
	else if (name == "recall") {
		name = "EXT-Recall";
	}
	else {
	}

	for (auto km : keyMaps) {
		for (const auto & [key, m] : km) {
			if ((m.plainCmd == name) || (startsWith(m.plainCmd, "EXT-") && (m.plainCmd.substr(4) == name))) {
				r.push_back(std::make_pair(key.length(), key));
			}
			if ((m.shiftCmd == name) || (startsWith(m.shiftCmd, "EXT-") && (m.shiftCmd.substr(4) == name))) {
				r.push_back(std::make_pair(key.length()+6, "Shift+"+key));
			}
			if ((m.ctrlCmd == name) || (startsWith(m.ctrlCmd, "EXT-") && (m.ctrlCmd.substr(4) == name))) {
				r.push_back(std::make_pair(key.length()+5, "Ctrl+"+key));
			}
			if ((m.altCmd == name) || (startsWith(m.altCmd, "EXT-") && (m.altCmd.substr(4) == name))) {
				r.push_back(std::make_pair(key.length()+4, "Alt+"+key));
			}
			if ((m.ctrlShiftCmd == name) || (startsWith(m.ctrlShiftCmd, "EXT-") && (m.ctrlShiftCmd.substr(4) == name))) {
				r.push_back(std::make_pair(key.length()+11, "Ctrl+Shift+"+key));
			}
			if ((m.ctrlAltCmd == name) || (startsWith(m.ctrlAltCmd, "EXT-") && (m.ctrlAltCmd.substr(4) == name))) {
				r.push_back(std::make_pair(key.length()+9, "Ctrl+Alt+"+key));
			}
			if ((m.altShiftCmd == name) || (startsWith(m.altShiftCmd, "EXT-") && (m.altShiftCmd.substr(4) == name))) {
				r.push_back(std::make_pair(key.length()+9, "Alt+Shift+"+key));
			}
		}
	}

	if ( ! isHexadecimal()) {
		if (name == "absolute") {
			r.push_back(std::make_pair(1, "A"));
			r.push_back(std::make_pair(7, "Shift+A"));
		}
		else if (name == "base") {
			r.push_back(std::make_pair(7, "Shift+B"));
		}
		else if (name == "clear") {
			r.push_back(std::make_pair(1, "C"));
			r.push_back(std::make_pair(7, "Shift+C"));
		}
		else if (name == "Convert_Angle_Degrees.Minutes-Seconds_Degrees") {
			r.push_back(std::make_pair(1, "D"));
		}
		else if (name == "Convert_Angle_Degrees_Degrees.Minutes-Seconds") {
			r.push_back(std::make_pair(7, "Shift+D"));
		}
		else if (name == "exponent") {
			r.push_back(std::make_pair(1, "E"));
		}
		else if (name == "etox") {
			r.push_back(std::make_pair(7, "Shift+E"));
		}
		else if (name == "floatingpart") {
			r.push_back(std::make_pair(7, "Shift+F"));
		}
		else {
		}
	}

	std::sort(r.begin(), r.end());

	for (auto p : r) {
		result.push_back(p.second);
	}
	return result;
}

keyHandlerResult CommandHandler::runHandler(std::string modifiers, KeyMap map, ErrorCode &ec)
{
	const std::map<std::string, keyHandlerResult> extMapping = {
		{"EXT-Cancel",          Key_Cancel},
		{"EXT-NextTab",         Key_NextTab},
		{"EXT-MoreConversions", Key_MoreConversions},
		{"EXT-ConstByName",     Key_ConstByName},
		{"EXT-DensityByName",   Key_DensityByName},
		{"EXT-Store",           Key_Store},
		{"EXT-Recall",          Key_Recall},
		{"EXT-SI",              Key_SI},
		{"EXT-CopyToClipboard",    Key_CopyToClipboard},
		{"EXT-PasteFromClipboard", Key_PasteFromClipboard},
		{"EXT-Quit",            Key_Quit}
	};
	std::string cmd = "NOP";

	keyHandler handler = NULL;
	if (modifiers == "plain") {
		cmd = map.plainCmd;
		handler = map.plainHandler;
	}
	else if (modifiers == "shift") {
		cmd = map.shiftCmd;
		handler = map.shiftHandler;
	}
	else if (modifiers == "alt") {
		cmd = map.altCmd;
		handler = map.altHandler;
	}
	else if (modifiers == "ctrl") {
		cmd = map.ctrlCmd;
		handler = map.ctrlHandler;
	}
	else if (modifiers == "ctrlShift") {
		cmd = map.ctrlShiftCmd;
		handler = map.ctrlShiftHandler;
	}
	else if (modifiers == "ctrlAlt") {
		cmd = map.ctrlAltCmd;
		handler = map.ctrlAltHandler;
	}
	else {
		return Key_NotHandled;
	}

	if (cmd != "NOP") {
		//std::cout << "Handling command " << cmd << std::endl;
		if (startsWith(cmd, "EXT-")) {
			return extMapping.at(cmd);
		}
		else {
			ec = keypress(cmd);
			return Key_Handled;
		}
	}
	else if (handler != NULL) {
		//std::cout << "Running handler" << std::endl;
		handler(this);
		return Key_Handled;
	}
	else {
		return Key_NotHandled;
	}
}

void CommandHandler::hex_key(std::string key)
{
	// Assume this will only be called with "A" to "F"
	if (isHexadecimal()) {
		keypress(key);
	}
	else {
		if (key == "a") {
			keypress("absolute");
		}
		else if (key == "b") {
			// NOP
		}
		else if (key == "c") {
			keypress("clear");
		}
		else if (key == "d") {
			keypress("Convert_Angle_Degrees.Minutes-Seconds_Degrees");
		}
		else if (key == "e") {
			keypress("exponent");
		}
		else if (key == "f") {
			// NOP
		}
		else {
		}
	}
}

void CommandHandler::shift_hex_key(std::string key)
{
	// Assume this will only be called with "A" to "F"
	if (isHexadecimal()) {
		keypress(key);
	}
	else {
		if (key == "a") {
			keypress("absolute");
		}
		else if (key == "b") {
			keypress("base");
		}
		else if (key == "c") {
			keypress("clear");
		}
		else if (key == "d") {
			keypress("Convert_Angle_Degrees_Degrees.Minutes-Seconds");
		}
		else if (key == "e") {
			keypress("etox");
		}
		else if (key == "f") {
			keypress("floatingpart");
		}
		else {
		}
	}
}

