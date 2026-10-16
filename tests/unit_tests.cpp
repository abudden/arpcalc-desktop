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

/*
 * Unit tests for the calculator core (no Qt needed).
 *
 * These were ported from the Android (Kotlin) version's tests in
 * app/src/test: CommandsTest.kt, ConversionTest.kt, GridsTest.kt,
 * OpsTest.kt, SITest.kt, StackTest.kt and LayoutTest.kt.  Each C++ test
 * function has the same name as the Kotlin @Test it came from and uses the
 * same values and expectations, except where a comment says otherwise.
 *
 * defaultConversionTests (like the Kotlin original) exports the list of
 * units and runs tests/generate_conversion_tests.py, which uses python-pint
 * to generate the expected results.  If pint can't be imported, that
 * test is skipped.  Set PYTHON to choose the python interpreter (default
 * python3).
 *
 * Usage: unit_tests [test name...] (run from the top of the repository;
 * see run_tests.sh).  With no arguments, every test is run.  The program
 * exits non-zero if any check fails.
 */
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <list>
#include <set>
#include <source_location>
#include <sstream>
#include <string>
#include <vector>
#include "commands.h"

/* -------------------------------------------------------------------- */
/* Minimal test framework                                                */
/* -------------------------------------------------------------------- */

static int checkCount = 0;
static int failureCount = 0;
static int skippedCount = 0;
static std::string currentTest;

static std::string describe(const std::string &v) { return "\"" + v + "\""; }
static std::string describe(const char *v) { return describe(std::string(v)); }
static std::string describe(bool v) { return v ? "true" : "false"; }
static std::string describe(int v) { return std::to_string(v); }
static std::string describe(long v) { return std::to_string(v); }
static std::string describe(long long v) { return std::to_string(v); }
static std::string describe(ErrorCode v) { return "ErrorCode(" + std::to_string((int) v) + ")"; }
static std::string describe(AF v) { return v.toString(); }
static std::string describe(double v)
{
	std::ostringstream s;
	s.precision(17);
	s << v;
	return s.str();
}
static std::string describe(const std::vector<std::string> &v)
{
	std::string result = "[";
	for (size_t i=0;i<v.size();i++) {
		result += (i > 0 ? ", " : "") + describe(v[i]);
	}
	return result + "]";
}

static void recordCheck(bool passed, const std::string &message, const std::source_location &loc)
{
	checkCount++;
	if ( ! passed) {
		failureCount++;
		std::cerr << loc.file_name() << ":" << loc.line() << ": FAILED in "
			<< currentTest << ": " << message << std::endl;
	}
}

static void checkTrue(bool cond, const std::string &text,
		const std::source_location &loc = std::source_location::current())
{
	recordCheck(cond, text, loc);
}

template <typename T, typename U>
static void checkEqual(T expected, U actual, const std::string &text,
		const std::source_location &loc = std::source_location::current())
{
	bool passed = (expected == actual);
	recordCheck(passed, text + ": expected " + describe(expected)
		+ ", got " + describe(actual), loc);
}

static double toDbl(double v) { return v; }
static double toDbl(AF v) { return v.toDouble(); }

template <typename T, typename U>
static void checkClose(T expected, U actual, double delta, const std::string &text,
		const std::source_location &loc = std::source_location::current())
{
	double e = toDbl(expected);
	double a = toDbl(actual);
	// Written so that NaN fails
	bool passed = (std::fabs(e - a) <= delta);
	recordCheck(passed, text + ": expected " + describe(e)
		+ " +/- " + describe(delta) + ", got " + describe(a), loc);
}

#define CHECK(cond) checkTrue((cond), #cond)
#define CHECK_MSG(cond, msg) checkTrue((cond), msg)
#define CHECK_EQUAL(expected, actual) checkEqual((expected), (actual), #actual)
#define CHECK_EQUAL_MSG(msg, expected, actual) checkEqual((expected), (actual), msg)
#define CHECK_CLOSE(expected, actual, delta) checkClose((expected), (actual), (delta), #actual)
#define CHECK_CLOSE_MSG(msg, expected, actual, delta) checkClose((expected), (actual), (delta), msg)

/* -------------------------------------------------------------------- */
/* Helpers (equivalents of the Kotlin extension functions in OpsTest.kt) */
/* -------------------------------------------------------------------- */

static void checkTop(Stack &st, double v, bool justPeek = false,
		const std::source_location &loc = std::source_location::current())
{
	const double ALLOWED_DELTA = 1e-9;
	AF x = justPeek ? st.peek() : st.pop();
	checkClose(v, x, ALLOWED_DELTA, "checkTop", loc);
}

static void checkTop(Stack &st, std::string v, bool justPeek = false,
		const std::source_location &loc = std::source_location::current())
{
	AF vf(v);
	AF x = justPeek ? st.peek() : st.pop();
	checkEqual(vf, x, "checkTop", loc);
}

static void pushList(Stack &st, std::vector<double> vs)
{
	std::vector<AF> va;
	for (double v : vs) {
		va.push_back(AF(v));
	}
	st.push(va);
}

/* -------------------------------------------------------------------- */
/* CommandsTest.kt                                                       */
/* -------------------------------------------------------------------- */

static void defaultCommandTests()
{
	CommandHandler ch;

	/* TODO: How to test this?! */
	ch.keypress("1");
	ch.keypress("2");
	ch.keypress("backspace");
	ch.keypress("backspace");
	ch.keypress("backspace");
	ch.keypress("backspace");

	ch.keypress("1");
	ch.keypress("Convert_Distance_Kilometres_Inches");
	checkTop(ch.st, 1.0*1000.0*1000.0/25.4);

	/* Test that all conversions in grid work without crash */
	std::vector< std::vector<BI> > grid = ch.getGrid("convpad");
	for (auto &row : grid) {
		for (auto &bi : row) {
			if (bi.name.starts_with("Convert_")) {
				std::string check = "";
				if (bi.name == "Convert_Date_Date in Year_Day of Year") {
					ch.st.push(AF(std::string("12.021824")));
					check = "43.1824";
				}
				else if (bi.name == "Convert_Date_Day of Year_Date in Year") {
					ch.st.push(AF(std::string("60.2020")));
					check = "29.022020";
				}
				ch.keypress(bi.name);
				if (check != "") {
					checkTop(ch.st, check);
				}
			}
		}
	}

	ch.setOption(EngNotation, true);
	ch.setOption(PowerExponentView, false);
	ch.setOption(TrimZeroes, true);
	ch.setOption(AlwaysShowDecimal, true);
	ch.setOption(BinaryPrefixes, false);
	ch.setOption(ThousandsSeparator, true);
	ch.setPlaces(10);

	ch.selectBase(baseDecimal);
	ch.keypress("clear");
	ch.keypress("clear");
	ch.keypress("1");
	ch.keypress("2");
	ch.keypress(".");
	ch.keypress("3");
	ch.keypress("exponent");
	ch.keypress("6");
	ch.keypress("enter");
	CHECK_EQUAL("12.3e6", ch.getXDisplay());
	ch.keypress("EngL");
	CHECK_EQUAL("12,300.0e3", ch.getXDisplay());
	ch.keypress("EngR");
	ch.keypress("EngR");
	CHECK_EQUAL("0.0123e9", ch.getXDisplay());
	ch.keypress("clear");
	ch.keypress("clear");
	ch.keypress("1");
	ch.keypress("2");
	ch.keypress("3");
	ch.keypress("4");
	ch.keypress("5");
	CHECK_EQUAL("12,345", ch.getXDisplay());
	ch.keypress("enter");
	CHECK_EQUAL("12,345.0", ch.getXDisplay());
	ch.selectBase(baseHexadecimal);
	CHECK_EQUAL("0x3039", ch.getXDisplay());
	ch.selectBase(baseBinary);
	CHECK_EQUAL("0b0011&thinsp;0000&thinsp;0011&thinsp;1001", ch.getXDisplay());
	ch.selectBase(baseOctal);
	CHECK_EQUAL("0o3&thinsp;0071", ch.getXDisplay());
	ch.selectBase(baseDecimal);

	ch.keypress("clear");
	ch.keypress("clear");
	ch.keypress("5");
	ch.keypress("Convert_Distance_Inches_Millimetres");
	CHECK_EQUAL("127.0", ch.getXDisplay());
	ch.keypress(".");
	CHECK_EQUAL("0.", ch.getXDisplay());
	ch.keypress("5");
	CHECK_EQUAL("0.5", ch.getXDisplay());
	ch.keypress("times");
	CHECK_EQUAL("63.5", ch.getXDisplay());
	ch.keypress("clear");
	ch.keypress("clear");

	ch.selectBase(baseHexadecimal);
	ch.keypress("E");
	ch.keypress("A");
	ch.keypress("B");
	ch.keypress("C");
	ch.keypress("D");
	ch.keypress("F");
	ch.selectBase(baseDecimal);
	CHECK_EQUAL("15.383775e6", ch.getXDisplay());

	ch.keypress("clear");
	ch.keypress("clear");

	ch.setPlaces(5);
	ch.selectBase(baseHexadecimal);
	ch.keypress("A");
	ch.keypress("F");
	ch.keypress("8");
	ch.keypress("B");
	ch.keypress("C");
	ch.keypress("1");
	ch.keypress("5");
	ch.selectBase(baseDecimal);
	CHECK_EQUAL("184.07324e6", ch.getXDisplay());
	ch.keypress("EngL");
	CHECK_EQUAL("184,073.237e3", ch.getXDisplay());
	ch.keypress("EngL");
	CHECK_EQUAL("184,073,237.0", ch.getXDisplay());
	ch.keypress("EngL");
	CHECK_EQUAL("184,073,237,000.0e&ndash;3", ch.getXDisplay());
}

static void dmsConversionTest()
{
	CommandHandler ch;
	ch.setOption(EngNotation);
	ch.setOption(TrimZeroes);
	ch.setOption(AlwaysShowDecimal);
	ch.setOption(ThousandsSeparator);

	// Simple case
	ch.keypresses("10");
	ch.keypress("Convert_Angle_Degrees_Degrees.Minutes-Seconds");
	CHECK_EQUAL("10.0", ch.getXDisplay());

	// This relies on correct rounding
	ch.keypresses("19.15");
	ch.keypress("Convert_Angle_Degrees.Minutes-Seconds_Degrees");
	// (0.15*100 in binary used to give 14 minutes 99.99 seconds)
	CHECK_EQUAL("19.25", ch.getXDisplay());
	ch.keypresses("8.3056");
	ch.keypress("Convert_Angle_Degrees.Minutes-Seconds_Degrees");
	ch.keypresses(std::list<std::string>{"plus", "24", "remainder"});
	ch.keypress("Convert_Angle_Degrees_Degrees.Minutes-Seconds");
	CHECK_EQUAL("3.4556", ch.getXDisplay());
}

static void teneminus10test()
{
	CommandHandler ch;
	ch.setOption(EngNotation);
	ch.setOption(TrimZeroes);
	ch.setOption(AlwaysShowDecimal);
	ch.setOption(ThousandsSeparator);

	ch.keypress("1");
	ch.keypress("0");
	ch.keypress("exponent");
	ch.keypress("3");
	ch.keypress("plusMinus");
	CHECK_EQUAL("10e&ndash;3", ch.getXDisplay());
	ch.keypress("enter");
	CHECK_EQUAL("0.01", ch.getXDisplay());
	ch.keypress("clear");
	ch.keypress("clear");

	ch.keypress("1");
	ch.keypress("0");
	ch.keypress("exponent");
	ch.keypress("1");
	ch.keypress("0");
	ch.keypress("plusMinus");

	CHECK_EQUAL("10e&ndash;10", ch.getXDisplay());
	ch.keypress("enter");
	CHECK_EQUAL("1.0e&ndash;9", ch.getXDisplay());
	ch.keypress("clear");
	ch.keypress("clear");
}

static void leading_dot_test()
{
	CommandHandler ch;
	ch.setOption(EngNotation);
	ch.setOption(TrimZeroes);
	ch.setOption(AlwaysShowDecimal);
	ch.setOption(ThousandsSeparator);

	ch.keypresses("15");
	ch.keypress("enter");
	ch.keypresses("0.5");
	ch.keypress("times");
	ch.keypresses("0.5");
	ch.keypress("times");
	CHECK_EQUAL("3.75", ch.getXDisplay());
	ch.keypress("clear");
	ch.keypress("clear");

	ch.keypresses("15");
	ch.keypress("enter");
	ch.keypresses(".5");
	ch.keypress("times");
	ch.keypresses("0.5");
	ch.keypress("times");
	CHECK_EQUAL("3.75", ch.getXDisplay());
	ch.keypress("clear");
	ch.keypress("clear");

	ch.keypresses("15");
	ch.keypress("enter");
	ch.keypresses(".5");
	ch.keypress("times");
	ch.keypresses(".5");
	ch.keypress("times");
	CHECK_EQUAL("3.75", ch.getXDisplay());
	ch.keypress("clear");
	ch.keypress("clear");
}

/* -------------------------------------------------------------------- */
/* ConversionTest.kt                                                     */
/* -------------------------------------------------------------------- */

static std::string pythonCommand()
{
	const char *p = getenv("PYTHON");
	return (p != NULL && p[0] != '\0') ? std::string(p) : std::string("python3");
}

static void defaultConversionTests()
{
	const double ALLOWED_DELTA = 1e-5; // proportion of expected value
	Stack st;

	std::filesystem::create_directories("output/tests");
	// Export list of all supported conversions
	std::string unitListFileName = "output/tests/unit_export.txt";
	// Python script for test generation
	std::string pythonScriptFileName = "tests/generate_conversion_tests.py";
	// Output of python script containing list of tests
	std::string conversionTestFileName = "output/tests/conversion_tests.txt";

	{
		std::ofstream out(unitListFileName);
		for (std::string cat : st.getConversionCategories()) {
			if (cat == "Currency") {
				continue;
			}
			for (std::string unit : st.getAvailableUnits(cat)) {
				if (unit.starts_with("Degrees.") || unit.starts_with("Hours.")) { // Degrees.Minutes-Seconds or Degrees.Minutes
					continue;
				}
				if ((unit == "Date in Year") || (unit == "Day of Year")) {
					continue;
				}
				out << cat << "_" << unit << "\n";
			}
		}
	}

	// The Kotlin version assumes python-pint is installed: skip (rather than
	// fail) if it isn't, as it's an old version that is rarely available.
	std::string python = pythonCommand();
	if (std::system((python + " -c 'import pint' > /dev/null 2>&1").c_str()) != 0) {
		std::cerr << "SKIPPED " << currentTest << ": python-pint is not available to "
			<< python << " (set PYTHON to a python with pint installed)" << std::endl;
		skippedCount++;
		std::filesystem::remove(unitListFileName);
		return;
	}

	// Now run the python script
	std::filesystem::remove(conversionTestFileName);
	int result = std::system((python + " " + pythonScriptFileName
		+ " --unit-list " + unitListFileName
		+ " --out-file " + conversionTestFileName).c_str());
	CHECK_EQUAL_MSG("Running " + pythonScriptFileName, 0, result);

	// Now read the python script result back in
	// (Should be Category_InputValue_FromUnit_ToUnit_ExpectedValue one per line)
	std::ifstream in(conversionTestFileName);
	CHECK_MSG(in.good(), "Reading " + conversionTestFileName);
	std::string line;
	while (std::getline(in, line)) {
		std::vector<std::string> parts;
		std::stringstream ss(line);
		std::string part;
		while (std::getline(ss, part, '_')) {
			parts.push_back(part);
		}
		if (parts.size() != 5) {
			CHECK_MSG(false, "Badly formed line in " + conversionTestFileName + ": " + line);
			continue;
		}
		std::string cat = parts[0];
		double inputValue = std::strtod(parts[1].c_str(), NULL);
		std::string fromUnit = parts[2];
		std::string toUnit = parts[3];
		double expectedValue = std::strtod(parts[4].c_str(), NULL);

		// Now test all entries from the python script result
		st.push(AF(inputValue));
		CHECK_EQUAL_MSG("Conversion of " + parts[1] + " failed: " + fromUnit + " to " + toUnit,
			NoError,
			st.convert(cat, fromUnit, toUnit));
		double value = st.pop().toDouble();
		double DELTA = std::fabs(ALLOWED_DELTA * expectedValue);
		CHECK_CLOSE_MSG("Conversion of " + parts[1] + " from " + fromUnit + " to " + toUnit + " incorrect",
			expectedValue, value, DELTA);
	}

	std::filesystem::remove(unitListFileName);
	std::filesystem::remove(conversionTestFileName);
}

static void symbolPresenceTest()
{
	Stack st;
	// The Kotlin version iterates over conversionTable, which is private
	// here; getAvailableUnits() returns the "from" unit of every entry in a
	// category's table, so this checks the same set of units.
	for (std::string cat : st.getConversionCategories()) {
		for (std::string fromUnit : st.getAvailableUnits(cat)) {
			CHECK_MSG(st.unitSymbols.contains(fromUnit),
				"Unit symbol missing for " + fromUnit);
		}
	}
}

static void dateTests()
{
	Stack st;
	st.push(AF(std::string("11.102022")));

	// (0.2022*10000 in binary used to give the year as 2021)
	CHECK_EQUAL_MSG("Conversion to day of year failed",
		NoError,
		st.convert("Date", "Date in Year", "Day of Year"));
	checkTop(st, std::string("284.2022"), true);

	CHECK_EQUAL_MSG("Conversion to date of year failed",
		NoError,
		st.convert("Date", "Day of Year", "Date in Year"));

	checkTop(st, std::string("11.102022"));
}

static void conversionTableCompleteTest()
{
	Stack st;
	for (std::string cat : st.getConversionCategories()) {
		std::set<std::string> units = st.getAvailableUnits(cat);
		for (std::string fromUnit : units) {
			for (std::string toUnit : units) {
				if ((fromUnit == "Date in Year")) {
					st.push(AF(28.072022));
				}
				else {
					st.push(AF(1.0));
				}
				CHECK_EQUAL_MSG("Conversion failed: " + fromUnit + " to " + toUnit,
					NoError,
					st.convert(cat, fromUnit, toUnit));
				st.pop();
			}
		}
	}
}

/* -------------------------------------------------------------------- */
/* GridsTest.kt                                                          */
/* -------------------------------------------------------------------- */

static void simpleGridTest()
{
	CommandHandler ch;
	// All grids except SI Pad
	std::vector<std::string> gridList = {"numpad",
		"funcpad", "altfuncpad",
		"convpad", "constpad",
		"romanupperpad", "romanlowerpad",
		"greekupperpad", "greeklowerpad"
	};

	for (std::string gridname : gridList) {
		std::vector< std::vector<BI> > grid = ch.getGrid(gridname);
		// Just check the first entry is not NOP
		CHECK_MSG(( ! grid.empty()) && ( ! grid[0].empty()) && (grid[0][0].name != "NOP"),
			"First entry of " + gridname + " is not NOP");
	}
	std::vector< std::vector<BI> > sigrid = ch.getGrid("sipad");
	// Check second row for sipad
	CHECK_MSG((sigrid.size() > 1) && ( ! sigrid[1].empty()) && (sigrid[1][0].name != "NOP"),
		"First entry of second row of sipad is not NOP");
}

/* -------------------------------------------------------------------- */
/* OpsTest.kt                                                            */
/* -------------------------------------------------------------------- */

static void defaultOpTests()
{
	// Run tests with default options
	Stack st;

	// Most of these tests just check basic normal operation
	// rather than looking for faults with edge cases
	// Result values taken from my HP 33s calculator

	st.push(1.0);
	st.push(2.0);
	st.plus();
	checkTop(st, 3.0);

	st.push(5.0);
	st.push(3.0);
	st.minus();
	checkTop(st, 2.0);

	st.push(3.3);
	st.push(7.9);
	st.times();
	checkTop(st, 26.07);

	st.push(7.0);
	st.push(3.5);
	st.divide();
	checkTop(st, 2.0);

	st.push(7.0);
	st.push(0.0);
	CHECK(st.divide() == DivideByZero);
	checkTop(st, 0.0);
	checkTop(st, 7.0);

	st.push(3.0);
	st.push(2.65);
	st.xrooty();
	checkTop(st, 1.51372072297);

	st.push(3.0);
	st.invert();
	checkTop(st, -3.0);

	st.push(2.0);
	st.etox();
	checkTop(st, 7.38905609893, true);
	st.loge();
	checkTop(st, 2.0);

	st.push(2.0);
	st.tentox();
	checkTop(st, 100.0);

	st.push(70.0);
	st.push(35.0);
	st.percent();
	checkTop(st, 24.5);

	st.push(7.2);
	st.square();
	checkTop(st, 51.84);

	st.push(7.2);
	st.cube();
	checkTop(st, 373.248);

	st.push(8.0);
	st.reciprocal();
	checkTop(st, 0.125);

	st.push(8.875);
	st.integerpart();
	checkTop(st, 8.0);
	st.push(-8.875);
	st.integerpart();
	checkTop(st, -8.0);

	st.push(8.875);
	st.floatingpart();
	checkTop(st, 0.875);
	st.push(-8.875);
	st.floatingpart();
	checkTop(st, -0.875);

	st.push(7.5);
	st.push(3.2);
	st.integerdivide();
	checkTop(st, 2.0);

	st.setBitCount(bc32);
	st.push((double) 0x002378FF);
	st.push((double) 0x00727655);
	st.bitwiseand();
	checkTop(st, (double) 0x00227055);

	st.setBitCount(bc32);
	st.push((double) 0x002378FF);
	st.push((double) 0x00727655);
	st.bitwiseor();
	checkTop(st, (double) 0x00737EFF);

	st.setBitCount(bc32);
	st.push((double) 0x002378FF);
	st.push((double) 0x00727655);
	st.bitwisexor();
	checkTop(st, (double) 0x00510EAA);

	st.setBitCount(bc32);
	st.push((double) 0x002378FF);
	st.bitwisenot();
	checkTop(st, (double) 0xFFDC8700);

	st.setBitCount(bc16);
	st.push((double) 0x002378FF);
	st.push((double) 0x00727655);
	st.bitwiseand();
	checkTop(st, (double) 0x00007055);

	st.setBitCount(bc16);
	st.push((double) 0x002378FF);
	st.push((double) 0x00727655);
	st.bitwiseor();
	checkTop(st, (double) 0x00007EFF);

	st.setBitCount(bc16);
	st.push((double) 0x002378FF);
	st.push((double) 0x00727655);
	st.bitwisexor();
	checkTop(st, (double) 0x00000EAA);

	st.setBitCount(bc16);
	st.push((double) 0x002378FF);
	st.bitwisenot();
	checkTop(st, (double) 0x00008700);

	st.push(-8.0);
	st.absolute();
	checkTop(st, 8.0, true);
	st.absolute();
	checkTop(st, 8.0);

	st.push(1000.0);
	st.log10();
	checkTop(st, 3.0);

	/* loge tested with etox above */

	st.push(256.0);
	st.log2();
	checkTop(st, 8.0);

	st.push(3.5);
	st.ceiling();
	checkTop(st, 4.0);
	st.push(-3.5);
	st.ceiling();
	checkTop(st, -3.0);

	st.push(3.5);
	st.floor();
	checkTop(st, 3.0);
	st.push(-3.5);
	st.floor();
	checkTop(st, -4.0);

	st.push(60.0);
	st.sin();
	st.square();
	checkTop(st, 0.75, true);
	st.squareroot();
	st.inversesin();
	checkTop(st, 60.0);

	st.push(60.0);
	st.cos();
	checkTop(st, 0.5, true);
	st.inversecos();
	checkTop(st, 60.0);

	st.push(45.0);
	st.tan();
	checkTop(st, 1.0, true);
	st.inversetan();
	checkTop(st, 45.0);

	st.push(150.0);
	st.sin();
	st.push(5.0);
	st.times();
	st.push(150.0);
	st.cos();
	st.push(5.0);
	st.times();
	st.inversetan2();
	checkTop(st, 150.0);

	st.push(3.3);
	st.round();
	checkTop(st, 3.0);
	st.push(3.7);
	st.round();
	checkTop(st, 4.0);
	st.push(5.5);
	st.round();
	checkTop(st, 6.0);
	st.push(-3.3);
	st.round();
	checkTop(st, -3.0);
	st.push(-3.7);
	st.round();
	checkTop(st, -4.0);
	st.push(-5.5);
	st.round();
	checkTop(st, -6.0);

	st.push(49.0);
	st.squareroot();
	checkTop(st, 7.0);

	st.push(8.0);
	st.cuberoot();
	checkTop(st, 2.0);

	st.push(2.0);
	st.push(5.0);
	st.power();
	checkTop(st, 32.0);

	st.push(3.0);
	st.push(5.0);
	st.swap();
	checkTop(st, 3.0);
	checkTop(st, 5.0);

	st.push(7.3);
	st.push(5.4);
	st.remainder();
	checkTop(st, 1.9);

	st.push(15.3455222);
	st.convertHmsToHours();
	checkTop(st, 15.5820061111, true);
	st.convertHoursToHms();
	checkTop(st, 15.3455222);

	// TODO: Add test with HP for reference

	st.push(8.30);
	st.convertHmsToHours();
	checkTop(st, 8.5, true);
	st.convertHoursToHms();
	checkTop(st, 8.3);

	st.push(-15.3455222);
	st.convertHmsToHours();
	checkTop(st, -15.5820061111, true);
	st.convertHoursToHms();
	checkTop(st, -15.3455222);

	st.push(-8.30);
	st.convertHmsToHours();
	checkTop(st, -8.5, true);
	st.convertHoursToHms();
	checkTop(st, -8.3);

	st.constant("Pi");
	checkTop(st, M_PI);

	st.constant("Euler's Number");
	checkTop(st, M_E);

	st.push(5.0);
	st.convertGallonsToPints();
	checkTop(st, 40.0, true);
	st.convertPintsToLitres();
	checkTop(st, 22.7304594, true);
	st.convertLitresToPints();
	checkTop(st, 40.0, true);
	st.convertPintsToGallons();
	checkTop(st, 5.0, true);
	st.convertGallonsToPints();
	checkTop(st, 40.0, true);
	st.convert("Volume", "Pints", "Fluid Ounces");
	checkTop(st, 800.0, true);
	st.convert("Volume", "Fluid Ounces", "Pints");
	checkTop(st, 40.0);

	st.push(5.0);
	st.convert("Volume", "Gallons", "Fluid Ounces");
	checkTop(st, 800.0);
}

static void mathValueTest(double startValue, std::function<double(double)> mathFun,
		Stack &st, ErrorCode (Stack::*stackFun)(),
		const std::source_location &loc = std::source_location::current())
{
	const double ALLOWED_DELTA = 1e-9;
	double javaMathVal = mathFun(startValue);
	st.push(AF(startValue));
	(st.*stackFun)();
	double afValue = st.pop().toDouble();
	checkClose(javaMathVal, afValue, ALLOWED_DELTA, "mathValueTest", loc);
}

static void mathValueTests()
{
	Stack st;
	st.setOption(Radians, true);
	mathValueTest(10.8, [](double it) { return std::cos(it);   }, st, &Stack::cos);
	mathValueTest(10.8, [](double it) { return std::sin(it);   }, st, &Stack::sin);
	mathValueTest(10.8, [](double it) { return std::tan(it);   }, st, &Stack::tan);
	mathValueTest(10.8, [](double it) { return std::log(it);   }, st, &Stack::loge);
	mathValueTest(10.8, [](double it) { return std::log10(it); }, st, &Stack::log10);
	mathValueTest(10.8, [](double it) { return std::log(it)/std::log(2.0); }, st, &Stack::log2);
	mathValueTest(10.8, [](double it) { return it*it; }, st, &Stack::square);
	mathValueTest(10.8, [](double it) { return it*it*it; }, st, &Stack::cube);
	mathValueTest(10.8, [](double it) { return std::sqrt(it); }, st, &Stack::squareroot);
	mathValueTest(10.8, [](double it) { return std::cbrt(it); }, st, &Stack::cuberoot);
	mathValueTest(10.8, [](double it) { return 1.0/it; }, st, &Stack::reciprocal);
}

/* -------------------------------------------------------------------- */
/* SITest.kt                                                             */
/* -------------------------------------------------------------------- */

static void defaultSITests()
{
	// Run tests with default options
	CommandHandler calc;

	calc.st.push(5.2);
	calc.SI("k");
	checkTop(calc.st, 5200.0);

	calc.st.push(1.0);
	calc.SI("Mi");
	checkTop(calc.st, 1048576.0);

	// In order of size (they used to come back in alphabetical order)
	CHECK_EQUAL((std::vector<std::string>{"Ki", "Mi", "Gi", "Ti", "Pi", "Ei"}),
		calc.getBinaryPrefixSymbols());
	CHECK_EQUAL((std::vector<std::string>{"y", "z", "a", "f", "p", "n", "&mu;", "m", "k", "M", "G", "T", "P", "E", "Z", "Y"}),
		calc.getDecimalPrefixSymbols());
	CHECK_EQUAL((std::vector<std::string>{"Kibi", "Mebi", "Gibi", "Tebi", "Pebi", "Exbi"}),
		calc.getBinaryPrefixNames());
	CHECK_EQUAL((std::vector<std::string>{"Yocto", "Zepto", "Atto", "Femto", "Pico", "Nano", "Micro", "Milli", "Kilo", "Mega", "Giga", "Tera", "Peta", "Exa", "Zetta", "Yotta"}),
		calc.getDecimalPrefixNames());
}

/* -------------------------------------------------------------------- */
/* StackTest.kt                                                          */
/* -------------------------------------------------------------------- */

static bool check(AF a, double b)
{
	AF diff = a - AF(b);
	diff = diff.abs();
	return (diff < AF(1e-9));
}

static bool check(std::vector<AF> a, std::vector<double> b)
{
	bool result = true;
	if (a.size() != b.size()) {
		result = false;
	}
	else {
		for (size_t index=0;index<a.size();index++) {
			if ( ! check(a[index], b[index])) {
				result = false;
			}
		}
	}
	if ( ! result) {
		std::cerr << "Lists differ: (";
		for (size_t i=0;i<a.size();i++) {
			std::cerr << (i > 0 ? ", " : "") << a[i].toString();
		}
		std::cerr << ") vs (";
		for (size_t i=0;i<b.size();i++) {
			std::cerr << (i > 0 ? ", " : "") << describe(b[i]);
		}
		std::cerr << ")" << std::endl;
	}
	return result;
}

static void stackTests()
{
	const double ALLOWED_DELTA = 1e-9;

	// Stack(mutableSetOf(CalcOpt.SaveHistory)) in Kotlin
	Stack nrset;
	nrset.setOption(SaveHistory, true);

	CHECK(nrset.stack.size() == 0);
	CHECK( ! nrset.getOption(ReplicateStack));
	CHECK(nrset.getOption(SaveHistory));
	nrset.push(1.0);
	nrset.push(2.0);
	nrset.push(3.0);
	nrset.push(4.0);
	nrset.push(5.0);
	CHECK_CLOSE(5.0, nrset.peek(), ALLOWED_DELTA);
	CHECK_CLOSE(5.0, nrset.peekAt(0), ALLOWED_DELTA);
	CHECK_CLOSE(4.0, nrset.peekAt(1), ALLOWED_DELTA);
	CHECK_CLOSE(3.0, nrset.peekAt(2), ALLOWED_DELTA);
	CHECK_CLOSE(2.0, nrset.peekAt(3), ALLOWED_DELTA);
	CHECK_CLOSE(1.0, nrset.peekAt(4), ALLOWED_DELTA);
	CHECK(nrset.stack.size() == 5);
	CHECK_CLOSE(5.0, nrset.pop(), ALLOWED_DELTA);
	CHECK(nrset.stack.size() == 4);
	CHECK_CLOSE(4.0, nrset.peekAt(0), ALLOWED_DELTA);
	CHECK_CLOSE(3.0, nrset.peekAt(1), ALLOWED_DELTA);
	CHECK_CLOSE(2.0, nrset.peekAt(2), ALLOWED_DELTA);
	CHECK_CLOSE(1.0, nrset.peekAt(3), ALLOWED_DELTA);
	CHECK_CLOSE(0.0, nrset.peekAt(4), ALLOWED_DELTA);
	CHECK_CLOSE(4.0, nrset.pop(), ALLOWED_DELTA);
	CHECK(nrset.stack.size() == 3);
	CHECK_CLOSE(3.0, nrset.pop(), ALLOWED_DELTA);
	CHECK(nrset.stack.size() == 2);
	CHECK_CLOSE(2.0, nrset.pop(), ALLOWED_DELTA);
	CHECK(nrset.stack.size() == 1);
	CHECK_CLOSE(1.0, nrset.pop(), ALLOWED_DELTA);
	CHECK(nrset.stack.size() == 0);
	CHECK_CLOSE(0.0, nrset.pop(), ALLOWED_DELTA);

	pushList(nrset, {1.0, 2.0, 3.0, 4.0, 5.0});
	CHECK(nrset.stack.size() == 5);
	CHECK_CLOSE(5.0, nrset.peek(), ALLOWED_DELTA);
	CHECK_CLOSE(1.0, nrset.peekAt(4), ALLOWED_DELTA);
	nrset.rollUp();
	CHECK(check(nrset.stack, {5.0, 1.0, 2.0, 3.0, 4.0}));
	nrset.rollDown();
	CHECK(check(nrset.stack, {1.0, 2.0, 3.0, 4.0, 5.0}));
	/* The Kotlin version has some commented-out undo() tests here, which no
	 * longer work as the functionality of deciding when to save history has
	 * been moved into the Command layer.
	 */

	// Stack(mutableSetOf(CalcOpt.SaveHistory, CalcOpt.ReplicateStack)) in Kotlin
	Stack rst;
	rst.setOption(SaveHistory, true);
	rst.setOption(ReplicateStack, true);

	CHECK(rst.stack.size() == 0);
	CHECK(rst.getOption(ReplicateStack));
	rst.push(1.0);
	rst.push(2.0);
	rst.push(3.0);
	rst.push(4.0);
	rst.push(5.0);
	CHECK_CLOSE(5.0, rst.peek(), ALLOWED_DELTA);
	CHECK_CLOSE(5.0, rst.peekAt(0), ALLOWED_DELTA);
	CHECK_CLOSE(4.0, rst.peekAt(1), ALLOWED_DELTA);
	CHECK_CLOSE(3.0, rst.peekAt(2), ALLOWED_DELTA);
	CHECK_CLOSE(2.0, rst.peekAt(3), ALLOWED_DELTA);
	CHECK_CLOSE(0.0, rst.peekAt(4), ALLOWED_DELTA);
	CHECK(rst.stack.size() == 4);
	CHECK_CLOSE(5.0, rst.pop(), ALLOWED_DELTA);
	CHECK(rst.stack.size() == 4);
	CHECK_CLOSE(4.0, rst.peekAt(0), ALLOWED_DELTA);
	CHECK_CLOSE(3.0, rst.peekAt(1), ALLOWED_DELTA);
	CHECK_CLOSE(2.0, rst.peekAt(2), ALLOWED_DELTA);
	CHECK_CLOSE(2.0, rst.peekAt(3), ALLOWED_DELTA);
	CHECK_CLOSE(0.0, rst.peekAt(4), ALLOWED_DELTA);
	CHECK_CLOSE(4.0, rst.pop(), ALLOWED_DELTA);
	CHECK(rst.stack.size() == 4);
	CHECK_CLOSE(3.0, rst.pop(), ALLOWED_DELTA);
	CHECK(rst.stack.size() == 4);
	CHECK_CLOSE(2.0, rst.pop(), ALLOWED_DELTA);
	CHECK(rst.stack.size() == 4);
	CHECK_CLOSE(2.0, rst.pop(), ALLOWED_DELTA);
	CHECK(rst.stack.size() == 4);
	CHECK_CLOSE(2.0, rst.pop(), ALLOWED_DELTA);

	rst.clear();
	CHECK(rst.stack.size() == 0);

	pushList(rst, {1.0, 2.0, 3.0, 4.0, 5.0});
	CHECK(rst.stack.size() == 4);
	CHECK_CLOSE(5.0, rst.peek(), ALLOWED_DELTA);
	CHECK_CLOSE(2.0, rst.peekAt(3), ALLOWED_DELTA);
	CHECK(check(rst.stack, {2.0, 3.0, 4.0, 5.0}));
	rst.rollUp();
	CHECK(check(rst.stack, {5.0, 2.0, 3.0, 4.0}));
	rst.rollDown();
	CHECK(check(rst.stack, {2.0, 3.0, 4.0, 5.0}));
	/* Commented-out undo() tests in the Kotlin version: see above. */
}

/* -------------------------------------------------------------------- */
/* LayoutTest.kt (the keypad layout and the bit shifts)                  */
/* -------------------------------------------------------------------- */

static std::vector<std::string> names(const std::vector<BI> &row, size_t count = 6)
{
	std::vector<std::string> result;
	for (size_t i=0;(i<row.size()) && (i<count);i++) {
		result.push_back(row[i].name);
	}
	return result;
}

static void newLayout()
{
	CommandHandler ch;
	ch.setDefaultOptions();
	CHECK_EQUAL((std::vector<std::string>{"base", "exponent", "power", "sqrt", "reciprocal", "divide"}),
		names(ch.getGrid("numpad")[1]));
	ch.selectBase(baseHexadecimal);
	CHECK_EQUAL((std::vector<std::string>{"base", "NOP", "A", "B", "C", "divide"}),
		names(ch.getGrid("numpad")[1]));
	ch.selectBase(baseDecimal);
	for (std::string pad : {"funcpad", "altfuncpad"}) {
		auto grid = ch.getGrid(pad);
		CHECK_EQUAL((std::vector<std::string>{"cuberoot", "xrooty", "square"}), names(grid[2], 3));
		CHECK_EQUAL((std::vector<std::string>{"shiftleft", "shiftright", "cube"}), names(grid[3], 3));
	}
	// RAND is at the end of the fifth row of constants, in both layouts
	for (bool classic : {false, true}) {
		ch.setOption(ClassicLayout, classic);
		auto constants = ch.getGrid("constpad");
		CHECK_EQUAL(std::string("random"), constants[4][5].name);
		int count = 0;
		for (auto &row : constants) {
			for (auto &b : row) {
				count += (b.name == "random") ? 1 : 0;
			}
		}
		CHECK_EQUAL(1, count);
	}
}

static void classicLayout()
{
	CommandHandler ch;
	ch.setDefaultOptions();
	ch.setOption(ClassicLayout, true);
	CHECK_EQUAL((std::vector<std::string>{"base", "random", "exponent", "sqrt", "reciprocal", "divide"}),
		names(ch.getGrid("numpad")[1]));
	for (std::string pad : {"funcpad", "altfuncpad"}) {
		auto grid = ch.getGrid(pad);
		CHECK_EQUAL((std::vector<std::string>{"sqrt", "cuberoot", "xrooty"}), names(grid[2], 3));
		CHECK_EQUAL((std::vector<std::string>{"square", "cube", "power"}), names(grid[3], 3));
	}
	CHECK(ch.getOptions().contains("Classic Keypad Layout"));
}

static intmax_t shiftResult(const std::string &command, intmax_t x, int y, BitCount bits = bc32)
{
	CommandHandler ch;
	ch.setDefaultOptions();
	ch.setBitCount(bits);
	ch.st.push(AF(y));
	ch.st.push(AF(x));
	CHECK_EQUAL(NoError, ch.keypress(command));
	return ch.st.pop().toLong();
}

static void shifts()
{
	CHECK_EQUAL((intmax_t) 0x84CA, shiftResult("shiftleft", 0x4265, 1));
	CHECK_EQUAL((intmax_t) 0x2132, shiftResult("shiftright", 0x4265, 1));
	CHECK_EQUAL((intmax_t) 0x42650, shiftResult("shiftleft", 0x4265, 4));
	CHECK_EQUAL((intmax_t) 0x4265, shiftResult("shiftleft", 0x4265, 0));
	// Bits shifted beyond the bit count are lost
	CHECK_EQUAL((intmax_t) 0x2, shiftResult("shiftleft", 0x80000001LL, 1));
	CHECK_EQUAL((intmax_t) 0xCA, shiftResult("shiftleft", 0x4265, 1, bc8));
	CHECK_EQUAL((intmax_t) 0, shiftResult("shiftleft", 0x4265, 32));
	CHECK_EQUAL((intmax_t) 0, shiftResult("shiftleft", 0x4265, 100));
	CHECK_EQUAL((intmax_t) 0, shiftResult("shiftright", 0x4265, 100));
	// A logical shift: no sign extension
	CHECK_EQUAL((intmax_t) 0x7FFFFFFF, shiftResult("shiftright", 0xFFFFFFFFLL, 1));
	// Negative shifts go the other way
	CHECK_EQUAL((intmax_t) 0x2132, shiftResult("shiftleft", 0x4265, -1));
	CHECK_EQUAL((intmax_t) 0x84CA, shiftResult("shiftright", 0x4265, -1));

	// As typed in hex
	CommandHandler ch;
	ch.setDefaultOptions();
	ch.selectBase(baseHexadecimal);
	for (std::string k : {"1", "enter", "4", "2", "6", "5", "shiftleft"}) {
		ch.keypress(k);
	}
	CHECK_EQUAL(std::string("0x84CA"), ch.getXDisplay());
}

/* -------------------------------------------------------------------- */

typedef struct _TestCase {
	std::string name;
	void (*function)();
} TestCase;

#define TEST(x) {#x, x}
static const std::vector<TestCase> allTests = {
	// CommandsTest.kt
	TEST(defaultCommandTests),
	TEST(dmsConversionTest),
	TEST(teneminus10test),
	TEST(leading_dot_test),
	// ConversionTest.kt
	TEST(defaultConversionTests),
	TEST(symbolPresenceTest),
	TEST(dateTests),
	TEST(conversionTableCompleteTest),
	// GridsTest.kt
	TEST(simpleGridTest),
	// OpsTest.kt
	TEST(defaultOpTests),
	TEST(mathValueTests),
	// SITest.kt
	TEST(defaultSITests),
	// StackTest.kt
	TEST(stackTests),
	// LayoutTest.kt
	TEST(newLayout),
	TEST(classicLayout),
	TEST(shifts),
};

int main(int argc, char **argv)
{
	std::set<std::string> selected;
	for (int i=1;i<argc;i++) {
		selected.insert(argv[i]);
	}
	std::set<std::string> unknown = selected;

	int testCount = 0;
	int failedTests = 0;
	for (const TestCase &t : allTests) {
		if (( ! selected.empty()) && ( ! selected.contains(t.name))) {
			continue;
		}
		unknown.erase(t.name);
		currentTest = t.name;
		int failuresBefore = failureCount;
		int checksBefore = checkCount;
		t.function();
		testCount++;
		bool passed = (failureCount == failuresBefore);
		if ( ! passed) {
			failedTests++;
		}
		std::cout << (passed ? "PASS " : "FAIL ") << t.name
			<< " (" << (checkCount - checksBefore) << " checks, "
			<< (failureCount - failuresBefore) << " failed)" << std::endl;
	}

	for (const std::string &name : unknown) {
		std::cerr << "Unknown test: " << name << std::endl;
		failedTests++;
	}

	std::cout << std::endl << testCount << " test(s), " << failedTests << " failed; "
		<< checkCount << " check(s), " << failureCount << " failed";
	if (skippedCount > 0) {
		std::cout << "; " << skippedCount << " test(s) skipped";
	}
	std::cout << std::endl;

	return (failedTests == 0) ? 0 : 1;
}
