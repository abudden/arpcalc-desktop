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

std::string CommandHandler::getCheckIcon(bool v)
{
	if (v) {
		return "checked";
	}
	else {
		return "unchecked";
	}
}

std::string CommandHandler::getPlacesIcon()
{
	int v = getPlaces();
	switch (v) {
		default:
		case 0: return "num_0";
		case 1: return "num_1";
		case 2: return "num_2";
		case 3: return "num_3";
		case 4: return "num_4";
		case 5: return "num_5";
		case 6: return "num_6";
		case 7: return "num_7";
		case 8: return "num_8";
		case 9: return "num_9";
	}
}

std::string CommandHandler::getBitCountIcon()
{
	BitCount bc = getBitCount();
	switch (bc) {
		case bc8: return "num_8";
		case bc16: return "num_16";
		default:
		case bc32: return "num_32";
		case bc64: return "num_64";
	}
}

std::string CommandHandler::getBaseIcon()
{
	switch (dspBase) {
		default:
		case baseDecimal: return "decimal";
		case baseHexadecimal: return "hexadecimal";
		case baseBinary: return "binary";
		case baseOctal: return "octal";
	}
}

std::string CommandHandler::getWindowSizeIcon()
{
	return toLower(sizeName);
}

std::map<std::string, Option> CommandHandler::getOptions()
{
	// When calling the lambdas, need to pass the command
	// handler instance as a parameter
	std::initializer_list<std::pair<const std::string, Option> > optMapInit = {
		{"Use Engineering Exponents", {
			.helpText = "When showing exponents, always use multiples of 3 to correspond with SI prefixes (milli-, kilo- etc).\n"
			"When using binary prefixes, always use multiples of 10 to correspond with SI binary prefixes (Kibi-, Mebi- etc).",
			.clickAction = [](CommandHandler *c) { c->toggleOption(EngNotation); },
			.iconAction = [](CommandHandler *c) -> std::string { return c->getCheckIcon(c->getOption(EngNotation)); },
			.location = {0, 0, 0} // OptLoc
		}},
		{"Use SI Exponents", {
			.helpText = "When showing exponents, use the SI prefixes (k, M, G etc) instead of showing powers of 10 (for powers that match SI prefixes.",
			.clickAction = [](CommandHandler *c) { c->toggleOption(SINotation); },
			.iconAction = [](CommandHandler *c) -> std::string { return c->getCheckIcon(c->getOption(SINotation)); },
			.location = {0, 1, 0}
		}},
		{"Show Exponents as Powers", {
			.helpText = "When showing exponents, show them as powers (e.g. 4.0&times;10<sup><small>3</small></sup>) rather than exponents\n"
			"(e.g. 4.0e3).  This option is set automatically if using binary prefixes to avoid confusion as to the meaning of 'e'.",
			.clickAction = [](CommandHandler *c) { c->toggleOption(PowerExponentView); },
			.iconAction = [](CommandHandler *c) -> std::string { return c->getCheckIcon(c->getOption(PowerExponentView)); },
			.location = {0, 2, 0}
		}},
		{"Use Binary Exponents", {
			.helpText = "When showing exponents, use powers of 2 instead of powers of 10.",
			.clickAction = [](CommandHandler *c) { c->toggleOption(BinaryPrefixes); },
			.iconAction = [](CommandHandler *c) -> std::string { return c->getCheckIcon(c->getOption(BinaryPrefixes)); },
			.location = {0, 3, 0}
		}},
		{"Degrees or Radians", {
			.helpText = "Angle type to use when performing trigonometric functions.",
			.clickAction = [](CommandHandler *c) { c->toggleOption(Radians); },
			.iconAction = [](CommandHandler *c) -> std::string { if (c->getOption(Radians)) { return "radians"; } else { return "degrees"; } },
			.location = {0, 4, 0}
		}},
		// TODO: Sort out the hide options for more granularity.
#if 0
		{"Show Help on Start", {
			.helpText = "Show a \"toast\" notification on start-up explaining that a long-press on any key gives help.",
			.clickAction = [](CommandHandler *c) { c->toggleOption(ShowHelpOnStart); },
			.iconAction = [](CommandHandler *c) -> std::string { return c->getCheckIcon(c->getOption(ShowHelpOnStart)); },
			.location = {0, 5, 0},
			.hideOnPC = true
		}},
#endif
		{"Window Size", {
			.helpText = "Switch the size of the PC UI - note that this option requires a restart to take effect.",
			.clickAction = [](CommandHandler *c) { c->nextWindowSize(); },
			.iconAction = [](CommandHandler *c) -> std::string { return c->getWindowSizeIcon(); },
			.location = {0, 5, 0},
			.hideOnPhone = true,
			.hideOnWeb = true,
			.requiresRestart = true
		}},
		{"Decimal Places to Show", {
			.helpText = "Number of decimal places to show.  See also \"Trim Zeroes after Decimal\" and \"Always Show Decimal\" options",
			.clickAction = [](CommandHandler *c) { c->nextPlaces(); },
			.iconAction = [](CommandHandler *c) -> std::string { return c->getPlacesIcon(); },
			.location = {0, 0, 3}
		}},
		{"Always Show Decimal", {
			.helpText = "Always show the decimal point (and following zero) even if there are no decimal places to show (this option is\n"
			"only relevant if the \"Trim Zeroes after Decimal\" option is set).",
			.clickAction = [](CommandHandler *c) { c->toggleOption(AlwaysShowDecimal); },
			.iconAction = [](CommandHandler *c) -> std::string { return c->getCheckIcon(c->getOption(AlwaysShowDecimal)); },
			.location = {0, 1, 3}
		}},
		{"Trim Zeroes after Decimal", {
			.helpText = "Do not show trailing zeroes (after the decimal point), regardless of the setting of \"Decimal Places to Show\".\n"
			"See also the \"Always Show Decimal\" option.",
			.clickAction = [](CommandHandler *c) { c->toggleOption(TrimZeroes); },
			.iconAction = [](CommandHandler *c) -> std::string { return c->getCheckIcon(c->getOption(TrimZeroes)); },
			.location = {0, 2, 3}
		}},
		{"Save Stack on Exit", {
			.helpText = "When quitting ARPCalc, save the current stack state to allow\n"
			"continuing when you restart.",
			.clickAction = [](CommandHandler *c) { c->toggleOption(SaveStackOnExit); },
			.iconAction = [](CommandHandler *c) -> std::string { return c->getCheckIcon(c->getOption(SaveStackOnExit)); },
			.location = {0, 3, 3}
		}},
		{"Use Replicating Stack", {
			.helpText = "Use a four-entry stack that replicates the bottom entry (T) rather than an infinite stack.",
			.clickAction = [](CommandHandler *c) { c->toggleOption(ReplicateStack); },
			.iconAction = [](CommandHandler *c) -> std::string { return c->getCheckIcon(c->getOption(ReplicateStack)); },
			.location = {0, 4, 3}
		}},
		{"Select Base", {
			.helpText = "Toggle between decimal, hexadecimal, binary and octal.",
			.clickAction = [](CommandHandler *c) { c->nextBase(); },
			.iconAction = [](CommandHandler *c) -> std::string { return c->getBaseIcon(); },
			.location = {1, 0, 0}
		}},
		{"Save Base on Exit", {
			.helpText = "Save base between sessions (so if you exit in hexadecimal mode,\n"
			"ARPCalc will start next time in hexadecimal mode).",
			.clickAction = [](CommandHandler *c) { c->toggleOption(SaveBaseOnExit); },
			.iconAction = [](CommandHandler *c) -> std::string { return c->getCheckIcon(c->getOption(SaveBaseOnExit)); },
			.location = {1, 1, 0}
		}},
		{"Show Thousands Separator", {
			.helpText = "For values of 1000 or greater, show thousands separators for readability (e.g. 1,234,567.0).",
			.clickAction = [](CommandHandler *c) { c->toggleOption(ThousandsSeparator); },
			.iconAction = [](CommandHandler *c) -> std::string { return c->getCheckIcon(c->getOption(ThousandsSeparator)); },
			.location = {1, 2, 0}
		}},
		{"Space As Thousands Separator", {
			.helpText = "If show thousands separator option is set, use spaces as the separator (e.g. 1&nbsp;234&nbsp;567.0).",
			.clickAction = [](CommandHandler *c) { c->toggleOption(SpaceAsThousandsSeparator); },
			.iconAction = [](CommandHandler *c) -> std::string { return c->getCheckIcon(c->getOption(SpaceAsThousandsSeparator)); },
			.location = {1, 3, 0}
		}},
		{"Bit Count for Bitwise Ops", {
			.helpText = "When performing bitwise operations, assume that the data size is set to this value.\n"
			"This is also used for the top-right base display.",
			.clickAction = [](CommandHandler *c) { c->nextBitCount(); },
			.iconAction = [](CommandHandler *c) -> std::string { return c->getBitCountIcon(); },
			.location = {1, 4, 0}
		}},
		{"Percent Leaves Y Unchanged", {
			.helpText = "When using % or &Delta;%, leave Y in place.",
			.clickAction = [](CommandHandler *c) { c->toggleOption(PercentLeavesY); },
			.iconAction = [](CommandHandler *c) -> std::string { return c->getCheckIcon(c->getOption(PercentLeavesY)); },
			.location = {1, 0, 3}
		}},
		{"American Units on Keypad", {
			.helpText = "When set, the main conversion screen shows more American units.",
			.clickAction = [](CommandHandler *c) { c->toggleOption(AmericanUnits); },
			.iconAction = [](CommandHandler *c) -> std::string { return c->getCheckIcon(c->getOption(AmericanUnits)); },
			.location = {1, 1, 3}
		}},
		{"European-Style Decimal", {
			.helpText = "When set, decimal places are shown with a comma.",
			.clickAction = [](CommandHandler *c) { c->toggleOption(EuropeanDecimal); },
			.iconAction = [](CommandHandler *c) -> std::string { return c->getCheckIcon(c->getOption(EuropeanDecimal)); },
			.location = {1, 2, 3}
		}},
		{"Decimal Base Bias", {
			.helpText = "When set, the first press of \"BASE\" will change to decimal when in any other base.",
			.clickAction = [](CommandHandler *c) { c->toggleOption(DecimalBaseBias); },
			.iconAction = [](CommandHandler *c) -> std::string { return c->getCheckIcon(c->getOption(DecimalBaseBias)); },
			.location = {1, 3, 3}
		}},
		{"Check for New Versions", {
			.helpText = "Check for new versions on start-up.",
			.clickAction = [](CommandHandler *c) { c->toggleOption(CheckForNewVersions); },
			.iconAction = [](CommandHandler *c) -> std::string { return c->getCheckIcon(c->getOption(CheckForNewVersions)); },
			.location = {1, 4, 3},
			.hideOnPhone = true,
			.hideOnWeb = true
		}}
	};

	std::map<std::string, Option> optMap = optMapInit;
	return optMap;
}

std::string CommandHandler::getStoreHelpText(std::string register_)
{
	std::string storeHelpText = "Store/Recall X in the selected register";
	if (varStore.contains(register_)) {
		AF currentValue = varStore[register_];
		std::string formattedValue = formatDecimal(currentValue, false);
		storeHelpText += " (current value is " + formattedValue + ")";
	}
	storeHelpText += ".";
	return storeHelpText;
}

std::vector< std::vector<BI> > CommandHandler::getGrid(std::string grid)
{
	BI nop = {.name = "NOP", .display = "", .hidden = true};
	std::vector< std::vector<BI> > result;

	result.resize(6, std::vector<BI>(6, nop));

	if (grid == "numpad") {
		std::vector< std::vector<BI> > numpadGrid{
			{
				{"ShowAll", "SHOW", "Ignore decimal places setting temporarily and show all non-zero digits."},
				{"SI", "SI", "Multiply X by a standard SI prefix."},
				{"EngL", "ENG&larr;", "Adjust the exponent used to display the current value of X."},
				{"EngR", "ENG&rarr;", "Adjust the exponent used to display the current value of X."},
				{"pi", "&pi;", "&pi; (3.141...): push onto stack."},
				{"backspace", "DEL", "Backspace: correct entered values."}
			},
			{
				{"base", "BASE", "Switch between decimal, hexadecimal, binary and octal."},
				{"random", "RAND", "Push a random number onto the stack."},
				{"exponent", "EXP", "Add an exponent to the entered value."},
				{"sqrt", "&radic;x", "Calculate the square-root of X."}, // Available if needed as duplicated on funcpad
				{"reciprocal", "1/x", "Calculate the reciprocal of X."},
				{"divide", "&divide;", "Divide Y by X."}
			},
			{
				{"store", "STO", "Store the current value of X for later use."},
				{"drop", "DROP", "Pop X off the stack and discard it."},
				{"7", "7"},
				{"8", "8"},
				{"9", "9"},
				{"times", "&times;", "Multiply Y by X."}
			},
			{
				{"recall", "RCL", "Recall a previously stored value and push it onto the stack."},
				{"swap", "x &hArr; y", "Swap the values in X and Y."},
				{"4", "4"},
				{"5", "5"},
				{"6", "6"},
				{"minus", "&ndash;", "Subtract X from Y."}
			},
			{
				{"undo", "UNDO", "Undo the last operation that modified the stack."},
				{"invert", "&plusmn;", "Swap the sign of the current value. If currently entering an exponent, switch sign of the exponent."},
				{"1", "1"},
				{"2", "2"},
				{"3", "3"},
				{"plus", "+", "Add Y to X."}
			},
			{
				{.name="clear", .display="CLEAR", .helpText="Press once to clear X, press again to clear the whole stack.", .doubleWidth = true},
				{"0", "0"},
				{"dot", "&middot;"},
				{.name="enter", .display="ENTER", .helpText="Push the entered value onto the stack.", .doubleWidth = true}
			}
		};
		result = numpadGrid;
		if ( ! getOption(SaveHistory)) {
			result[4][0] = nop; // remove undo if save history not set
		}
		if (dspBase == baseHexadecimal) {
			result[0][2] = BI{"D", "D"};
			result[0][3] = BI{"E", "E"};
			result[0][4] = BI{"F", "F"};
			result[1][2] = BI{"A", "A"};
			result[1][3] = BI{"B", "B"};
			result[1][4] = BI{"C", "C"};
			result[5][2] = nop;
		}
		else if (dspBase == baseOctal) {
			for (int row = 0 ; row < 2; row++) {
				for (int col = 2; col < 5; col++) {
					result[row][col] = nop;
				}
			}
			result[2][3] = nop;
			result[2][4] = nop;
			result[5][2] = nop;
		}
		else if (dspBase == baseBinary) {
			for (int row = 0 ; row < 4; row++) {
				for (int col = 2 ; col < 5; col++) {
					result[row][col] = nop;
				}
			}
			result[4][3] = nop;
			result[4][4] = nop;
			result[5][2] = nop;
		}
		else {
			// stet
		}
	}
	else if ((grid == "funcpad") || (grid == "hypfuncpad")) {
		std::vector< std::vector<BI> > funcGrid{
			{
				{"rollUp", "ROLL&uarr;", "Roll the stack up."},
				{"tentox", "10<sup><small>x</small></sup>", "Raise 10 to the power of X."},
				{"etox", "e<sup><small>x</small></sup>", "Raise e (Euler's number) to the power of X."},
				{"twotox", "2<sup><small>x</small></sup>", "Raise 2 to the power of X."},
				{"round", "ROUND", "Round X to the nearest integer."},
				{"absolute", "ABS", "If X is negative, invert it, otherwise leave as is."}
			},
			{
				{"rollDown", "ROLL&darr;", "Roll the stack down."},
				{"log10", "log10", "Calculate the log (base 10) of X."},
				{"loge", "ln", "Calculate the log (base e) of X."},
				{"log2", "log2", "Calculate the log (base 2) of X."},
				{"floor", "FLOOR", "Round down to the nearest integer."},
				{"integerpart", "INT", "Remove the floating point part of X, leaving only the integer part - equivalent to FLOOR for X &gt; 0 or CEIL for X &lt; 0."}
			},
			{
				{"sqrt", "&radic;x", "Calculate the square-root of X."},
				{"cuberoot", "3&radic;x", "Calculate the cube-root of X."},
				{"xrooty", "x&radic;y", "Calculate the X-root of Y."},
				{"percent", "%", "Convert X to a percentage of Y."}, // TODO: Check operation
				{"ceiling", "CEIL", "Round up to the nearest integer."},
				{"floatingpart", "FP", "Remove the integer part of X, leaving only the floating point part."}
			},
			{
				{"square", "x<sup><small>2</small></sup>", "Calculate X&times;X."},
				{"cube", "x<sup><small>3</small></sup>", "Calculate X&times;X&times;X"},
				{"power", "y<sup><small>x</small></sup>", "Raise Y to the power of X."},
				{"percentchange", "&Delta;%", "Calculate the percentage change of X vs Y."}, // TODO: Check operation
				{"remainder", "REM", "Calculate the remainder after an integer division of Y / X."},
				{"integerdivide", "INTDIV", "Perform an integer division Y / X."}
			},
			{
				{"bitwiseand", "x & y", "Perform a bitwise AND on the integer part of X and Y."},
				{"bitwiseor", "x | y", "Perform a bitwise OR on the integer part of X and Y."},
				{"inversesin", "sin<sup><small>-1</small></sup>(x)", "Calculate the inverse sine of X."},
				{"inversecos", "cos<sup><small>-1</small></sup>(x)", "Calculate the inverse cosine of X."},
				{"inversetan", "tan<sup><small>-1</small></sup>(x)", "Calculate the inverse tangent of X."},
				{"inversetan2", "tan<sup><small>-1</small></sup>(y/x)", "Calculate the inverse tangent of Y/X."}
			},
			{
				{"bitwisenot", "~ x", "Perform a bitwise NOT on the integer part of X."},
				{"bitwisexor", "x ^ y", "Perform a bitwise XOR on the integer part of X."},
				{"sin", "sin(x)", "Calculate the sine of X."},
				{"cos", "cos(x)", "Calculate the cosine of X."},
				{"tan", "tan(x)", "Calculate the tangent of X."},
				{"hypmode", "HYP", "Switch to hyperbolic trigonometry mode."}
			}
		};
		result = funcGrid;

		if (grid == "hypfuncpad") {
			result[4][2] = BI{"inversesinh", "sinh<sup><small>-1</small></sup>(x)", "Calculate the inverse hyperbolic sine of X."};
			result[4][3] = BI{"inversecosh", "cosh<sup><small>-1</small></sup>(x)", "Calculate the inverse hyperbolic cosine of X."};
			result[4][4] = BI{"inversetanh", "tanh<sup><small>-1</small></sup>(x)", "Calculate the inverse hyperbolic tangent of X."};
			result[4][5] = BI{"inversetanh2", "tanh<sup><small>-1</small></sup>(y/x)", "Calculate the inverse hyperbolic tangent of Y/X.", 0.8f};
			result[5][2] = BI{"sinh", "sinh(x)", "Calculate the hyperbolic sine of X."};
			result[5][3] = BI{"cosh", "cosh(x)", "Calculate the hyperbolic cosine of X."};
			result[5][4] = BI{"tanh", "tanh(x)", "Calculate the hyperbolic tangent of X."};
		}
	}
	else if (grid == "convpad") {
		std::vector< std::vector<BI> > convGrid{
			{
				{"Convert_Distance_Inches_Millimetres",                              st.unitSymbols["Millimetres"]+"&larr;",                "Convert Inches to Millimetres."},
				{"Convert_Distance_Millimetres_Inches",                              "&rarr;"+st.unitSymbols["Inches"],                     "Convert Millimetres to Inches."},
				{"Convert_Mass_Ounces_Grams",                                        st.unitSymbols["Grams"]+"&larr;",                      "Convert Ounces to Grams."},
				{"Convert_Mass_Grams_Ounces",                                        "&rarr;"+st.unitSymbols["Ounces"],                     "Convert Grams to Ounces."},
				{"Convert_Angle_Radians_Degrees",                                    st.unitSymbols["Degrees"]+"&larr;",                    "Convert Radians to Degrees."},
				{"Convert_Angle_Degrees_Radians",                                    "&rarr;"+st.unitSymbols["Radians"],                    "Convert Degrees to Radians."}
			},
			{
				{"Convert_Distance_Miles_Kilometres",                                st.unitSymbols["Kilometres"]+"&larr;",                 "Convert Miles to Kilometres."},
				{"Convert_Distance_Kilometres_Miles",                                "&rarr;"+st.unitSymbols["Miles"],                      "Convert Kilometres to Miles."},
				{"Convert_Mass_Pounds_Kilograms",                                    st.unitSymbols["Kilograms"]+"&larr;",                  "Convert Pounds to Kilograms."},
				{"Convert_Mass_Kilograms_Pounds",                                    "&rarr;"+st.unitSymbols["Pounds"],                     "Convert Kilograms to Pounds."},
				{"Convert_Angle_Degrees.Minutes-Seconds_Degrees",                    st.unitSymbols["Degrees"]+"&larr;",                    "Convert Hours.Minutes-Seconds (or Degrees.Minutes-Seconds) to Hours (or Degrees)."},
				{"Convert_Angle_Degrees_Degrees.Minutes-Seconds",                    "&rarr;"+st.unitSymbols["Degrees.Minutes-Seconds"],    "Convert Hours (or Degrees) to Hours.Minutes-Seconds (or Degrees.Minutes-Seconds)."}
			},
			{
				{"Convert_Volume_Gallons_Litres",                                    st.unitSymbols["Litres"]+"&larr;",                     "Convert Gallons to Litres."},
				{"Convert_Volume_Litres_Gallons",                                    "&rarr;"+st.unitSymbols["Gallons"],                    "Convert Litres to Gallons."},
				{"Convert_Temperature_Fahrenheit_Celsius",                           st.unitSymbols["Celsius"]+"&larr;",                    "Convert Fahrenheit to Celsius."},
				{"Convert_Temperature_Celsius_Fahrenheit",                           "&rarr;"+st.unitSymbols["Fahrenheit"],                 "Convert Celsius to Fahrenheit."},
				{"Convert_Volume_Cubic Millimetres_Cubic Metres",                    st.unitSymbols["Cubic Metres"]+"&larr;",               "Convert Cubic Millimetres to Cubic Metres."},
				{"Convert_Volume_Cubic Metres_Cubic Millimetres",                    "&rarr;"+st.unitSymbols["Cubic Millimetres"],          "Convert Cubic Metres to Cubic Millimetres."}
			},
			{
				{"Convert_Volume_Pints_Litres",                                      st.unitSymbols["Litres"]+"&larr;",                     "Convert Pints to Litres."},
				{"Convert_Volume_Litres_Pints",                                      "&rarr;"+st.unitSymbols["Pints"],                      "Convert Litres to Pints."},
				{"Convert_Fuel Economy_Miles Per Litre_Miles Per Gallon",            st.unitSymbols["Miles Per Gallon"]+"&larr;",           "Convert Miles Per Litre to Miles Per Gallon."},
				{"Convert_Fuel Economy_Miles Per Gallon_Miles Per Litre",            "&rarr;"+st.unitSymbols["Miles Per Litre"],            "Convert Miles Per Gallon to Miles Per Litre."},
				{"Convert_Frequency_RPM_Hertz",                                      st.unitSymbols["Hertz"]+"&larr;",                      "Convert RPM to Hertz."},
				{"Convert_Frequency_Hertz_RPM",                                      "&rarr;"+st.unitSymbols["RPM"],                        "Convert Hertz to RPM."}
			},
			{
				{"Convert_Volume_Fluid Ounces_Litres",                               st.unitSymbols["Litres"]+"&larr;",                     "Convert Fluid Ounces to Litres."},
				{"Convert_Volume_Litres_Fluid Ounces",                               "&rarr;"+st.unitSymbols["Fluid Ounces"],               "Convert Litres to Fluid Ounces."},
				{"Convert_Fuel Economy_Litres Per 100 Kilometres_Miles Per Gallon",  st.unitSymbols["Miles Per Gallon"]+"&larr;",           "Convert Litres Per 100 Kilometres to Miles Per Gallon."},
				{.name="Convert_Fuel Economy_Miles Per Gallon_Litres Per 100 Kilometres",  .display="&rarr;"+st.unitSymbols["Litres Per 100 Kilometres"],  .helpText="Convert Miles Per Gallon to Litres Per 100 Kilometres.", .scale=0.8f},
				{"Convert_Torque_Pounds Feet_Newton Metres",                         st.unitSymbols["Newton Metres"]+"&larr;",              "Convert Pound-Force Feet to Newton Metres."},
				{"Convert_Torque_Newton Metres_Pounds Feet",                         "&rarr;"+st.unitSymbols["Pound-Force Feet"],           "Convert Newton Metres to Pound-Force Feet."}
			},
			{
				{"Convert_Volume_US Pints_Pints",                                    st.unitSymbols["Pints"]+"&larr;",                      "Convert US Pints to Pints."},
				{"Convert_Volume_Pints_US Pints",                                    "&rarr;"+st.unitSymbols["US Pints"],                   "Convert Pints to US Pints."},
				{"Convert_Date_Day of Year_Date in Year",                            st.unitSymbols["Date in Year"]+"&larr;",               "Convert Day Number as NN.(YYYY) to Date as DD.MM(YYYY) - current year is assumed if not specified."},
				{"Convert_Date_Date in Year_Day of Year",                            "&rarr;"+st.unitSymbols["Day of Year"],                "Convert Date as DD.MM(YYYY) to Day Number as NN.(YYYY) - current year is assumed if not specified."},
				{.name="MoreConversions",                                            .display="MORE",                                       .helpText="Open a menu with other possible conversions", .doubleWidth = true}
			}
		};
		result = convGrid;

		if (getOption(AmericanUnits)) {
			std::map<std::string, std::string> us_change;
			us_change["Gallons"] = "US Gallons";
			us_change["Pints"] = "US Pints";
			us_change["Fluid Ounces"] = "US Fluid Ounces";
			us_change["Miles Per Gallon"] = "Miles Per US Gallon";

			for (int i=0;i<(int) result.size();i++) {
				std::vector<BI> ml = result[i];
				for (int j=0;j<(int)ml.size();j++) {
					BI bi = ml[j];
					if (contains(bi.name, "US ")) {
						// Already a US conversion
						continue;
					}

					std::vector<std::string> parts = split(bi.name, '_');
					if (parts.size() < 4) {
						continue;
					}
					std::string cat = parts[1];
					std::string from = parts[2];
					std::string to = parts[3];

					if (us_change.contains(from)) {
						from = us_change[from];
					}
					if (us_change.contains(to)) {
						to = us_change[to];
					}
					std::string disp;
					if (contains(bi.display, "&larr")) {
						disp = st.unitSymbols[to] + "&larr;";
					}
					else {
						disp = "&rarr;" + st.unitSymbols[to];
					}
					std::string newname = "Convert_" + cat + "_" + from + "_" + to;
					result[i][j].name = newname;
					result[i][j].display = disp;
					result[i][j].helpText = "Convert " + from + " to " + to;
				}
			}
		}
	}
	else if (grid == "constpad") {
		std::vector<std::vector<BI> > vres;
		std::vector<BI> firstRow;
		vres.push_back(firstRow);
		int colCount = 0;
		int totalCount = 0;
		std::vector<Constant> constants = st.getConstants();

		for (Constant k: constants) {
			if (colCount >= 6) {
				std::vector<BI> thisRow;
				vres.push_back(thisRow);
				colCount = 0;
			}
			std::string helptext = k.name + ":<br><span style=\"white-space: nowrap;\">";
			helptext += formatDecimal(k.value, false, true);
			if ( ! getOption(SINotation)) {
				helptext += "&nbsp;";
			}
			helptext += k.unit + "</span>";

			BI b = {"Const-" + k.name, k.symbol, helptext};
			vres.back().push_back(b);
			colCount += 1;
			totalCount += 1;
			if (totalCount >= 36) {
				break;
			}
		}
		while (totalCount < 32) {
			if (colCount >= 6) {
				std::vector<BI> thisRow;
				vres.push_back(thisRow);
				colCount = 0;
			}
			vres.back().push_back(nop);
			colCount += 1;
			totalCount += 1;
		}
		BI density = {.name="DensityByName", .display="Material Density", .helpText="Material Densities in kg/m&#x00B3;", .doubleWidth=true};
		vres.back().push_back(density);
		BI byname = {.name="ConstByName", .display="Add By Name", .doubleWidth=true};
		vres.back().push_back(byname);
		result = vres;
	}
	else if (grid == "sipad") {
		std::vector< std::vector<BI> > siGrid{
			{
				{.name="NOP", .display="Prefixes > 1:", .doubleWidth = true},
				{.name="NOP", .display="Prefixes < 1:", .doubleWidth = true},
				{.name="NOP", .display="Binary Prefixes:", .doubleWidth = true}
			},
			{
				{"SI-Yotta", "Y", "SI prefix Yotta: multiply X by 1&times;10&#x00B2;&#x2074;."},
				{"SI-Tera", "T", "SI prefix Tera: multiply X by 1&times;10&#x00B9;&#x00B2;."},
				{"SI-Milli", "m", "SI prefix Milli: multiply X by 1&times;10&#x207B;&#x00B3;."},
				{"SI-Femto", "f", "SI prefix Femto: multiply X by 1&times;10&#x207B;&#x00B9;&#x2075;."},
				{"SI-Exbi", "Ei", "SI prefix Exbi: multiply X by 2&#x2076;&#x2070;."},
				{"SI-Gibi", "Gi", "SI prefix Gibi: multiply X by 2&#x00B3;&#x2070;."}
			},
			{
				{"SI-Zetta", "Z", "SI prefix Zetta: multiply X by 1&times;10&#x00B2;&#x00B9;."},
				{"SI-Giga", "G", "SI prefix Giga: multiply X by 1&times;10&#x2079;."},
				{"SI-Micro", "&mu;", "SI prefix Micro: multiply X by 1&times;10&#x207B;&#x2076;."},
				{"SI-Atto", "a", "SI prefix Atto: multiply X by 1&times;10&#x207B;&#x00B9;&#x2078;."},
				{"SI-Pebi", "Pi", "SI prefix Pebi: multiply X by 2&#x2075;&#x2070;."},
				{"SI-Mebi", "Mi", "SI prefix Mebi: multiply X by 2&#x00B2;&#x2070;."}
			},
			{
				{"SI-Exa", "E", "SI prefix Exa: multiply X by 1&times;10&#x00B9;&#x2078;."},
				{"SI-Mega", "M", "SI prefix Mega: multiply X by 1&times;10&#x2076;."},
				{"SI-Nano", "n", "SI prefix Nano: multiply X by 1&times;10&#x207B;&#x2079;."},
				{"SI-Zepto", "z", "SI prefix Zepto: multiply X by 1&times;10&#x207B;&#x00B2;&#x00B9;."},
				{"SI-Tebi", "Ti", "SI prefix Tebi: multiply X by 2&#x2074;&#x2070;."},
				{"SI-Kibi", "Ki", "SI prefix Kibi: multiply X by 2&#x00B9;&#x2070;."}
			},
			{
				{"SI-Peta", "P", "SI prefix Peta: multiply X by 1&times;10&#x00B9;&#x2075;."},
				{"SI-Kilo", "k", "SI prefix Kilo: multiply X by 1&times;10&#x00B3;."},
				{"SI-Pico", "p", "SI prefix Pico: multiply X by 1&times;10&#x207B;&#x00B9;&#x00B2;."},
				{"SI-Yocto", "y", "SI prefix Yocto: multiply X by 1&times;10&#x207B;&#x00B2;&#x2074;."},
				nop,
				nop
			},
			{
				nop,
				nop,
				nop,
				nop,
				{.name="CANCEL", .display="Cancel", .doubleWidth = true}
			}
		};
		result = siGrid;
	}
	else if (grid == "romanupperpad") {
		std::vector< std::vector<BI> > romanUpperGrid{
			{
				{"StoreRomanUpperA", "A", getStoreHelpText("StoreRomanUpperA")},
				{"StoreRomanUpperB", "B", getStoreHelpText("StoreRomanUpperB")},
				{"StoreRomanUpperC", "C", getStoreHelpText("StoreRomanUpperC")},
				{"StoreRomanUpperD", "D", getStoreHelpText("StoreRomanUpperD")},
				{"StoreRomanUpperE", "E", getStoreHelpText("StoreRomanUpperE")},
				{"StoreRomanUpperF", "F", getStoreHelpText("StoreRomanUpperF")}
			},
			{
				{"StoreRomanUpperG", "G", getStoreHelpText("StoreRomanUpperG")},
				{"StoreRomanUpperH", "H", getStoreHelpText("StoreRomanUpperH")},
				{"StoreRomanUpperI", "I", getStoreHelpText("StoreRomanUpperI")},
				{"StoreRomanUpperJ", "J", getStoreHelpText("StoreRomanUpperJ")},
				{"StoreRomanUpperK", "K", getStoreHelpText("StoreRomanUpperK")},
				{"StoreRomanUpperL", "L", getStoreHelpText("StoreRomanUpperL")}
			},
			{
				{"StoreRomanUpperM", "M", getStoreHelpText("StoreRomanUpperM")},
				{"StoreRomanUpperN", "N", getStoreHelpText("StoreRomanUpperN")},
				{"StoreRomanUpperO", "O", getStoreHelpText("StoreRomanUpperO")},
				{"StoreRomanUpperP", "P", getStoreHelpText("StoreRomanUpperP")},
				{"StoreRomanUpperQ", "Q", getStoreHelpText("StoreRomanUpperQ")},
				{"StoreRomanUpperR", "R", getStoreHelpText("StoreRomanUpperR")}
			},
			{
				{"StoreRomanUpperS", "S", getStoreHelpText("StoreRomanUpperS")},
				{"StoreRomanUpperT", "T", getStoreHelpText("StoreRomanUpperT")},
				{"StoreRomanUpperU", "U", getStoreHelpText("StoreRomanUpperU")},
				{"StoreRomanUpperV", "V", getStoreHelpText("StoreRomanUpperV")},
				{"StoreRomanUpperW", "W", getStoreHelpText("StoreRomanUpperW")},
				{"StoreRomanUpperX", "X", getStoreHelpText("StoreRomanUpperX")}
			},
			{
				{"StoreRomanUpperY", "Y", getStoreHelpText("StoreRomanUpperY")},
				{"StoreRomanUpperZ", "Z", getStoreHelpText("StoreRomanUpperZ")},
				nop,
				nop,
				nop,
				nop
			},
			{
				nop,
				nop,
				nop,
				nop,
				{.name="CANCEL", .display="Cancel", .doubleWidth = true}
			}
		};
		result = romanUpperGrid;
	}
	else if (grid == "romanlowerpad") {
		std::vector< std::vector<BI> > romanLowerGrid{
			{
				{"StoreRomanLowerA", "a", getStoreHelpText("StoreRomanLowerA")},
				{"StoreRomanLowerB", "b", getStoreHelpText("StoreRomanLowerB")},
				{"StoreRomanLowerC", "c", getStoreHelpText("StoreRomanLowerC")},
				{"StoreRomanLowerD", "d", getStoreHelpText("StoreRomanLowerD")},
				{"StoreRomanLowerE", "e", getStoreHelpText("StoreRomanLowerE")},
				{"StoreRomanLowerF", "f", getStoreHelpText("StoreRomanLowerF")}
			},
			{
				{"StoreRomanLowerG", "g", getStoreHelpText("StoreRomanLowerG")},
				{"StoreRomanLowerH", "h", getStoreHelpText("StoreRomanLowerH")},
				{"StoreRomanLowerI", "i", getStoreHelpText("StoreRomanLowerI")},
				{"StoreRomanLowerJ", "j", getStoreHelpText("StoreRomanLowerJ")},
				{"StoreRomanLowerK", "k", getStoreHelpText("StoreRomanLowerK")},
				{"StoreRomanLowerL", "l", getStoreHelpText("StoreRomanLowerL")}
			},
			{
				{"StoreRomanLowerM", "m", getStoreHelpText("StoreRomanLowerM")},
				{"StoreRomanLowerN", "n", getStoreHelpText("StoreRomanLowerN")},
				{"StoreRomanLowerO", "o", getStoreHelpText("StoreRomanLowerO")},
				{"StoreRomanLowerP", "p", getStoreHelpText("StoreRomanLowerP")},
				{"StoreRomanLowerQ", "q", getStoreHelpText("StoreRomanLowerQ")},
				{"StoreRomanLowerR", "r", getStoreHelpText("StoreRomanLowerR")}
			},
			{
				{"StoreRomanLowerS", "s", getStoreHelpText("StoreRomanLowerS")},
				{"StoreRomanLowerT", "t", getStoreHelpText("StoreRomanLowerT")},
				{"StoreRomanLowerU", "u", getStoreHelpText("StoreRomanLowerU")},
				{"StoreRomanLowerV", "v", getStoreHelpText("StoreRomanLowerV")},
				{"StoreRomanLowerW", "w", getStoreHelpText("StoreRomanLowerW")},
				{"StoreRomanLowerX", "x", getStoreHelpText("StoreRomanLowerX")}
			},
			{
				{"StoreRomanLowerY", "y", getStoreHelpText("StoreRomanLowerY")},
				{"StoreRomanLowerZ", "z", getStoreHelpText("StoreRomanLowerZ")},
				nop,
				nop,
				nop,
				nop
			},
			{
				nop,
				nop,
				nop,
				nop,
				{.name="CANCEL", .display="Cancel", .doubleWidth = true}
			}
		};
		result = romanLowerGrid;
	}
	else if (grid == "greekupperpad") {
		std::vector< std::vector<BI> > greekUpperGrid{
			{
				{"StoreGreekUpperAlpha",    "&Alpha;", getStoreHelpText("StoreGreekUpperAlpha")},
				{"StoreGreekUpperBeta",     "&Beta;", getStoreHelpText("StoreGreekUpperBeta")},
				{"StoreGreekUpperGamma",    "&Gamma;", getStoreHelpText("StoreGreekUpperGamma")},
				{"StoreGreekUpperDelta",    "&Delta;", getStoreHelpText("StoreGreekUpperDelta")},
				{"StoreGreekUpperEpsilon",  "&Epsilon;", getStoreHelpText("StoreGreekUpperEpsilon")},
				{"StoreGreekUpperZeta",     "&Zeta;", getStoreHelpText("StoreGreekUpperZeta")}
				},
			{
				{"StoreGreekUpperEta",      "&Eta;", getStoreHelpText("StoreGreekUpperEta")},
				{"StoreGreekUpperTheta",    "&Theta;", getStoreHelpText("StoreGreekUpperTheta")},
				{"StoreGreekUpperIota",     "&Iota;", getStoreHelpText("StoreGreekUpperIota")},
				{"StoreGreekUpperKappa",    "&Kappa;", getStoreHelpText("StoreGreekUpperKappa")},
				{"StoreGreekUpperLambda",   "&Lambda;", getStoreHelpText("StoreGreekUpperLambda")},
				{"StoreGreekUpperMu",       "&Mu;", getStoreHelpText("StoreGreekUpperMu")}
				},
			{
				{"StoreGreekUpperNu",       "&Nu;", getStoreHelpText("StoreGreekUpperNu")},
				{"StoreGreekUpperXi",       "&Xi;", getStoreHelpText("StoreGreekUpperXi")},
				{"StoreGreekUpperOmicron",  "&Omicron;", getStoreHelpText("StoreGreekUpperOmicron")},
				{"StoreGreekUpperPi",       "&Pi;", getStoreHelpText("StoreGreekUpperPi")},
				{"StoreGreekUpperRho",      "&Rho;", getStoreHelpText("StoreGreekUpperRho")},
				{"StoreGreekUpperSigma",    "&Sigma;", getStoreHelpText("StoreGreekUpperSigma")}
				},
			{
				{"StoreGreekUpperTau",      "&Tau;", getStoreHelpText("StoreGreekUpperTau")},
				{"StoreGreekUpperUpsilon",  "&Upsilon;", getStoreHelpText("StoreGreekUpperUpsilon")},
				{"StoreGreekUpperPhi",      "&Phi;", getStoreHelpText("StoreGreekUpperPhi")},
				{"StoreGreekUpperChi",      "&Chi;", getStoreHelpText("StoreGreekUpperChi")},
				{"StoreGreekUpperPsi",      "&Psi;", getStoreHelpText("StoreGreekUpperPsi")},
				{"StoreGreekUpperOmega",    "&Omega;", getStoreHelpText("StoreGreekUpperOmega")}
			},
			{
				nop,
				nop,
				nop,
				nop,
				nop,
				nop
			},
			{
				nop,
				nop,
				nop,
				nop,
				{.name="CANCEL", .display="Cancel", .doubleWidth = true}
			}
		};
		result = greekUpperGrid;
	}
	else if (grid == "greeklowerpad") {
		std::vector< std::vector<BI> > greekLowerGrid{
			{
				{"StoreGreekLowerAlpha",    "&alpha;", getStoreHelpText("StoreGreekLowerAlpha")},
				{"StoreGreekLowerBeta",     "&beta;", getStoreHelpText("StoreGreekLowerBeta")},
				{"StoreGreekLowerGamma",    "&gamma;", getStoreHelpText("StoreGreekLowerGamma")},
				{"StoreGreekLowerDelta",    "&delta;", getStoreHelpText("StoreGreekLowerDelta")},
				{"StoreGreekLowerEpsilon",  "&epsilon;", getStoreHelpText("StoreGreekLowerEpsilon")},
				{"StoreGreekLowerZeta",     "&zeta;", getStoreHelpText("StoreGreekLowerZeta")}
				},
			{
				{"StoreGreekLowerEta",      "&eta;", getStoreHelpText("StoreGreekLowerEta")},
				{"StoreGreekLowerTheta",    "&theta;", getStoreHelpText("StoreGreekLowerTheta")},
				{"StoreGreekLowerIota",     "&iota;", getStoreHelpText("StoreGreekLowerIota")},
				{"StoreGreekLowerKappa",    "&kappa;", getStoreHelpText("StoreGreekLowerKappa")},
				{"StoreGreekLowerLambda",   "&lambda;", getStoreHelpText("StoreGreekLowerLambda")},
				{"StoreGreekLowerMu",       "&mu;", getStoreHelpText("StoreGreekLowerMu")}
				},
			{
				{"StoreGreekLowerNu",       "&nu;", getStoreHelpText("StoreGreekLowerNu")},
				{"StoreGreekLowerXi",       "&xi;", getStoreHelpText("StoreGreekLowerXi")},
				{"StoreGreekLowerOmicron",  "&omicron;", getStoreHelpText("StoreGreekLowerOmicron")},
				{"StoreGreekLowerPi",       "&pi;", getStoreHelpText("StoreGreekLowerPi")},
				{"StoreGreekLowerRho",      "&rho;", getStoreHelpText("StoreGreekLowerRho")},
				{"StoreGreekLowerSigma",    "&sigma;", getStoreHelpText("StoreGreekLowerSigma")}
				},
			{
				{"StoreGreekLowerTau",      "&tau;", getStoreHelpText("StoreGreekLowerTau")},
				{"StoreGreekLowerUpsilon",  "&upsilon;", getStoreHelpText("StoreGreekLowerUpsilon")},
				{"StoreGreekLowerPhi",      "&phi;", getStoreHelpText("StoreGreekLowerPhi")},
				{"StoreGreekLowerChi",      "&chi;", getStoreHelpText("StoreGreekLowerChi")},
				{"StoreGreekLowerPsi",      "&psi;", getStoreHelpText("StoreGreekLowerPsi")},
				{"StoreGreekLowerOmega",    "&omega;", getStoreHelpText("StoreGreekLowerOmega")}
			},
			{
				nop,
				nop,
				nop,
				nop,
				nop,
				nop
			},
			{
				nop,
				nop,
				nop,
				nop,
				{.name="CANCEL", .display="Cancel", .doubleWidth = true}
			}
		};
		result = greekLowerGrid;
	}

	return result;
}


