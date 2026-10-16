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
 * Arithmetic in hours/degrees, minutes and seconds (H.MMSS / D.MMSS) and
 * degrees and minutes (D.MM), as it's done on the calculator: type a value,
 * convert it to hours (or degrees), type another and convert that, add or
 * subtract, then convert back.
 *
 * The minutes and seconds typed in can be 60 or more (12.8622 is 12 hours,
 * 86 minutes and 22 seconds, i.e. 13:26:22), and the seconds (or for D.MM
 * the minutes) can have two decimal places.  The expected answer is worked
 * out separately, in whole hundredths of a second (or of a minute), and
 * both the result's value and how it's displayed are checked against it.
 *
 * The same tests are in the Android version's app/src/test/HmsTest.kt.
 *
 * Usage: hms_tests [random cases for each kind] (default 2000); see
 * run_tests.sh.  Exits non-zero if any case fails.
 */
#include <cstdlib>
#include <iostream>
#include <random>
#include <string>
#include <vector>
#include "commands.h"

// One of the formats: the value is a whole number of units (hundredths of a
// second or of a minute), written as whole.MMSScc or whole.MMcc
struct Kind
{
	std::string name;
	std::string toDecimal;
	std::string fromDecimal;
	// How many of the previous field make one of this one, from the whole
	// number downwards, e.g. {60, 60, 100} for H.MMSScc
	std::vector<int> fields;
};

struct Value
{
	long long whole;
	std::vector<int> parts; // Any left off (e.g. the hundredths) are zero
};

struct Case
{
	const Kind *kind;
	Value a;
	std::string op; // "", "plus" or "minus"
	Value b;
};

static std::vector<int> padded(const Kind &kind, std::vector<int> parts)
{
	parts.resize(kind.fields.size(), 0);
	return parts;
}

// What would be typed in for a value (the whole number, then each field as
// two digits, with trailing zeros left off as a person would)
static std::string typed(const Kind &kind, const Value &v)
{
	std::string fraction;
	for (int p : padded(kind, v.parts)) {
		fraction += std::to_string(p / 10) + std::to_string(p % 10);
	}
	while (( ! fraction.empty()) && (fraction.back() == '0')) {
		fraction.pop_back();
	}
	std::string result = std::to_string(v.whole);
	if ( ! fraction.empty()) {
		result += "." + fraction;
	}
	return result;
}

static long long units(const Kind &kind, const Value &v)
{
	long long result = v.whole;
	std::vector<int> parts = padded(kind, v.parts);
	for (size_t i=0;i<parts.size();i++) {
		result = (result * kind.fields[i]) + parts[i];
	}
	return result;
}

// The expected result: the units written out with each field in range
// (minutes and seconds below 60)
static std::string expected(const Kind &kind, long long u)
{
	std::string sign = (u < 0) ? "-" : "";
	long long rest = (u < 0) ? -u : u;
	Value v;
	v.parts.resize(kind.fields.size());
	for (int i=(int) kind.fields.size()-1;i>=0;i--) {
		v.parts[i] = (int) (rest % kind.fields[i]);
		rest /= kind.fields[i];
	}
	v.whole = rest;
	return sign + typed(kind, v);
}

// Returns a description of what went wrong, or "" if it worked
static std::string check(const Case &c)
{
	const Kind &kind = *c.kind;
	CommandHandler ch;
	std::string what = typed(kind, c.a);
	long long result = units(kind, c.a);
	if ( ! c.op.empty()) {
		what += " " + c.op + " " + typed(kind, c.b);
		result += ((c.op == "plus") ? 1 : -1) * units(kind, c.b);
	}
	std::string expectedText = expected(kind, result);

	std::vector<std::string> keys;
	for (char k : typed(kind, c.a)) {
		keys.push_back(std::string(1, k));
	}
	keys.push_back(kind.toDecimal);
	if ( ! c.op.empty()) {
		for (char k : typed(kind, c.b)) {
			keys.push_back(std::string(1, k));
		}
		keys.push_back(kind.toDecimal);
		keys.push_back(c.op);
	}
	keys.push_back(kind.fromDecimal);
	for (const std::string &k : keys) {
		ErrorCode ec = ch.keypress(k);
		if (ec != NoError) {
			return kind.name + " " + what + ": " + k + " gave error " + std::to_string((int) ec);
		}
	}

	AF actual = ch.st.peek();
	std::string display = ch.getXDisplay();

	// How the expected value is displayed (with the same options)
	CommandHandler reference;
	reference.st.push(AF(expectedText));
	std::string expectedDisplay = reference.getXDisplay();

	if ((actual - AF(expectedText)).abs() > AF("1e-9")) {
		return kind.name + " " + what + ": expected " + expectedText + ", got " + actual.toString();
	}
	if (display != expectedDisplay) {
		return kind.name + " " + what + ": expected display " + expectedDisplay
			+ ", got " + display + " (value " + actual.toString() + ")";
	}
	return "";
}

static int run(const std::string &name, const std::vector<Case> &cases)
{
	int failures = 0;
	for (const Case &c : cases) {
		std::string failure = check(c);
		if ( ! failure.empty()) {
			failures++;
			if (failures <= 30) {
				std::cerr << "  " << failure << std::endl;
			}
		}
	}
	std::cout << ((failures == 0) ? "PASS " : "FAIL ") << name << " ("
		<< cases.size() << " cases, " << failures << " failed)" << std::endl;
	return failures;
}

int main(int argc, char **argv)
{
	int count = (argc > 1) ? atoi(argv[1]) : 2000;

	const Kind hms = {"H.MS", "Convert_Time_Hours.Minutes-Seconds_Hours",
		"Convert_Time_Hours_Hours.Minutes-Seconds", {60, 60, 100}};
	const Kind dms = {"D.MS", "Convert_Angle_Degrees.Minutes-Seconds_Degrees",
		"Convert_Angle_Degrees_Degrees.Minutes-Seconds", {60, 60, 100}};
	const Kind dm = {"D.M", "Convert_Angle_Degrees.Minutes_Degrees",
		"Convert_Angle_Degrees_Degrees.Minutes", {60, 100}};

	std::vector<Case> handPicked;
	for (const Kind *k : {&hms, &dms}) {
		// Out of range minutes and seconds, just converted and back
		for (const Value &x : std::vector<Value>{{12, {86, 22}}, {0, {0, 60}}, {0, {60, 0}},
				{0, {59, 60}}, {23, {59, 59}}, {0, {99, 99}}, {1, {15}}, {19, {15}},
				{2, {15}}, {0, {0, 1}}, {0, {0, 0, 1}}, {10, {10, 10, 10}},
				{0, {59, 59, 99}}, {359, {59, 59, 99}}, {999, {99, 99, 99}}}) {
			handPicked.push_back({k, x, "", {}});
		}
		for (const std::string op : {"plus", "minus"}) {
			for (const auto &[x, y] : std::vector<std::pair<Value, Value> >{
					{{12, {86, 22}}, {1, {33, 38}}},
					{{23, {59, 59}}, {0, {0, 1}}},
					{{0, {0, 1}}, {0, {0, 2}}},
					{{0, {30}}, {1, {0}}},
					{{10, {0}}, {9, {59, 59, 99}}},
					{{1, {15}}, {2, {15}}},
					{{19, {15}}, {8, {30, 56}}},
					{{0, {0, 59, 99}}, {0, {0, 0, 1}}},
					{{5, {}}, {5, {}}},
					{{12, {34, 56, 78}}, {98, {76, 54, 32}}}}) {
				handPicked.push_back({k, x, op, y});
			}
		}
	}
	for (const Value &x : std::vector<Value>{{51, {30, 45}}, {0, {99, 99}}, {12, {60}},
			{0, {59, 99}}, {1, {15}}}) {
		handPicked.push_back({&dm, x, "", {}});
	}
	for (const std::string op : {"plus", "minus"}) {
		for (const auto &[x, y] : std::vector<std::pair<Value, Value> >{
				{{51, {30, 45}}, {0, {29, 55}}}, {{0, {0, 1}}, {0, {0, 2}}},
				{{10, {0}}, {9, {59, 99}}}}) {
			handPicked.push_back({&dm, x, op, y});
		}
	}

	std::vector<Case> random;
	std::mt19937 rng(1);
	for (const Kind *k : {&hms, &dms, &dm}) {
		// Mostly small numbers of hours, with some large ones; minutes and
		// seconds mostly in range, sometimes up to 99; often no hundredths
		auto value = [&]() {
			Value v;
			v.whole = ((rng() % 4) == 0) ? (rng() % 10000) : (rng() % 48);
			for (size_t i=0;i<k->fields.size();i++) {
				bool last = (i == (k->fields.size() - 1));
				if (last && ((rng() % 2) == 0)) {
					v.parts.push_back(0);
				}
				else {
					v.parts.push_back((last || ((rng() % 5) == 0)) ? (rng() % 100) : (rng() % 60));
				}
			}
			return v;
		};
		for (int i=0;i<count;i++) {
			const char *ops[] = {"", "plus", "minus"};
			std::string op = ops[i % 3];
			Value a = value();
			Value b = op.empty() ? Value() : value();
			random.push_back({k, a, op, b});
		}
	}

	int failures = run("handPicked", handPicked) + run("random", random);
	return (failures == 0) ? 0 : 1;
}
