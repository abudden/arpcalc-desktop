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

#include <sstream>

#include "commands.h"
#include "strutils.h"

CommandHandler::CommandHandler()
{
	dspOptions.decimalPlaces = 7;
	dspOptions.expNegMinDisplay = -3;
	dspOptions.expPosMaxDisplay = 7;

	dspBase = baseDecimal;

	dspState.forcedEngDisplay = false;
	dspState.forcedEngFactor = 0;
	dspState.enteredText = "";
	dspState.enteredValue = AF(0);
	dspState.entering = false;
	dspState.justPressedEnter = false;
	dspState.justPressedBase = false;
	dspState.showAll = false;

	initialiseSIPrefixes();
	populateKeyMaps();
}

bool CommandHandler::varStoreHasChanged()
{
	return varStoreChanged;
}

void CommandHandler::loadVarStore(std::map<std::string, AF> stored)
{
	varStore = stored;
}

void CommandHandler::setOption(CalcOpt o, bool v)
{
	st.setOption(o, v);
}

bool CommandHandler::getOption(CalcOpt o)
{
	return st.getOption(o);
}

void CommandHandler::setOptionByName(std::string o, bool v)
{
	if (DispOptNames.contains(o)) {
		setOption(DispOptNames.at(o), v);
	}
	else {
		std::map<std::string, CalcOpt> coNames = st.getOptionNameMap();

		if (coNames.contains(o)) {
			setOption(coNames.at(o), v);
		}
		else {
			std::cerr << "Invalid option " << o << std::endl;
		}
	}
}

bool CommandHandler::getOptionByName(std::string o)
{
	if (DispOptNames.contains(o)) {
		return getOption(DispOptNames.at(o));
	}
	else {
		std::map<std::string, CalcOpt> coNames = st.getOptionNameMap();

		if (coNames.contains(o)) {
			return getOption(coNames.at(o));
		}
		else {
			std::cerr << "Invalid option " << o << std::endl;
		}
	}
	return false;
}

std::vector<std::string> CommandHandler::getOptionNames()
{
	std::map<std::string, CalcOpt> coNames = st.getOptionNameMap();
	std::vector<std::string> result;

	for (auto & it1 : DispOptNames) {
		std::string name = it1.first;
		result.push_back(name);
	}
	for (auto & it2 : coNames) {
		std::string name = it2.first;
		result.push_back(name);
	}

	return result;
}

void CommandHandler::toggleOption(CalcOpt o)
{
	setOption(o, ! getOption(o));
}

void CommandHandler::setPlaces(int v)
{
	dspOptions.decimalPlaces = v;
}

int CommandHandler::getPlaces()
{
	return dspOptions.decimalPlaces;
}

void CommandHandler::setOption(DispOpt o, bool v)
{
	if (v) {
		dspOptions.bOptions.insert(o);
	}
	else {
		dspOptions.bOptions.erase(o);
	}
}

bool CommandHandler::getOption(DispOpt o)
{
	return dspOptions.bOptions.contains(o);
}

void CommandHandler::toggleOption(DispOpt o)
{
	setOption(o, ! getOption(o));
}

void CommandHandler::startEntering()
{
	if (dspState.justPressedEnter) {
		if ( ! dspState.entering) {
			st.pop();
		}
		dspState.justPressedEnter = false;
		dspState.entering = false;
	}
	if (dspState.entering) {
		if (dspState.enteredText.length() > 0) {
			return;
		}
	}
	dspState.enteredText = "";
	dspState.entering = true;
}

void CommandHandler::completeEntering(bool needValue)
{
	dspState.justPressedEnter = false;
	if ( ! dspState.entering) {
		return;
	}
	if (needValue || (dspState.enteredText.length() > 0)) {
		st.push(dspState.enteredValue);
	}
	dspState.entering = false;
	dspState.enteredText = "";
}

void CommandHandler::updateValueFromText()
{
	if (dspState.enteredText.length() == 0) {
		dspState.enteredValue = AF("0.0");
	}
	else if (isDecimal()) {
		if (getOption(BinaryPrefixes) && (contains(dspState.enteredText, "e"))) {
			std::vector<std::string> parts = split(dspState.enteredText, 'e');
			dspState.enteredValue = AF(parts[0]) * (AF("2.0").pow(AF(parts[1])));
		}
		else {
			dspState.enteredValue = AF(dspState.enteredText);
		}
	}
	else if (dspBase == baseHexadecimal) {
		std::istringstream converter { dspState.enteredText };
		intmax_t value = 0;
		converter >> std::hex >> value;
		dspState.enteredValue = AF(value);
	}
	else if (dspBase == baseBinary) {
		intmax_t value = std::stoll(dspState.enteredText.c_str(), NULL, 2);
		dspState.enteredValue = AF(value);
	}
	else {
		assert(0);
	}
}

void CommandHandler::clearButton()
{
	if (dspState.justPressedEnter) {
		dspState.enteredText = "";
		dspState.enteredValue = AF("0.0");
		dspState.justPressedEnter = false;
	}
	else if (dspState.entering) {
		if ((dspState.enteredText.length() == 0) || (dspState.enteredValue == AF(0.0))) {
			st.saveHistory();
			st.clear();
		}
	}
	else if (st.peek() == AF(0.0)) {
		st.saveHistory();
		st.clear();
	}
	else {
		st.saveHistory();
		st.pop();
	}
	dspState.entering = true;
	dspState.enteredText = "";
	updateValueFromText();

}

void CommandHandler::numInput(std::string value)
{
	startEntering();
	if ((dspBase == baseBinary) && (std::stoi(value) > 1)) {
		return;
	}
	if ((dspBase == baseOctal) && (std::stoi(value) > 7)) {
		return;
	}
	if (endsWith(dspState.enteredText, "e0")) {
		dspState.enteredText.pop_back();
	}
	if ((dspState.enteredText != "") || (value != "0")) {
		dspState.enteredText += value;
	}
	updateValueFromText();
}

void CommandHandler::hexInput(std::string value)
{
	if (dspBase == baseHexadecimal) {
		startEntering();
		if ((dspState.enteredText != "") || (value != "0")) {
			dspState.enteredText += value;
		}
		updateValueFromText();
	}
}

void CommandHandler::dotInput()
{
	if ( ! isDecimal()) {
		return;
	}
	if (contains(dspState.enteredText, "e")) {
		return;
	}
	if (contains(dspState.enteredText, ".")) {
		return;
	}
	startEntering();
	if (dspState.enteredText.length() == 0) {
		dspState.enteredText = "0";
	}
	dspState.enteredText += ".";
	updateValueFromText();
}

void CommandHandler::debugStackPrint()
{
	st.debugStackPrint();
}

void CommandHandler::enter()
{
	st.saveHistory();
	if (dspState.entering) {
		//std::cerr << "Entering; "
		//	<< dspState.enteredValue.toString()
		//	<< " (from " << dspState.enteredText << ")"
		//	<< std::endl;
		st.push(dspState.enteredValue);
		dspState.enteredText = "";
	}
	else {
		//std::cerr << "Duplicating top value ("
		//	<< st.peek().toString() << ")"
		//	<< std::endl;
		st.push(st.peek());
	}
	dspState.justPressedEnter = true;
}

void CommandHandler::exponent()
{
	if ( ! isDecimal()) {
		return;
	}
	if (contains(dspState.enteredText, "e")) {
		return;
	}
	startEntering();
	if (dspState.enteredText.length() == 0) {
		dspState.enteredText = "1";
	}
	dspState.enteredText += "e0";
	updateValueFromText();
}

void CommandHandler::backspace()
{
	if (dspState.entering && ( ! dspState.justPressedEnter)) {
		int l = dspState.enteredText.length();
		if (l == 1) {
			clearButton();
		}
		else {
			if ((l > 1) && (dspState.enteredText[l-2] == 'e')) {
				dspState.enteredText.pop_back();
				dspState.enteredText.pop_back();
			}
			else if ((l > 2) && (dspState.enteredText[l-3] == 'e') && (dspState.enteredText[l-2] == '-')) {
				dspState.enteredText.pop_back();
				dspState.enteredText.pop_back();
				dspState.enteredText.pop_back();
			}
			else {
				dspState.enteredText.pop_back();
			}
			updateValueFromText();
		}
	}
	else {
		clearButton();
	}

}

void CommandHandler::plusMinus()
{
	if (dspState.entering && ( ! dspState.justPressedEnter)) {
		if (dspState.enteredText.length() == 0) {
			// Do nothing
		}
		else if (contains(dspState.enteredText, "e")) {
			std::vector<std::string> parts = split(dspState.enteredText, 'e');
			if (startsWith(parts[1], "-")) {
				dspState.enteredText = parts[0] + "e" + parts[1].substr(1);
			}
			else {
				dspState.enteredText = parts[0] + "e-" + parts[1];
			}
		}
		else {
			if (startsWith(dspState.enteredText, "-")) {
				dspState.enteredText = dspState.enteredText.substr(1);
			}
			else {
				dspState.enteredText = "-" + dspState.enteredText;
			}
		}
		updateValueFromText();
	}
	else {
		completeEntering(true);
		st.saveHistory();
		st.invert();
	}
}

AF CommandHandler::getXValue()
{
	if (dspState.entering && ( ! dspState.justPressedEnter)) {
		return dspState.enteredValue;
	}
	else {
		return st.peek();
	}
}

std::string CommandHandler::getXDisplay()
{
	AF xValue = getXValue();
	std::string formattedX = "";

	if ( ! isDecimal()) {
		formattedX = formatBase(xValue, dspBase, true);
		return formattedX;
	}

	// Must be decimal if we got here
	if (dspState.entering && ( ! dspState.justPressedEnter)) {
		std::string t = dspState.enteredText;
		if (t.length() == 0) {
			formattedX = formatDecimal(AF("0.0"), true);
		}
		else {
			formattedX = formatEnteredText(t);
		}
	}
	else {
		formattedX = formatDecimal(xValue, true);
	}
	return formattedX;
}

std::string CommandHandler::getStackDisplay()
{
	std::vector<AF> sd = st.getStackForDisplay();
	if (dspState.entering) {
		sd.insert(sd.begin(), dspState.enteredValue);
	}
	int l = sd.size();
	std::string result;
	std::vector<std::string> prefixes = { "X", "Y", "Z" };

	if (st.getOption(ReplicateStack)) {
		if (l > 4) {
			l = 4;
		}
		prefixes.push_back("T");
	}

	for (int i=1; i < l; i++) {
		std::string entry;
		if (result != "") {
			result = "<br>" + result;
		}
		if (i < (int) prefixes.size()) {
			entry = prefixes[i] + ": ";
		}
		else {
			entry = "S(";
			entry += std::to_string(i-prefixes.size());
			entry += "): ";
		}
		if (isDecimal()) {
			entry += formatDecimal(sd[i], false);
		}
		else {
			entry += formatBase(sd[i], dspBase);
		}
		//std::cerr << "Stack entry " << i << ": "<< entry << std::endl;
		result = entry + result;
	}
	return result;
}

std::string CommandHandler::getStatusAngularUnits()
{
	if (st.getOption(Radians)) {
		return "RAD";
	}
	else {
		return "DEG";
	}
}

std::string CommandHandler::getStatusBase()
{
	switch (dspBase) {
		default:
		case baseDecimal:
			return "DEC";
		case baseHexadecimal:
			return "HEX";
		case baseBinary:
			return "BIN";
		case baseOctal:
			return "OCT";
	}
}

void CommandHandler::setStatusBase(std::string b)
{
	if ((b == "DEC") || (b == "decimal")) {
		dspBase = baseDecimal;
	}
	else if ((b == "HEX") || (b == "hexadecimal")) {
		dspBase = baseHexadecimal;
	}
	else if ((b == "BIN") || (b == "binary")) {
		dspBase = baseBinary;
	}
	else if ((b == "OCT") || (b == "octal")) {
		dspBase = baseOctal;
	}
	else {
		dspBase = baseDecimal;
	}
}

std::string CommandHandler::getStatusExponent()
{
	if (getOption(BinaryPrefixes)) {
		return "EXP:2";
	}
	else {
		return "EXP:10";
	}
}

std::string CommandHandler::getBaseDisplay()
{
	AF xValue = getXValue();
	int bc;
	switch(getBitCount()) {
		case bc8:  bc = 8;  break;
		case bc16: bc = 16; break;
		case bc32: bc = 32; break;
		default:
		case bc64: bc = 64; break;
	}
	std::string result = "";

	result += "As " + std::to_string(bc) + "-bit integer:<br><br>";
	result += "Dec: " + formatBase(xValue, baseDecimal) + "<br>";
	result += "Hex: " + formatBase(xValue, baseHexadecimal) + "<br>";
	result += "Bin: " + formatBase(xValue, baseBinary) + "<br>";
	result += "Oct: " + formatBase(xValue, baseOctal) + "<br>";
	return result;
}

void CommandHandler::nextBase()
{
	completeEntering(false);
	if ((dspBase != baseDecimal) && getOption(DecimalBaseBias) && ( ! dspState.justPressedBase)) {
		dspBase = baseDecimal;
	}
	else {
		switch (dspBase) {
			case baseDecimal: dspBase = baseHexadecimal; break;
			case baseHexadecimal: dspBase = baseBinary; break;
			case baseBinary: dspBase = baseOctal; break;
			default:
			case baseOctal: dspBase = baseDecimal; break;
		}
	}
	dspState.justPressedBase = true;
}

void CommandHandler::selectBase(DisplayBase base)
{
	completeEntering(false);
	dspBase = base;
	dspState.justPressedBase = true;
}

void CommandHandler::nextWindowSize()
{
	if (sizeName == "Large") {
		sizeName = "Medium";
	}
	else if (sizeName == "Medium") {
		sizeName = "Small";
	}
	else if (sizeName == "Small") {
		sizeName = "Large";
	}
	else {
		sizeName = "Large";
	}
}

std::string CommandHandler::getSizeName()
{
	return sizeName;
}

void CommandHandler::setSizeName(std::string name)
{
	if (toLower(name) == "large") {
		sizeName = "Large";
	}
	else if (toLower(name) == "medium") {
		sizeName = "Medium";
	}
	else if (toLower(name) == "small") {
		sizeName = "Small";
	}
	else {
		sizeName = "Large";
	}
}

void CommandHandler::nextPlaces()
{
	int v = getPlaces();
	v += 1;
	if ((v > 9) || (v < 0)) {
		v = 0;
	}
	setPlaces(v);
}

void CommandHandler::setBitCount(BitCount v)
{
	st.setBitCount(v);
}

BitCount CommandHandler::getBitCount()
{
	return st.getBitCount();
}

void CommandHandler::nextBitCount()
{
	switch (st.getBitCount()) {
		case bc8:  setBitCount(bc16); break;
		case bc16: setBitCount(bc32); break;
		default:
		case bc32: setBitCount(bc64); break;
		case bc64: setBitCount(bc8);  break;
	}
}

std::map<std::string, std::string> CommandHandler::getDisplayContents()
{
	// TODO: doesn't seem to be implemented in kotlin - is this
	// a mistake?
	std::map<std::string, std::string> r;
	return r;
}

void CommandHandler::optionsChanged()
{
	completeEntering(false);
}

std::string CommandHandler::getParseableX()
{
	return getXValue().toString();
}

bool CommandHandler::isDecimal()
{
	if (dspBase == baseDecimal) {
		return true;
	}
	else {
		return false;
	}
}

bool CommandHandler::isHexadecimal()
{
	if (dspBase == baseHexadecimal) {
		return true;
	}
	else {
		return false;
	}
}

void CommandHandler::store(std::string name)
{
	completeEntering(false);
	dspState.showAll = false;
	varStore[name] = getXValue();
	varStoreChanged = true;
}

void CommandHandler::recall(std::string name)
{
	AF v = AF(0);
	dspState.showAll = false;
	completeEntering(false);
	if (varStore.contains(name)) {
		v = varStore[name];
	}
	st.push(v);
}

ErrorCode CommandHandler::keypresses(std::string charKeys)
{
	int l = charKeys.length();
	std::list<std::string> commands;
	for (int i=0;i<l;i++) {
		commands.push_back(charKeys.substr(i, 1));
	}
	return keypresses(commands);
}

ErrorCode CommandHandler::keypresses(std::list<std::string> commands)
{
	ErrorCode ec = NoError;
	std::list<std::string>::iterator it;
	for (it = commands.begin(); it != commands.end(); it++) {
		std::string command = *it;
		if (('0' <= command[0]) && (command[0] <= '9') && (command.length() != 1)) {
			ec = keypresses(command);
		}
		else {
			ec = keypress(command);
		}
		if (ec != NoError) {
			break;
		}
	}
	return ec;
}

ErrorCode CommandHandler::keypress(std::string key)
{
	bool handled = true;
	dspState.showAll = false;
	if (key != "base") {
		dspState.justPressedBase = false;
	}
	if (( key != "EngL") && (key != "EngR")) {
		dspState.forcedEngDisplay = false;
	}

	if (key.length() == 1) {
		switch (key[0]) {
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				numInput(key);
				break;
			case 'A':
			case 'B':
			case 'C':
			case 'D':
			case 'a':
			case 'b':
			case 'c':
			case 'd':
			case 'f':
			case 'F':
				hexInput(toUpper(key));
				break;
			case 'E':
			case 'e':
				if (isDecimal()) {
					exponent();
				}
				else if (dspBase == baseHexadecimal) {
					hexInput("E");
				}
				else {
					// Do nothing
				}
				break;
			case '.':
				dotInput();
				break;
			default:
				handled = false;
		}
	}
	else {
		if (key == "backspace") { backspace(); }
		else if ((key == "plusMinus") || (key == "invert")) { plusMinus(); }
		else if (key == "clear") { clearButton(); }
		else if (key == "enter") { enter(); }
		else if (key == "exponent") { exponent(); }
		else if (key == "display") { std::cout << st.peek().toString(); }
		else if (key == "dot") { dotInput(); }
		else if (key == "EngL") { engRotate(-1); }
		else if (key == "EngR") { engRotate(1); }
		else if (key == "ShowAll") { dspState.showAll = true; }
		else if (key == "base") { nextBase(); }
		else if (key == "debugHistory") { st.printHistory(); }
		else { handled = false; }
	}

	if (handled) {
		return NoError;
	}

	//std::cerr << "Processing " << key << std::endl;

	if (startsWith(key, "Const-")) {
		completeEntering(false);
		st.saveHistory();
		return st.constant(key.substr(6));
	}
	if (startsWith(key, "Density-")) {
		completeEntering(false);
		st.saveHistory();
		return st.density(key.substr(8));
	}
	if (startsWith(key, "SI-")) {
		completeEntering(true);
		st.saveHistory();
		return SI(key.substr(3));
	}
	if (startsWith(key, "Convert_")) {
		completeEntering(true);
		st.saveHistory();
		std::vector<std::string> parts = split(key, '_');
		return st.convert(parts[1], parts[2], parts[3]);
	}

	bool takesValue = true;
	std::set<std::string> noValueKeys = {"p", "pi", "random", "rUp", "rDown", "undo", "u" };

	if (noValueKeys.contains(key)) {
		takesValue = false;
	}

	//std::cerr << key << " takes value: " << takesValue << std::endl;

	ErrorCode ec;

	// This is a much more clunky implementation than the kotlin original
	// There's probably a cleaner way with C++, but this'll do for now.
	if (key == "absolute") {
		completeEntering(takesValue); ec = st.absolute(); st.saveHistory();
	}
	else if (key == "bitwiseand") {
		completeEntering(takesValue); ec = st.bitwiseand(); st.saveHistory();
	}
	else if (key == "bitwisenot") {
		completeEntering(takesValue); ec = st.bitwisenot(); st.saveHistory();
	}
	else if (key == "bitwiseor") {
		completeEntering(takesValue); ec = st.bitwiseor(); st.saveHistory();
	}
	else if (key == "bitwisexor") {
		completeEntering(takesValue); ec = st.bitwisexor(); st.saveHistory();
	}
	else if (key == "ceiling") {
		completeEntering(takesValue); ec = st.ceiling(); st.saveHistory();
	}
	else if (key == "pi") {
		std::cerr << "pi" << std::endl;
		completeEntering(false); return st.constant("Pi");
	}
	else if (key == "cos") {
		completeEntering(takesValue); ec = st.cos(); st.saveHistory();
	}
	else if (key == "cosh") {
		completeEntering(takesValue); ec = st.cosh(); st.saveHistory();
	}
	else if (key == "cube") {
		completeEntering(takesValue); ec = st.cube(); st.saveHistory();
	}
	else if (key == "cuberoot") {
		completeEntering(takesValue); ec = st.cuberoot(); st.saveHistory();
	}
	else if ((key == "/") || (key == "divide")) {
		completeEntering(takesValue); ec = st.divide(); st.saveHistory();
	}
	else if (key == "drop") {
		completeEntering(takesValue); ec = st.drop(); st.saveHistory();
	}
	else if (key == "etox") {
		completeEntering(takesValue); ec = st.etox(); st.saveHistory();
	}
	else if (key == "floatingpart") {
		completeEntering(takesValue); ec = st.floatingpart(); st.saveHistory();
	}
	else if (key == "floor") {
		completeEntering(takesValue); ec = st.floor(); st.saveHistory();
	}
	else if (key == "integerdivide") {
		completeEntering(takesValue); ec = st.integerdivide(); st.saveHistory();
	}
	else if (key == "integerpart") {
		completeEntering(takesValue); ec = st.integerpart(); st.saveHistory();
	}
	else if (key == "inversecos") {
		completeEntering(takesValue); ec = st.inversecos(); st.saveHistory();
	}
	else if (key == "inversesin") {
		completeEntering(takesValue); ec = st.inversesin(); st.saveHistory();
	}
	else if (key == "inversetan") {
		completeEntering(takesValue); ec = st.inversetan(); st.saveHistory();
	}
	else if (key == "inversetan2") {
		completeEntering(takesValue); ec = st.inversetan2(); st.saveHistory();
	}
	else if (key == "inversecosh") {
		completeEntering(takesValue); ec = st.inversecosh(); st.saveHistory();
	}
	else if (key == "inversesinh") {
		completeEntering(takesValue); ec = st.inversesinh(); st.saveHistory();
	}
	else if (key == "inversetanh") {
		completeEntering(takesValue); ec = st.inversetanh(); st.saveHistory();
	}
	else if (key == "inversetanh2") {
		completeEntering(takesValue); ec = st.inversetanh2(); st.saveHistory();
	}
	else if (key == "log10") {
		completeEntering(takesValue); ec = st.log10(); st.saveHistory();
	}
	else if (key == "log2") {
		completeEntering(takesValue); ec = st.log2(); st.saveHistory();
	}
	else if ((key == "loge") || (key == "ln")) {
		completeEntering(takesValue); ec = st.loge(); st.saveHistory();
	}
	else if ((key == "-") || (key == "minus")) {
		if (dspState.entering && (endsWith(dspState.enteredText, "e0"))) {
			plusMinus();
			return NoError;
		}
		else {
			completeEntering(takesValue); ec = st.minus(); st.saveHistory();
		}
	}
	else if ((key == "%") || (key == "percent")) {
		completeEntering(takesValue); ec = st.percent(); st.saveHistory();
	}
	else if (key == "percentchange") {
		completeEntering(takesValue); ec = st.percentchange(); st.saveHistory();
	}
	else if ((key == "+") || (key == "plus")) {
		completeEntering(takesValue); ec = st.plus(); st.saveHistory();
	}
	else if ((key == "^") || (key == "power")) {
		completeEntering(takesValue); ec = st.power(); st.saveHistory();
	}
	else if (key == "random") {
		completeEntering(takesValue); ec = st.random(); st.saveHistory();
	}
	else if (key == "reciprocal") {
		completeEntering(takesValue); ec = st.reciprocal(); st.saveHistory();
	}
	else if (key == "remainder") {
		completeEntering(takesValue); ec = st.remainder(); st.saveHistory();
	}
	else if (key == "rollDown") {
		completeEntering(takesValue); ec = st.rollDown(); st.saveHistory();
	}
	else if (key == "rollUp") {
		completeEntering(takesValue); ec = st.rollUp(); st.saveHistory();
	}
	else if (key == "round") {
		completeEntering(takesValue); ec = st.round(); st.saveHistory();
	}
	else if (key == "sin") {
		completeEntering(takesValue); ec = st.sin(); st.saveHistory();
	}
	else if (key == "sinh") {
		completeEntering(takesValue); ec = st.sinh(); st.saveHistory();
	}
	else if (key == "square") {
		completeEntering(takesValue); ec = st.square(); st.saveHistory();
	}
	else if ((key == "r") || (key == "sqrt") || (key == "squareroot")) {
		completeEntering(takesValue); ec = st.squareroot(); st.saveHistory();
	}
	else if ((key == "w") || (key == "swap")) {
		completeEntering(takesValue); ec = st.swap(); st.saveHistory();
	}
	else if (key == "tan") {
		completeEntering(takesValue); ec = st.tan(); st.saveHistory();
	}
	else if (key == "tanh") {
		completeEntering(takesValue); ec = st.tanh(); st.saveHistory();
	}
	else if (key == "tentox") {
		completeEntering(takesValue); ec = st.tentox(); st.saveHistory();
	}
	else if (key == "twotox") {
		completeEntering(takesValue); ec = st.twotox(); st.saveHistory();
	}
	else if ((key == "*") || (key == "times")) {
		completeEntering(takesValue); ec = st.times(); st.saveHistory();
	}
	else if ((key == "u") || (key =="undo")) {
		completeEntering(takesValue); ec = st.undo();
	}
	else if (key == "xrooty") {
		completeEntering(takesValue); ec = st.xrooty(); st.saveHistory();
	}
	else {
		std::cerr << "No function" << std::endl;
		ec = NoFunction;
	}
	//std::cerr << "Result: " << ec << std::endl;
	return ec;
}

std::string CommandHandler::addPeriodic(std::string source, int spacing, std::string insertion)
{
	std::string newS;

	int takeLast = source.length() - spacing;
	if (takeLast > 0) {
		newS = source.substr(source.length() - spacing);
	}
	else {
		newS = source;
	}

	std::string old = source;
	int i;

	for (i=0;i<spacing;i++) {
		if (old.length() > 0) {
			old.pop_back();
		}
	}
	while (old.length() > 0) {
		int start_idx = old.length() - spacing;
		if (start_idx < 0) {
			start_idx = 0;
		}
		newS = old.substr(start_idx) + insertion + newS;
		for (i=0;i<spacing;i++) {
			if (old.length() == 0) {
				break;
			}
			old.pop_back();
		}
	}
	return newS;
}

std::string CommandHandler::addThinSpaces(std::string source, int spacing)
{
	return addPeriodic(source, spacing, "&thinsp;");
}

std::string CommandHandler::addThousandsSeparator(std::string source, int spacing)
{
	std::string prefix = "";
	std::string suffix = "";
	std::string modify = source;
	std::string separator = ",";

	if (getOption(SpaceAsThousandsSeparator)) {
		separator = "&nbsp;";
	}
	if (startsWith(source, "-")) {
		prefix = "-";
		modify = source.substr(1);
	}
	if (contains(modify, ".")) {
		std::vector<std::string> parts = split(modify, '.');
		suffix = "." + parts[1];
		modify = parts[0];
	}
	else if (contains(modify, "e")) {
		std::vector<std::string> parts = split(modify, 'e');
		suffix = "e" + parts[1];
		modify = parts[0];
	}
	else {
	}

	std::string result = prefix;
	result += addPeriodic(modify, spacing, separator);
	result += suffix;
	return result;
}

std::string CommandHandler::europeanDecimal(std::string source)
{
	std::regex reDot("\\.");
	std::regex reComma(",");
	std::regex rePlaceholder("__DECIMAL__");
	std::string modified = std::regex_replace(source, reDot, "__DECIMAL__");
	modified = std::regex_replace(modified, reComma, ".");
	modified = std::regex_replace(modified, rePlaceholder, ",");
	return modified;
}

std::string CommandHandler::formatBase(AF value, DisplayBase base, bool isX)
{
	int bn = 10;
	std::string prefix = "";

	switch(base) {
		default:
		case baseDecimal:
			bn = 10;
			prefix = "";
			break;
		case baseHexadecimal:
			bn = 16;
			prefix = "0x";
			break;
		case baseBinary:
			bn = 2;
			prefix = "0b";
			break;
		case baseOctal:
			bn = 8;
			prefix = "0o";
			break;
	}

	std::string formatted;
	intmax_t asLong = value.toLong();
	intmax_t mask = st.getBitMask();

	if ((asLong >= 0) || (base == baseDecimal)) {
		if ( ! isX) {
			asLong = asLong & mask;
		}
		formatted = toUpper(convertWithBase(asLong, bn));
	}
	else {
		intmax_t absolute = asLong;
		if (absolute < 0) {
			absolute = - absolute;
		}
		intmax_t twoscomp = (~ absolute) + 1;
		if ( ! isX) {
			twoscomp = twoscomp & mask;
		}
		formatted = toUpper(convertWithBase(twoscomp, bn));
	}
	if (base == baseBinary) {
		while ((formatted.length() % 4) != 0) {
			formatted = "0" + formatted;
		}
	}
	if (base != baseDecimal) {
		formatted = addThinSpaces(formatted);
	}
	return prefix + formatted;
}

ValueAndExp CommandHandler::getValueAndExponent(AF value, bool isX)
{
	ValueAndExp result = {AF("0.0"), 0};
	intmax_t exponent = 0;
	AF absolute = value.abs();
	AF realnumber = value;
	int places = dspOptions.decimalPlaces;

	if ((absolute != 0.0) && (getOption(BinaryPrefixes))) {
		double binExpNegMinDisplay = 
			floor(log10(pow(10.0, ((double) dspOptions.expNegMinDisplay)))/log10(2));
		double binExpPosMaxDisplay =
			floor(log10(pow(10.0, ((double) dspOptions.expPosMaxDisplay)))/log10(2));
		if ((isX && dspState.forcedEngDisplay)
				|| ((absolute < pow(2.0, binExpNegMinDisplay))
					|| (absolute >= pow(2.0, binExpPosMaxDisplay)))) {
			if (isX && dspState.forcedEngDisplay) {
				exponent = dspState.forcedEngFactor;
			}
			else {
				AF exp = absolute.log() / AF("2").log();
				exp = exp.floor();
				exp = exp.round();
				exp = exp / 10.0;
				exp = exp.floor();
				exp = exp.round();
				exp = exp * 10.0;
				exponent = exp.toLong();
			}
			realnumber = realnumber / AF("2").pow(AF(exponent));

			// Check whether rounding bumps exponent
			if ( ! dspState.showAll) {
				realnumber = RoundToDecimalPlaces(realnumber, places);
			}
			absolute = realnumber.abs();

			if (getOption(EngNotation) && ( ! dspState.forcedEngDisplay)
					&& (absolute >= 1024.0)) {
				realnumber = realnumber / 1024.0;
				exponent += 10;
			}
			else if (( ! ( getOption(EngNotation) || dspState.forcedEngDisplay))
					&& (absolute >= 2.0)) {
				realnumber = realnumber / 2.0;
				exponent += 1;
			}
			else {
				// No rounding needed
			}
		}
	}
	else if (absolute != AF("0")) { // but not binary prefixes
		if ((isX && dspState.forcedEngDisplay)
				|| ((absolute < AF(10.0).pow(AF(dspOptions.expNegMinDisplay)))
					|| ((absolute >= AF(10.0).pow(AF(dspOptions.expPosMaxDisplay)))))) {
			if (isX && dspState.forcedEngDisplay) {
				exponent = dspState.forcedEngFactor;
			}
			else {
				exponent = absolute.log10().floor().round().toLong();
				if ((getOption(EngNotation)) || (dspState.forcedEngDisplay)) {
					exponent = 3 * AF(AF(exponent) / 3.0).floor().round().toLong();
				}
			}
			realnumber = realnumber / AF(10.0).pow(exponent);

			// Check whether rounding bumps exponent
			if (( ! dspState.showAll) && ( ! dspState.forcedEngDisplay)) {
				realnumber = RoundToDecimalPlaces(realnumber, places);
			}
			absolute = realnumber.abs();

			if (getOption(EngNotation) && ( ! dspState.forcedEngDisplay)
					&& (absolute >= 1000.0)) {
				realnumber = realnumber / 1000.0;
				exponent += 3;
			}
			else if (( ! (getOption(EngNotation) || dspState.forcedEngDisplay))
					&& (absolute >= 10.0)) {
				realnumber = realnumber / 10.0;
				exponent += 1;
			}
			else {
				// No rounding needed
			}
		}
	}
	else {
	}
	result.value = realnumber;
	result.exponent = int(exponent);
	return result;
}

std::string CommandHandler::formatEnteredText(std::string value)
{
	std::string formatted = value;
	if (getOption(ThousandsSeparator)) {
		formatted = addThousandsSeparator(formatted);
	}
	if (getOption(EuropeanDecimal)) {
		formatted = europeanDecimal(formatted);
	}
	formatted = powerExponent(formatted, true);
	std::regex re("-");
	formatted = std::regex_replace(formatted, re, "&ndash;");
	return formatted;
}

std::string CommandHandler::formatDecimal(AF value, bool isX, bool constHelpMode)
{
	ValueAndExp v = getValueAndExponent(value, isX);
	AF realnumber = v.value;
	int exponent = v.exponent;

	int places = dspOptions.decimalPlaces;
	char buffer[100] = {0};

	std::string formatted;

	if (dspState.showAll) {
		snprintf(buffer, sizeof(buffer), "%.30f", realnumber.toDouble());
		formatted = buffer;
	}
	else if (constHelpMode) {
		snprintf(buffer, sizeof(buffer), "%.10f", realnumber.toDouble());
		formatted = buffer;
	}
	else if ((realnumber != AF("0")) && dspState.forcedEngDisplay) {
		snprintf(buffer, sizeof(buffer), "%d", places);
		
		std::string fmt = "%.";
		fmt += buffer;
		fmt += "f";
		snprintf(buffer, sizeof(buffer), fmt.c_str(), realnumber.toDouble());
		formatted = buffer;

		if (contains(formatted, "e")) {
			std::vector<std::string> parts = split(formatted, 'e');
			std::vector<std::string> numparts;

			if (contains(parts[0], ".")) {
				numparts = split(parts[0], '.');
			}
			else {
				numparts.push_back(parts[0]);
				numparts.push_back("0");
			}

			int sc_exponent = std::stoi(parts[1]);
			std::string newformatted;

			if (sc_exponent < 0) {
				newformatted = "0.";

				int major_length = abs(sc_exponent) - numparts[0].length();
				if (major_length < (int) numparts[0].length()) {
					/* TODO: problem! */
				}
				else if (major_length == (int) numparts[0].length()) {
					newformatted += numparts[0];
				}
				else {
					std::string padStr = "";
					while (padStr.length() < (major_length - numparts[0].length())) {
						padStr += "0";
					}
					newformatted += padStr + numparts[0];
				}
				newformatted += numparts[1];
				formatted = newformatted;
			}
			else {
				newformatted = numparts[0] + numparts[1];
				int total_length = sc_exponent + numparts[0].length();
				if (total_length < (int) newformatted.length()) {
					/* TODO: problem */
				}
				else if (total_length > (int) newformatted.length()) {
					std::string padStr = "";
					while (padStr.length() < (newformatted.length() - total_length)) {
						padStr += "0";
					}
					newformatted + padStr;
				}
				else {
					/* NOP */
				}
				formatted = newformatted;
			}
		}

		if (contains(formatted, ".")) {
			while (endsWith(formatted, "0")) {
				formatted.pop_back();
			}
			if (endsWith(formatted, ".")) {
				formatted += "0";
			}
		}
	}
	else {
		snprintf(buffer, sizeof(buffer), "%d", places);
		std::string fmt = "%.";
		fmt += buffer;
		fmt += "f";
		//std::cerr << "Formatting with " << fmt;
		snprintf(buffer, sizeof(buffer), fmt.c_str(), realnumber.toDouble());
		formatted = buffer;
		//std::cerr << " result: " << formatted << std::endl;
	}

	if (getOption(ThousandsSeparator)) {
		formatted = addThousandsSeparator(formatted);
	}

	if (getOption(TrimZeroes) || dspState.showAll || constHelpMode) {
		if (contains(formatted, ".")) {
			while (endsWith(formatted, "0")) {
				formatted.pop_back();
			}
			if (endsWith(formatted, ".")) {
				if (getOption(AlwaysShowDecimal) || constHelpMode) {
					formatted += "0";
				}
				else {
					formatted.pop_back();
				}
			}
		}
	}
	if (getOption(EuropeanDecimal)) {
		formatted = europeanDecimal(formatted);
	}

	bool trim_exponents = true;

	if ((exponent != 0) || (isX && (contains(dspState.enteredText, "e")))) {
		int exponent_digits = 4;
		if (trim_exponents) {
			exponent_digits = 1;
		}
		snprintf(buffer, sizeof(buffer), "%d", exponent_digits);
		std::string fmt = "%0";
		fmt += buffer;
		fmt += "d";
		snprintf(buffer, sizeof(buffer), fmt.c_str(), exponent);
		std::string exponentStr = buffer;
		while (startsWith(exponentStr, "0") && (exponentStr.length() > 1)) {
			exponentStr.pop_back();
		}
		formatted += "e";
		formatted += exponentStr;
		formatted = powerExponent(formatted, isX);
	}
	std::regex redash("-");
	formatted = std::regex_replace(formatted, redash, "&ndash;");
	return formatted;
}

std::string CommandHandler::powerExponent(std::string preformatted, bool isX)
{
	std::vector<std::string> parts = split(toLower(preformatted), 'e');

	if (parts.size() > 1) {
		std::string exponentPart = parts[1];
		std::string coefficientPart = parts[0];
		std::string exponentSign = "";

		if (startsWith(exponentPart, "+")) {
			exponentPart = exponentPart.substr(1);
		}
		else if (startsWith(exponentPart, "-")) {
			exponentSign = "-";
			exponentPart = exponentPart.substr(1);
		}
		else {
		}

		while (startsWith(exponentPart, "0") && (exponentPart.length() > 1)) {
			exponentPart = exponentPart.substr(1);
		}

		int expValue = std::stoi(parts[1]);
		if ((expValue == 0) && isX && (( ! dspState.entering) || (dspState.justPressedEnter))) {
			return coefficientPart;
		}
		else if (getOption(BinaryPrefixes)) {
			if (getOption(SINotation)) {
				for (std::map<std::string, int>::iterator it = SIBinaryPrefixes.begin(); it != SIBinaryPrefixes.end(); it++) {
					std::string prefix = it->first;
					int eV = it->second;

					if (eV == expValue) {
						return coefficientPart + "&nbsp;" + SISymbols[prefix];
					}
				}
			}

			/* Enforce power exponent view if binary prefixes are used to avoid
			 * confusion as to what E means
			 */
			return coefficientPart + "&times;2<sup><small>" + exponentSign + exponentPart + "</small></sup>";
		}
		else if (getOption(SINotation)) {
			for (std::map<std::string, int>::iterator it = SIDecimalPrefixes.begin(); it != SIDecimalPrefixes.end(); it++) {
				std::string prefix = it->first;
				int eV = it->second;
				if (eV == expValue) {
					return coefficientPart + "&nbsp;" + SISymbols[prefix];
				}
			}
		}
		else {
			/* Handled below */
		}

		if (getOption(PowerExponentView)) {
			return coefficientPart + "&times;10<sup><small>" + exponentSign + exponentPart + "</small></sup>";
		}
	}
	return preformatted;
}

void CommandHandler::engRotate(int direction)
{
	int multiplier = 3;
	completeEntering(true);
	if (getOption(BinaryPrefixes)) {
		multiplier = 10;
	}

	if (dspState.forcedEngDisplay) {
		dspState.forcedEngFactor += multiplier*direction;
	}
	else {
		/* Temporarily set use eng notation */
		bool before = getOption(EngNotation);
		setOption(EngNotation, true);
		ValueAndExp v = getValueAndExponent(st.peek(), true);
		setOption(EngNotation, before);

		int exponent = v.exponent;

		// Ensure it's a multiple of multiplier
		exponent = multiplier *
			(AF(exponent) / AF(multiplier)).floor().round().toInt();

		dspState.forcedEngDisplay = true;
		dspState.forcedEngFactor = long(exponent + (multiplier*direction));
	}
}

AF CommandHandler::RoundToDecimalPlaces(AF d, int c)
{
	AF ten = AF(10);
	AF temp = d * ten.pow(AF(c));
	temp = temp.round();

	return temp / ten.pow(AF(c));
}

std::string CommandHandler::processCurrencyData(std::map<std::string, double> wrtEuro, std::string date)
{
	st.registerCurrencies(wrtEuro);
	last_currency_date = date;

	const int MAX_CURRENCY_AGE_DAYS = 5;

	// TODO: check date - need to do this even if nothing has been received!
	// TODO: Also need to consider that the date isn't currently parsed by javascript!
	return "";
}

std::map<std::string, double> CommandHandler::getRawCurrencyData()
{
	return st.getRawCurrencyData();
}

void CommandHandler::setDefaultOptions()
{
	setOption(EngNotation, true);
	setOption(BinaryPrefixes, false);
	setOption(PowerExponentView, true);
	setOption(SINotation, false);
	setOption(TrimZeroes, true);
	setOption(AlwaysShowDecimal, true);
	setOption(ThousandsSeparator, true);
	setOption(SpaceAsThousandsSeparator, false);
	setOption(SaveStackOnExit, false);
	setOption(SaveBaseOnExit, false);
	setOption(DecimalBaseBias, true);
	setOption(CheckForNewVersions, true);
	setOption(AmericanUnits, false);
	setOption(EuropeanDecimal, false);
	setOption(ShowHelpOnStart, true);

	setSizeName("Medium");

	setPlaces(7);

	setOption(ReplicateStack, false);
	setOption(Radians, false);
	setOption(SaveHistory, true);
	setOption(PercentLeavesY, true);

	setStatusBase("DEC");

	setBitCount(bc32);
}
