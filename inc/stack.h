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
#ifndef STACK_H
#define STACK_H

#include <stddef.h>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <random>
#include "arpfloat.h"

typedef enum _CalcOpt {
	ReplicateStack, // Use an X, Y, Z, T replicating stack
	Radians,        // Use radians instead of degrees
	SaveHistory,    // Save the stack history (memory intensive)
	PercentLeavesY, // Leave Y in place when calculating percentage
	// If adding any here, make sure they're added to getCalcOptionByName
} CalcOpt;

#define NAME(x) #x, x
const std::map<std::string, CalcOpt> CalcOptNames = {
	{NAME(ReplicateStack)},
	{NAME(Radians)},
	{NAME(SaveHistory)},
	{NAME(PercentLeavesY)},
};

typedef enum _ErrorCode {
	NoError,
	DivideByZero,
	InvalidRoot,
	InvalidLog,
	InvalidTan,
	InvalidInverseTrig,
	InvalidInverseHypTrig,
	UnknownConstant,
	UnknownConversion,
	InvalidConversion,
	UnknownSI,
	NoFunction,
	NoHistorySaved,
	NotImplemented,
} ErrorCode;

typedef enum _BitCount {
	bc8,
	bc16,
	bc32,
	bc64,
} BitCount;

typedef struct _Constant {
	std::string name;
	std::string symbol;
	AF value;
	std::string unit;
	std::string category;
} Constant;

// Forward definition for the typedef
class Stack;
typedef ErrorCode (Stack::*CustomConversionFunction)();

typedef struct _Conversion {
	std::string from;
	std::string to;
	CustomConversionFunction ConvFunc; // NULL if just a multiplier
	double multiplier; // use convertMultiplier instead of a custom function
} Conversion;

class Stack
{
	// Need to include everything from Stack, Ops and Conversion!
	public:
		// Stack.kt:
		Stack();

		std::vector<AF> getStackForDisplay();

		void setOption(CalcOpt o, bool v);
		bool getOption(CalcOpt o);
		const std::map<std::string, CalcOpt> getOptionNameMap();

		intmax_t getBitMask();

		void saveHistory();

		void clear();

		void push(AF v);

		void push(std::vector<AF> vs);

		AF pop();
		AF peek();
		AF peekAt(int index);

		ErrorCode rollUp();
		ErrorCode rollDown();

		ErrorCode undo();


		// Ops.kt
		ErrorCode random();
		ErrorCode plus();
		ErrorCode minus();
		ErrorCode times();
		ErrorCode divide();
		ErrorCode xrooty();
		ErrorCode invert();
		ErrorCode etox();
		ErrorCode tentox();
		ErrorCode twotox();
		ErrorCode percent();
		ErrorCode percentchange();
		ErrorCode square();
		ErrorCode cube();
		ErrorCode reciprocal();
		ErrorCode integerpart();
		ErrorCode floatingpart();
		ErrorCode integerdivide();
		ErrorCode bitwiseand();
		ErrorCode bitwiseor();
		ErrorCode bitwisexor();
		ErrorCode bitwisenot();
		ErrorCode absolute();
		ErrorCode log10();
		ErrorCode loge();
		ErrorCode log2();
		ErrorCode ceiling();
		ErrorCode floor();
		ErrorCode cos();
		ErrorCode sin();
		ErrorCode tan();
		ErrorCode inversecos();
		ErrorCode inversesin();
		ErrorCode inversetan();
		ErrorCode inversetan2();
		ErrorCode cosh();
		ErrorCode sinh();
		ErrorCode tanh();
		ErrorCode inversecosh();
		ErrorCode inversesinh();
		ErrorCode inversetanh();
		ErrorCode inversetanh2();
		ErrorCode round();
		ErrorCode squareroot();
		ErrorCode cuberoot();
		ErrorCode power();
		ErrorCode swap();
		ErrorCode duplicate();
		ErrorCode remainder();
		ErrorCode drop();
		ErrorCode convertHmsToHours();
		ErrorCode convertHmToHours();
		ErrorCode convertHoursToHms();
		ErrorCode convertHoursToHm();
		void populateConstants();
		ErrorCode constant(std::string name);

		// Conversion.kt
		ErrorCode convertKelvinToCelsius();
		ErrorCode convertCelsiusToKelvin();
		ErrorCode convertFahrenheitToCelsius();
		ErrorCode convertCelsiusToFahrenheit();
		ErrorCode convertPintsToLitres(bool reverse=false, bool american=false);
		ErrorCode convertGallonsToPints();
		ErrorCode convertLitresToPints(bool american=false);
		ErrorCode convertGallonsToLitres();
		ErrorCode convertPintsToGallons();
		ErrorCode convertLitresToGallons();
		ErrorCode convertInchToMM();
		ErrorCode convertMMToInch();
		ErrorCode convertRadiansToDegrees();
		ErrorCode convertDegreesToRadians();
		ErrorCode convertMilesToKilometres();
		ErrorCode convertKilometresToMiles();
		ErrorCode convertRPMToHertz();
		ErrorCode convertHertzToRPM();
		ErrorCode convertOuncesToGrams();
		ErrorCode convertGramsToOunces();
		ErrorCode convertNewtonMetresToPoundsFeet();
		ErrorCode convertPoundsFeetToNewtonMetres();
		ErrorCode convertKilogramsToPounds();
		ErrorCode convertPoundsToKilograms();
		ErrorCode convertKilogramsToStone();
		ErrorCode convertStoneToKilograms();
		ErrorCode convertGramsToKilograms();
		ErrorCode convertKilogramsToGrams();
		ErrorCode convertRadPerSecToHertz();
		ErrorCode convertHertzToRadPerSec();
		ErrorCode convertKilometresPerLitreToLitresPer100KM();
		ErrorCode convertLitresPer100KMToKilometresPerLitre();

		ErrorCode convertMultiplier(double mult);
		ErrorCode convertMultiplier(AF mult);
		void populateConversionTable();
		void registerCurrencies(std::map<std::string, double> wrtEuro);
		bool seenConv(std::vector<Conversion> path, std::string to);
		std::vector<Conversion> getConvPathsFrom(std::string type, std::string from, std::vector<Conversion> pathSoFar);
		std::vector<Conversion> getConvPathsFromTo(std::string type, std::string from, std::string to, std::vector<Conversion> pathSoFar = std::vector<Conversion>());
		ErrorCode convert(std::string type, std::string to,  std::string from);
		std::set<std::string> getAvailableConversions(std::string from) ;
		std::vector<std::string> getConversionCategories();
		std::set<std::string> getAvailableUnits(std::string category);

		void setBitCount(BitCount b);
		BitCount getBitCount();

		void debugStackPrint();

		std::vector<Constant> getConstants();
		std::map<std::string, double> getRawCurrencyData();

		/* Things that were done in a lambda in Kotlin: */
		ErrorCode returnNoError();
		ErrorCode convertUKPintsToLitres();
		ErrorCode convertLitresToUKPints();
		ErrorCode convertUSPintsToLitres();
		ErrorCode convertLitresToUSPints();
		ErrorCode convertMPHToKMH();
		ErrorCode convertKMHToMPH();
		ErrorCode convertMPGToMPL();
		ErrorCode convertMPLToMPG();
		ErrorCode convertMPGToUSMPG();
		ErrorCode convertUSMPGToMPG();

		ErrorCode convertKMPLToMPL();
		ErrorCode convertMPLToKMPL();

		ErrorCode convertDateInCurrentYearToDayOfYear();
		ErrorCode convertDayOfYearToDateInCurrentYear();

		std::map<std::string, std::string> unitSymbols;
		std::vector<AF> stack;
		std::vector<Constant> constants;

		void printHistory();
		void printThisStack(std::vector<AF> s);
	private:
		std::list< std::vector< AF > > history;
		std::map<std::string, std::vector<Conversion> > conversionTable;
		std::map<std::string, std::string> currencyMap;
		std::map<std::string, double> rawCurrencyData;
		std::set<CalcOpt> options;
		BitCount bitCount = bc32;
		const uintmax_t MAX_HIST = 50;

		std::mt19937 rng;  // the Mersenne Twister with a popular choice of parameters
};

#endif
