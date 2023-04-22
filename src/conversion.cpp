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
#include <regex>
#include <iostream>
#include <random>
#include <chrono>

#include "stack.h"

ErrorCode Stack::convertKelvinToCelsius()
{
	push(pop() - 273.15);
	return NoError;
}

ErrorCode Stack::convertCelsiusToKelvin()
{
	push(pop() + 273.15);
	return NoError;
}

ErrorCode Stack::convertFahrenheitToCelsius()
{
	push((pop()-32.0)*5.0/9.0);
	return NoError;
}

ErrorCode Stack::convertCelsiusToFahrenheit()
{
	push((pop()*9.0/5.0)+32.0);
	return NoError;
}

ErrorCode Stack::convertPintsToLitres(bool reverse, bool american)
{
	double multiplier;
	if (american) {
		multiplier = 0.47317648;
	}
	else {
		multiplier = 0.568261485;
	}

	if (reverse) {
		multiplier = 1.0/multiplier;
	}
	push(pop()*multiplier);
	return NoError;
}

ErrorCode Stack::convertGallonsToPints()
{
	return convertMultiplier(8.0);
}

ErrorCode Stack::convertLitresToPints(bool american)
{
	return convertPintsToLitres(true, american);
}

ErrorCode Stack::convertGallonsToLitres()
{
	convertGallonsToPints();
	return convertPintsToLitres();
}

ErrorCode Stack::convertPintsToGallons()
{
	return convertMultiplier(1.0/8.0);
}

ErrorCode Stack::convertLitresToGallons()
{
	convertPintsToLitres(true);
	return convertPintsToGallons();
}

ErrorCode Stack::convertInchToMM()
{
	return convertMultiplier(25.4);
}

ErrorCode Stack::convertMMToInch()
{
	return convertMultiplier(1.0/25.4);
}

ErrorCode Stack::convertRadiansToDegrees()
{
	return convertMultiplier(AF(180)/AF::pi());
}

ErrorCode Stack::convertDegreesToRadians()
{
	return convertMultiplier(AF::pi()/180);
}

ErrorCode Stack::convertMilesToKilometres()
{
	return convertMultiplier(1.609344);
}

ErrorCode Stack::convertKilometresToMiles()
{
	return convertMultiplier(1.0/1.609344);
}

ErrorCode Stack::convertRPMToHertz()
{
	return convertMultiplier(1.0/60.0);
}

ErrorCode Stack::convertHertzToRPM()
{
	return convertMultiplier(60.0);
}

ErrorCode Stack::convertOuncesToGrams()
{
	return convertMultiplier(28.3495231);
}

ErrorCode Stack::convertGramsToOunces()
{
	return convertMultiplier(1.0/28.3495231);
}

ErrorCode Stack::convertNewtonMetresToPoundsFeet() {
	return convertMultiplier(0.737562149277);
}
ErrorCode Stack::convertPoundsFeetToNewtonMetres() {
	return convertMultiplier(1.0/0.737562149277);
}
ErrorCode Stack::convertKilogramsToPounds() {
	return convertMultiplier(1/0.45359237);
}
ErrorCode Stack::convertPoundsToKilograms() {
	return convertMultiplier(0.45359237);
}
ErrorCode Stack::convertKilogramsToStone() {
	return convertMultiplier(2.2046228/14.0);
}
ErrorCode Stack::convertStoneToKilograms() {
	return convertMultiplier(14.0/2.2046228);
}
ErrorCode Stack::convertGramsToKilograms() {
	return convertMultiplier(1.0/1000.0);
}
ErrorCode Stack::convertKilogramsToGrams() {
	return convertMultiplier(1000.0);
}
ErrorCode Stack::convertRadPerSecToHertz() {
	return convertMultiplier(AF(1.0)/(AF(2.0)*AF::pi()));
}
ErrorCode Stack::convertHertzToRadPerSec() {
	return convertMultiplier(AF(2.0)*AF::pi());
}

ErrorCode Stack::convertKilometresPerLitreToLitresPer100KM() {
	AF kmpl = pop();
	if (kmpl == AF(0.0)) {
		push(kmpl);
		return DivideByZero;
	}
	AF lpkm = AF(1.0) / kmpl;
	AF lp100km = AF(100.0) * lpkm;
	push(lp100km);
	return NoError;
}

ErrorCode Stack::convertLitresPer100KMToKilometresPerLitre() {
	AF lp100km = pop();
	if (lp100km == AF(0.0)) {
		push(lp100km);
		return DivideByZero;
	}
	AF lpkm = lp100km / AF(100.0);
	AF kmpl = AF(1.0) / lpkm;
	push(kmpl);
	return NoError;
}

ErrorCode Stack::convertDayOfYearToDateInCurrentYear() {
	AF dayofyear = pop();
	if ((dayofyear < 0) || (dayofyear > 366)) {
		push(dayofyear);
		return InvalidConversion;
	}

	int dayofyear_i = dayofyear.floor().toInt();
	AF fp = dayofyear - dayofyear.floor();
	fp = fp * AF("10000");
	int year_i = fp.round().toInt();

	if (year_i < 1900) {
		auto now = std::chrono::system_clock::now();
		std::time_t tt = std::chrono::system_clock::to_time_t(now);
		struct std::tm *parts = std::localtime(&tt);
		year_i = parts->tm_year;
	}

	int num = dayofyear_i;
	// correct non-leap years
	if ( ( ! std::chrono::year{year_i}.is_leap()) && (num > 59)) { // day 59 is 28th of February
		num += 1;
	}

	static const std::vector<int> days_in_month = {
		31, 29, 31, 30, 31, 30,
		31, 31, 30, 31, 30, 31
	};

	for (int m = 0; m < 12; m++) {
		int dm = days_in_month[m];
		if (num <= dm) {
			AF result(year_i);
			result = result / AF("10000");
			result = result + AF(m + 1);
			result = result / AF("100");
			result = result + AF(num);
			push(result);
			return NoError;
		} else {
			num -= dm;
		}
	}

	// Shouldn't ever get here, but if we did, then something's gone wrong
	push(dayofyear);
	return InvalidConversion;
}

ErrorCode Stack::convertDateInCurrentYearToDayOfYear() {
	AF xval = pop();

	AF date_form = xval;

	int dom = date_form.floor().toInt();
	date_form = date_form - date_form.floor();
	date_form = date_form * AF("100");
	int m = date_form.floor().toInt();
	date_form = date_form - date_form.floor();
	date_form = date_form * AF("10000");
	int year_i = date_form.floor().toInt();
	if (year_i < 1900) {
		auto now = std::chrono::system_clock::now();
		std::time_t tt = std::chrono::system_clock::to_time_t(now);
		struct std::tm *parts = std::localtime(&tt);
		year_i = parts->tm_year + 1900;
	}

	static const std::vector<int> days_in_month = {
		31, 29, 31, 30, 31, 30,
		31, 31, 30, 31, 30, 31
	};

	int dayofyear = 0;
	for (int im = 0; im < (m-1); im++) {
		dayofyear += days_in_month[im];
	}
	dayofyear += dom;

	if ( ( ! std::chrono::year{year_i}.is_leap()) && (dayofyear > 59)) { // day 59 is 28th of February
		dayofyear -= 1;
	}

	AF result(dayofyear);
	result = result + (AF(year_i) / AF("10000"));

	push(result);

	return NoError;
}


ErrorCode Stack::convertMultiplier(double mult) {
	push(pop()*mult);
	return NoError;
}

ErrorCode Stack::convertMultiplier(AF mult) {
	push(pop()*mult);
	return NoError;
}

void Stack::populateConversionTable()
{
	std::initializer_list<std::pair<const std::string, std::string> > unitTableInit = {
		{"Acres", "ac"},
		{"Angstroms", "&Aring;"},
		{"Atmosphere", "atm"},
		{"Bar", "bar"},
		{"Bytes", "B"},
		{"Calories", "cal"},
		{"Celsius", "&deg;C"},
		{"Centimetres", "cm"},
		{"Chains", "ch"},
		{"Cubic Centimetres", "cc"},
		{"Cubic Decimetres", "dm<sup><small>3</small></sup>"},
		{"Cubic Feet", "cu ft"},
		{"Cubic Inches", "cu in"},
		{"Cubic Metres", "m<sup><small>3</small></sup>"},
		{"Cubic Millimetres", "mm<sup><small>3</small></sup>"},
		{"Cubic Yards", "cu yd"},
		{"Days", "days"},
		{"Day of Year", "N.(Y)"},
		{"Date in Year", "D.M(Y)"},
		{"Degrees", "deg"},
		{"Degrees.Minutes", "D.M"},
		{"Degrees.Minutes-Seconds", "DMS"},
		{"Fahrenheit", "&deg;F"},
		{"Fathoms", "fm"},
		{"Feet Per Second", "ft/s"},
		{"Feet", "ft"},
		{"Fluid Ounces", "fl.oz"},
		{"Furlongs", "furlongs"},
		{"Gallons", "gal"},
		{"Gibibytes", "GiB"},
		{"Gigabytes", "GB"},
		{"Grams", "g"},
		{"Gram-Force", "gf"},
		{"Gram-Force Centimetres", "gf cm"},
		{"Gram-Force Millimetres", "gf mm"},
		{"Gram-Force Metres", "gf m"},
		{"Hectares", "ha"},
		{"Hectopascal", "hPa"},
		{"Hertz", "Hz"},
		{"Horsepower (Mech)", "hp<sub><small>mech</small></sub>"},
		{"Horsepower (Metric)", "hp<sub><small>met</small></sub>"},
		{"Hours", "hr"},
		{"Hours.Minutes-Seconds", "HMS"},
		{"Hundredweight", "cwt"},
		{"Inches of Mercury", "in Hg"},
		{"Inches", "in"},
		{"Joules", "J"},
		{"Kelvin", "K"},
		{"Kibibytes", "KiB"},
		{"Kilobytes", "kB"},
		{"Kilocalories", "kcal"},
		{"Kilogram-Force", "kgf"},
		{"Kilograms Per Sq. cm", "kg/cm<sup><small>2</small></sup>"},
		{"Kilograms", "kg"},
		{"Kilogram-Force Centimetres", "kgf cm"},
		{"Kilogram-Force Millimetres", "kgf mm"},
		{"Kilogram-Force Metres", "kgf m"},
		{"Kilojoules", "kJ"},
		{"Kilometres Per Hour", "km/h"},
		{"Kilometres Per Litre", "km/l"},
		{"Kilometres", "km"},
		{"Kilonewtons", "kN"},
		{"Kilopascal", "kPa"},
		{"Kilowatt-Hours", "kWh"},
		{"Kilowatts", "kW"},
		{"Knots", "kt"},
		{"Light Years", "ly"},
		{"Litres Per 100 Kilometres", "l/100 km"},
		{"Litres", "l"},
		{"Mebibytes", "MiB"},
		{"Megabytes", "MB"},
		{"Megajoules", "MJ"},
		{"Megapascal", "MPa"},
		{"Metres Per Second", "m/s"},
		{"Metres Per Hour", "m/hr"},
		{"Metres", "m"},
		{"Microgram", "&mu;g"},
		{"Micrometres", "&mu;m"},
		{"Micronewtons", "&mu;N"},
		{"Microns", "&mu;m"},
		{"Microseconds", "&mu;s"},
		{"Miles Per Gallon", "mpg"},
		{"Miles Per US Gallon", "mpg<sub><small>US</small></sub>"},
		{"Miles Per Litre", "mpl"},
		{"Miles Per Hour", "mph"},
		{"Miles", "miles"},
		{"Millibar", "mbar"},
		{"Milligrams", "mg"},
		{"Millilitres", "ml"},
		{"Millimetres", "mm"},
		{"Millinewtons", "mN"},
		{"Milliseconds", "ms"},
		{"Mils", "mil"},
		{"Minutes", "min"},
		{"Nanometres", "nm"},
		{"Nanoseconds", "ns"},
		{"Nautical Miles", "nm"},
		{"Newton Centimetres", "N cm"},
		{"Newton Millimetres", "N mm"},
		{"Newton Metres", "Nm"},
		{"Newtons", "N"},
		{"Ounces", "oz"},
		{"Ounce-Force", "ozf"},
		{"Ounce-Force Feet", "ozf-ft"},
		{"Ounce-Force Inches", "ozf-in"},
		{"Pascal", "Pa"},
		{"Pints", "pt"},
		{"Points", "pt"},
		{"Pound-Force", "lb<sub><small>F</small></sub>"},
		{"Pound-Force Feet", "lbf ft"},
		{"Pound-Force Inches", "lbf in"},
		{"Pounds Per Sq. Inch", "psi"},
		{"Pounds", "lb"},
		{"RPM", "RPM"},
		{"Radians Per Second", "rad/s"},
		{"Radians", "rad"},
		{"Seconds", "s"},
		{"Sq. Centimetres", "cm<sup><small>2</small></sup>"},
		{"Sq. Feet", "sq ft"},
		{"Sq. Inches", "sq in"},
		{"Sq. Kilometres", "km<sup><small>2</small></sup>"},
		{"Sq. Metres", "m<sup><small>2</small></sup>"},
		{"Sq. Miles", "sq mi"},
		{"Sq. Millimetres", "mm<sup><small>2</small></sup>"},
		{"Sq. Yards", "sq yd"},
		{"Stone", "st"},
		{"Tebibytes", "TiB"},
		{"Terabytes", "TB"},
		{"Thou", "th"},
		{"Tonnes", "t"},
		{"Tons", "ton"},
		{"Torr", "Torr"},
		{"US Fluid Ounces", "fl.oz<sub><small>US</small></sub>"},
		{"US Gallons", "gal<sub><small>US</small></sub>"},
		{"US Hundredweight", "cwt<sub><small>US</small></sub>"},
		{"US Pints", "pt<sub><small>US</small></sub>"},
		{"US Tons", "ton<sub><small>US</small></sub>"},
		{"Watts", "W"},
		{"Weeks", "wk"},
		{"Yards", "yd"},
		// {"British Thermal Units", "BTU"},
		// {"Years (Gregorian)", "a<sub><small>g</small></sub>"},
		// {"Years (Julian)", "a<sub><small>j</small></sub>"},
		{"Calories Per Second", "cal/s"},
		{"Megawatts", "MW"},
		// Currencies
		{"Australian Dollar", "AUD"},
		{"Bulgarian Lev", "BGN"},
		{"Brazilian Real", "BRL"},
		{"Canadian Dollar", "CAD"},
		{"Swiss Franc", "CHF"},
		{"Chinese Yuan", "CNY"},
		{"Czech Koruna", "CZK"},
		{"Danish Krone", "DKK"},
		{"Euro", "EUR / &euro;"},
		{"United Kingdom Pound", "GBP / &pound;"},
		{"Hong Kong Dollar", "HKD"},
		{"Croatian Kuna", "HRK"},
		{"Hungarian Forint", "HUF"},
		{"Indonesian Rupiah", "IDR"},
		{"Israeli New Shekel", "ILS"},
		{"Indian Rupee", "INR"},
		{"Icelandic Krona", "ISK"},
		{"Japanese Yen", "JPY"},
		{"South Korean Won", "KRW"},
		{"Mexican Peso", "MXN"},
		{"Malaysian Ringgit", "MYR"},
		{"Norwegian Krone", "NOK"},
		{"New Zealand Dollar", "NZD"},
		{"Philippine Peso", "PHP"},
		{"Polish Z\u0142oty", "PLN"},
		{"Romanian Leu", "RON"},
		{"Russian Ruble", "RUB"},
		{"Swedish Krona", "SEK"},
		{"Singapore Dollar", "SGD"},
		{"Thai Baht - Baht", "THB"},
		{"Turkish Lira", "TRY"},
		{"US Dollar", "USD / $"},
		{"South African Rand", "ZAR"}
	};
	unitSymbols = unitTableInit;

	std::initializer_list<std::pair<const std::string, std::string> > currencyTableInit = {
		{"AUD", "Australian Dollars"},
		{"BGN", "Bulgarian Levs"},
		{"BRL", "Brazilian Real"},
		{"CAD", "Canadian Dollars"},
		{"CHF", "Swiss Francs"},
		{"CNY", "Chinese Yuan"},
		{"CZK", "Czech Koruna"},
		{"DKK", "Danish Krone"},
		{"EUR", "Euros"},
		{"GBP", "GB Pounds"},
		{"HKD", "Hong Kong Dollars"},
		{"HRK", "Croatian Kuna"},
		{"HUF", "Hungarian Forint"},
		{"IDR", "Indonesian Rupiah"},
		{"ILS", "Israeli New Shekels"},
		{"INR", "Indian Rupees"},
		{"ISK", "Icelandic Krona"},
		{"JPY", "Japanese Yen"},
		{"KRW", "South Korean Won"},
		{"MXN", "Mexican Pesos"},
		{"MYR", "Malaysian Ringgit"},
		{"NOK", "Norwegian Krone"},
		{"NZD", "New Zealand Dollars"},
		{"PHP", "Philippine Pesos"},
		{"PLN", "Polish Z\u0142oty"},
		{"RON", "Romanian Leu"},
		{"RUB", "Russian Rubles"},
		{"SEK", "Swedish Krona"},
		{"SGD", "Singapore Dollars"},
		{"THB", "Thai Baht"},
		{"TRY", "Turkish Lira"},
		{"USD", "US Dollars"},
		{"ZAR", "South African Rand"}
	};
	currencyMap = currencyTableInit;

	/* Fluid volume conversions */
	std::vector<Conversion> volumeTable = {
		{"Pints",              "Fluid Ounces",       NULL,                            20.0},
		{"Fluid Ounces",       "Pints",              NULL,                            1.0/20.0},
		{"US Fluid Ounces",    "Cubic Inches",       NULL,                            1.8046875},
		{"Cubic Inches",       "US Fluid Ounces",    NULL,                            1.0/1.8046875},
		{"US Pints",           "US Fluid Ounces",    NULL,                            16.0},
		{"US Fluid Ounces",    "US Pints",           NULL,                            1.0/16.0},
		{"Pints",              "Gallons",            &Stack::convertPintsToGallons,   0.0},
		{"Gallons",            "Pints",              &Stack::convertGallonsToPints,   0.0},
		{"US Pints",           "US Gallons",         &Stack::convertPintsToGallons,   0.0},
		{"US Gallons",         "US Pints",           &Stack::convertGallonsToPints,   0.0},
		{"Pints",              "Litres",             &Stack::convertUKPintsToLitres,  0.0},
		{"Litres",             "Pints",              &Stack::convertLitresToUKPints,  0.0},
		{"US Pints",           "Litres",             &Stack::convertUSPintsToLitres,  0.0},
		{"Litres",             "US Pints",           &Stack::convertLitresToUSPints,  0.0},
		{"Millilitres",        "Litres",             NULL,                            1.0/1000.0},
		{"Litres",             "Millilitres",        NULL,                            1000.0},
		{"Millilitres",        "Cubic Centimetres",  &Stack::returnNoError,           0.0},
		{"Cubic Centimetres",  "Millilitres",        &Stack::returnNoError,           0.0},
		{"Litres",             "Cubic Metres",       NULL,                            1.0/1000.0},
		{"Cubic Metres",       "Litres",             NULL,                            1000.0},
		{"Cubic Millimetres",  "Cubic Metres",       NULL,                            1.0e-9},
		{"Cubic Metres",       "Cubic Millimetres",  NULL,                            1.0e9},
		{"Cubic Decimetres",   "Cubic Metres",       NULL,                            1.0/1000.0},
		{"Cubic Metres",       "Cubic Decimetres",   NULL,                            1000.0},
		{"Millilitres",        "Cubic Inches",       NULL,                            1.0/(2.54*2.54*2.54)},
		{"Cubic Inches",       "Millilitres",        NULL,                            2.54*2.54*2.54},
		{"Cubic Inches",       "Cubic Feet",         NULL,                            1.0/1728.0},
		{"Cubic Feet",         "Cubic Inches",       NULL,                            1728.0},
		{"Cubic Feet",         "Cubic Yards",        NULL,                            1.0/27.0},
		{"Cubic Yards",        "Cubic Feet",         NULL,                            27.0}
	};
	conversionTable["Volume"] = volumeTable;
	/* Weight conversions */
	std::vector<Conversion> massTable = {
		{"Ounces",            "Grams",             &Stack::convertOuncesToGrams,      0.0},
		{"Grams",             "Ounces",            &Stack::convertGramsToOunces,      0.0},
		{"Grams",             "Kilograms",         &Stack::convertGramsToKilograms,   0.0},
		{"Kilograms",         "Grams",             &Stack::convertKilogramsToGrams,   0.0},
		{"Kilograms",         "Pounds",            &Stack::convertKilogramsToPounds,  0.0},
		{"Pounds",            "Kilograms",         &Stack::convertPoundsToKilograms,  0.0},
		{"Kilograms",         "Stone",             &Stack::convertKilogramsToStone,   0.0},
		{"Stone",             "Kilograms",         &Stack::convertStoneToKilograms,   0.0},
		{"Grams",             "Microgram",         NULL,                              1e6},
		{"Microgram",         "Grams",             NULL,                              1.0/(1e6)},
		{"Grams",             "Milligrams",        NULL,                              1000.0},
		{"Milligrams",        "Grams",             NULL,                              1.0/1000.0},
		{"Tonnes",            "Kilograms",         NULL,                              1000.0},
		{"Kilograms",         "Tonnes",            NULL,                              1.0/1000.0},
		{"Stone",             "Hundredweight",     NULL,                              1.0/8.0},
		{"Hundredweight",     "Stone",             NULL,                              8.0},
		{"Pounds",            "US Hundredweight",  NULL,                              1.0/100.0},
		{"US Hundredweight",  "Pounds",            NULL,                              100.0},
		{"Hundredweight",     "Tons",              NULL,                              1.0/20.0},
		{"Tons",              "Hundredweight",     NULL,                              20.0},
		{"Pounds",            "US Tons",           NULL,                              1.0/2000.0},
		{"US Tons",           "Pounds",            NULL,                              2000.0}
	};
	conversionTable["Mass"] = massTable;
	/* Torque conversions */
	std::vector<Conversion> torqueTable = {
		{"Pound-Force Feet",            "Newton Metres",               &Stack::convertPoundsFeetToNewtonMetres,  0.0},
		{"Newton Metres",               "Pound-Force Feet",            &Stack::convertNewtonMetresToPoundsFeet,  0.0},
		{"Newton Metres",               "Newton Centimetres",          NULL,                                     100.0},
		{"Newton Centimetres",          "Newton Metres",               NULL,                                     1.0/100.0},
		{"Newton Metres",               "Newton Millimetres",          NULL,                                     1000.0},
		{"Newton Millimetres",          "Newton Metres",               NULL,                                     1.0/1000.0},
		{"Newton Metres",               "Kilogram-Force Metres",       NULL,                                     1.0/9.80665},
		{"Kilogram-Force Metres",       "Newton Metres",               NULL,                                     9.80665},
		{"Kilogram-Force Metres",       "Kilogram-Force Centimetres",  NULL,                                     100.0},
		{"Kilogram-Force Centimetres",  "Kilogram-Force Metres",       NULL,                                     1.0/100.0},
		{"Kilogram-Force Metres",       "Kilogram-Force Millimetres",  NULL,                                     1000.0},
		{"Kilogram-Force Millimetres",  "Kilogram-Force Metres",       NULL,                                     1.0/1000.0},
		{"Kilogram-Force Metres",       "Gram-Force Metres",           NULL,                                     1000.0},
		{"Gram-Force Metres",           "Kilogram-Force Metres",       NULL,                                     1.0/1000.0},
		{"Gram-Force Metres",           "Gram-Force Millimetres",      NULL,                                     1000.0},
		{"Gram-Force Millimetres",      "Gram-Force Metres",           NULL,                                     1.0/1000.0},
		{"Gram-Force Metres",           "Gram-Force Centimetres",      NULL,                                     100.0},
		{"Gram-Force Centimetres",      "Gram-Force Metres",           NULL,                                     1.0/100.0},
		{"Pound-Force Feet",            "Pound-Force Inches",          NULL,                                     12.0},
		{"Pound-Force Inches",          "Pound-Force Feet",            NULL,                                     1.0/12.0},
		{"Pound-Force Feet",            "Ounce-Force Feet",            NULL,                                     16.0},
		{"Ounce-Force Feet",            "Pound-Force Feet",            NULL,                                     1.0/16.0},
		{"Pound-Force Inches",          "Ounce-Force Inches",          NULL,                                     16.0},
		{"Ounce-Force Inches",          "Pound-Force Inches",          NULL,                                     1.0/16.0}
	};
	conversionTable["Torque"] = torqueTable;
	std::vector<Conversion> speedTable = {
		{"Metres Per Second",    "Kilometres Per Hour",  NULL,                     3.6},
		{"Kilometres Per Hour",  "Metres Per Second",    NULL,                     1.0/3.6},
		{"Kilometres Per Hour",  "Metres Per Hour",      NULL,                     1000.0},
		{"Metres Per Hour",      "Kilometres Per Hour",  NULL,                     1.0/1000.0},
		{"Miles Per Hour",       "Feet Per Second",      NULL,                     1760.0 * 3.0 / (3600.0)},
		{"Feet Per Second",      "Miles Per Hour",       NULL,                     1.0/(1760.0 * 3.0 / (3600.0))},
		{"Miles Per Hour",       "Kilometres Per Hour",  &Stack::convertMPHToKMH,  0.0},
		{"Kilometres Per Hour",  "Miles Per Hour",       &Stack::convertKMHToMPH,  0.0},
		{"Knots",                "Metres Per Hour",      NULL,                     1852.0},
		{"Metres Per Hour",      "Knots",                NULL,                     1.0/1852.0}
	};
	conversionTable["Speed"] = speedTable;
	std::vector<Conversion> timeTable = {
		{"Seconds",                "Nanoseconds",            NULL,                       1e9},
		{"Nanoseconds",            "Seconds",                NULL,                       1.0/(1e9)},
		{"Seconds",                "Microseconds",           NULL,                       1e6},
		{"Microseconds",           "Seconds",                NULL,                       1.0/(1e6)},
		{"Seconds",                "Milliseconds",           NULL,                       1e3},
		{"Milliseconds",           "Seconds",                NULL,                       1.0/(1e3)},
		{"Minutes",                "Seconds",                NULL,                       60.0},
		{"Seconds",                "Minutes",                NULL,                       1.0/60.0},
		{"Hours",                  "Minutes",                NULL,                       60.0},
		{"Minutes",                "Hours",                  NULL,                       1.0/60.0},
		{"Days",                   "Hours",                  NULL,                       24.0},
		{"Hours",                  "Days",                   NULL,                       1.0/24.0},
		{"Weeks",                  "Days",                   NULL,                       7.0},
		{"Days",                   "Weeks",                  NULL,                       1.0/7.0},
		{"Hours",                  "Hours.Minutes-Seconds",  &Stack::convertHoursToHms,  0.0},
		{"Hours.Minutes-Seconds",  "Hours",                  &Stack::convertHmsToHours,  0.0}
		// "Years (Julian)":Days / 365.25
		// "Years (Gregorian)", "a<sub><small>g</small></sub>"
	};
	conversionTable["Time"] = timeTable;
	std::vector<Conversion> dateTable = {
		{"Day of Year", "Date in Year", &Stack::convertDayOfYearToDateInCurrentYear, 0.0 },
		{"Date in Year", "Day of Year", &Stack::convertDateInCurrentYearToDayOfYear, 0.0 }
		// "Years (Julian)":Days / 365.25
		// "Years (Gregorian)", "a<sub><small>g</small></sub>"
	};
	conversionTable["Date"] = dateTable;
	std::vector<Conversion> forceTable = {
		{"Newtons",         "Micronewtons",    NULL,  1e6},
		{"Micronewtons",    "Newtons",         NULL,  1.0/1e6},
		{"Newtons",         "Millinewtons",    NULL,  1e3},
		{"Millinewtons",    "Newtons",         NULL,  1.0/1e3},
		{"Kilonewtons",     "Newtons",         NULL,  1e3},
		{"Newtons",         "Kilonewtons",     NULL,  1.0/1e3},
		{"Kilogram-Force",  "Newtons",         NULL,  9.80665},
		{"Newtons",         "Kilogram-Force",  NULL,  1.0/9.80665},
		{"Kilogram-Force",  "Gram-Force",      NULL,  1000.0},
		{"Gram-Force",      "Kilogram-Force",  NULL,  1.0/1000.0},
		{"Pound-Force",     "Newtons",         NULL,  4.4482216152605},
		{"Newtons",         "Pound-Force",     NULL,  1.0/4.4482216152605},
		{"Pound-Force",     "Ounce-Force",     NULL,  16.0},
		{"Ounce-Force",     "Pound-Force",     NULL,  1.0/16.0}
	};
	conversionTable["Force"] = forceTable;
	std::vector<Conversion> pressureTable = {
		{"Pascal",                "Hectopascal",           NULL,  1.0/100.0},
		{"Hectopascal",           "Pascal",                NULL,  100.0},
		{"Pascal",                "Kilopascal",            NULL,  1.0/1e3},
		{"Kilopascal",            "Pascal",                NULL,  1e3},
		{"Pascal",                "Megapascal",            NULL,  1.0/1e6},
		{"Megapascal",            "Pascal",                NULL,  1e6},
		{"Millibar",              "Pascal",                NULL,  100.0},
		{"Pascal",                "Millibar",              NULL,  1.0/100.0},
		{"Millibar",              "Bar",                   NULL,  1.0/1000.0},
		{"Bar",                   "Millibar",              NULL,  1000.0},
		{"Pascal",                "Atmosphere",            NULL,  1.0/101325.0},
		{"Atmosphere",            "Pascal",                NULL,  101325.0},
		{"Kilopascal",            "Kilograms Per Sq. cm",  NULL,  1.0/98.0665},
		{"Kilograms Per Sq. cm",  "Kilopascal",            NULL,  98.0665},
		{"Pascal",                "Pounds Per Sq. Inch",   NULL,  1.0/6894.780176784},
		{"Pounds Per Sq. Inch",   "Pascal",                NULL,  6894.780176784},
		{"Pascal",                "Inches of Mercury",     NULL,  1.0/3386.389},
		{"Inches of Mercury",     "Pascal",                NULL,  3386.389},
		{"Torr",                  "Atmosphere",            NULL,  1.0/760.0},
		{"Atmosphere",            "Torr",                  NULL,  760.0}
	};
	conversionTable["Pressure"] = pressureTable;
	// TODO Review got here
	std::vector<Conversion> energyTable = {
		{"Kilojoules",      "Joules",          NULL,  1000.0},
		{"Joules",          "Kilojoules",      NULL,  1.0/1000.0},
		{"Megajoules",      "Kilojoules",      NULL,  1000.0},
		{"Kilojoules",      "Megajoules",      NULL,  1.0/1000.0},
		{"Joules",          "Kilowatt-Hours",  NULL,  1.0/3.6e6},
		{"Kilowatt-Hours",  "Joules",          NULL,  3.6e6},
		{"Joules",          "Kilocalories",    NULL,  1.0/4184.0},
		{"Kilocalories",    "Joules",          NULL,  4184.0},
		{"Kilocalories",    "Calories",        NULL,  1000.0},
		{"Calories",        "Kilocalories",    NULL,  1.0/1000.0}
		// "British Thermal Units", "BTU"
	};
	conversionTable["Energy"] = energyTable;
	/* Temperature conversions */
	std::vector<Conversion> temperatureTable = {
		{"Kelvin",      "Celsius",     &Stack::convertKelvinToCelsius,      0.0},
		{"Celsius",     "Kelvin",      &Stack::convertCelsiusToKelvin,      0.0},
		{"Celsius",     "Fahrenheit",  &Stack::convertCelsiusToFahrenheit,  0.0},
		{"Fahrenheit",  "Celsius",     &Stack::convertFahrenheitToCelsius,  0.0}
	};
	conversionTable["Temperature"] = temperatureTable;
	std::vector<Conversion> areaTable = {
		{"Sq. Millimetres",  "Sq. Metres",       NULL,  1.0/1e6},
		{"Sq. Metres",       "Sq. Millimetres",  NULL,  1e6},
		{"Sq. Centimetres",  "Sq. Metres",       NULL,  1.0/10000.0},
		{"Sq. Metres",       "Sq. Centimetres",  NULL,  10000.0},
		{"Sq. Metres",       "Sq. Kilometres",   NULL,  1.0/1e6},
		{"Sq. Kilometres",   "Sq. Metres",       NULL,  1e6},
		{"Sq. Metres",       "Hectares",         NULL,  1.0/10000.0},
		{"Hectares",         "Sq. Metres",       NULL,  10000.0},
		{"Sq. Millimetres",  "Sq. Inches",       NULL,  1.0/(25.4*25.4)},
		{"Sq. Inches",       "Sq. Millimetres",  NULL,  25.4*25.4},
		{"Sq. Inches",       "Sq. Feet",         NULL,  1.0/(12.0*12.0)},
		{"Sq. Feet",         "Sq. Inches",       NULL,  12.0*12.0},
		{"Sq. Feet",         "Sq. Yards",        NULL,  1.0/9.0},
		{"Sq. Yards",        "Sq. Feet",         NULL,  9.0},
		{"Sq. Yards",        "Acres",            NULL,  1.0/4840.0},
		{"Acres",            "Sq. Yards",        NULL,  4840.0},
		{"Sq. Yards",        "Sq. Miles",        NULL,  1.0/(1760.0*1760.0)},
		{"Sq. Miles",        "Sq. Yards",        NULL,  1760.0*1760.0}
	};
	conversionTable["Area"] = areaTable;
	std::vector<Conversion> dataSizeTable = {
		{"Kibibytes",  "Bytes",      NULL,  1024.0},
		{"Bytes",      "Kibibytes",  NULL,  1.0/1024.0},
		{"Mebibytes",  "Kibibytes",  NULL,  1024.0},
		{"Kibibytes",  "Mebibytes",  NULL,  1.0/1024.0},
		{"Gibibytes",  "Mebibytes",  NULL,  1024.0},
		{"Mebibytes",  "Gibibytes",  NULL,  1.0/1024.0},
		{"Tebibytes",  "Gibibytes",  NULL,  1024.0},
		{"Gibibytes",  "Tebibytes",  NULL,  1.0/1024.0},
		{"Kilobytes",  "Bytes",      NULL,  1000.0},
		{"Bytes",      "Kilobytes",  NULL,  1.0/1000.0},
		{"Megabytes",  "Kilobytes",  NULL,  1000.0},
		{"Kilobytes",  "Megabytes",  NULL,  1.0/1000.0},
		{"Gigabytes",  "Megabytes",  NULL,  1000.0},
		{"Megabytes",  "Gigabytes",  NULL,  1.0/1000.0},
		{"Terabytes",  "Gigabytes",  NULL,  1000.0},
		{"Gigabytes",  "Terabytes",  NULL,  1.0/1000.0}
	};
	conversionTable["Data Size"] = dataSizeTable;
	/* Distance conversions */
	std::vector<Conversion> distanceTable = {
		{"Inches",          "Millimetres",     &Stack::convertInchToMM,  0.0},
		{"Millimetres",     "Inches",          &Stack::convertMMToInch,  0.0},
		{"Metres",          "Millimetres",     NULL,                     1000.0},
		{"Millimetres",     "Metres",          NULL,                     1.0/1000.0},
		{"Millimetres",     "Microns",         NULL,                     1000.0},
		{"Microns",         "Millimetres",     NULL,                     1.0/1000.0},
		{"Nanometres",      "Microns",         NULL,                     1.0/1000.0},
		{"Microns",         "Nanometres",      NULL,                     1000.0},
		{"Micrometres",     "Microns",         &Stack::returnNoError,    0.0},
		{"Microns",         "Micrometres",     &Stack::returnNoError,    0.0},
		{"Nanometres",      "Angstroms",       NULL,                     10.0},
		{"Angstroms",       "Nanometres",      NULL,                     0.1},
		{"Metres",          "Centimetres",     NULL,                     100.0},
		{"Centimetres",     "Metres",          NULL,                     1.0/100.0},
		{"Kilometres",      "Metres",          NULL,                     1000.0},
		{"Metres",          "Kilometres",      NULL,                     1.0/1000.0},
		{"Inches",          "Thou",            NULL,                     1000.0},
		{"Thou",            "Inches",          NULL,                     1.0/1000.0},
		{"Inches",          "Points",          NULL,                     72.0},
		{"Points",          "Inches",          NULL,                     1.0/72.0},
		{"Inches",          "Feet",            NULL,                     1.0/12.0},
		{"Feet",            "Inches",          NULL,                     12.0},
		{"Yards",           "Feet",            NULL,                     3.0},
		{"Feet",            "Yards",           NULL,                     1.0/3.0},
		{"Yards",           "Miles",           NULL,                     1.0/1760.0},
		{"Miles",           "Yards",           NULL,                     1760.0},
		{"Yards",           "Furlongs",        NULL,                     1.0/220.0},
		{"Furlongs",        "Yards",           NULL,                     220.0},
		{"Metres",          "Microns",         NULL,                     1e6},
		{"Microns",         "Metres",          NULL,                     1.0/1e6},
		{"Mils",            "Thou",            &Stack::returnNoError,    0.0},
		{"Thou",            "Mils",            &Stack::returnNoError,    0.0},
		{"Nautical Miles",  "Metres",          NULL,                     1852.0},
		{"Metres",          "Nautical Miles",  NULL,                     1.0/1852.0},
		{"Fathoms",         "Feet",            NULL,                     6.0},
		{"Feet",            "Fathoms",         NULL,                     1.0/6.0},
		{"Chains",          "Yards",           NULL,                     22.0},
		{"Yards",           "Chains",          NULL,                     1.0/22.0},
		{"Light Years",     "Metres",          NULL,                     9460730472580800.0},
		{"Metres",          "Light Years",     NULL,                     1.0/9460730472580800.0}
	};
	conversionTable["Distance"] = distanceTable;
	/* Angular conversions */
	std::vector<Conversion> angleTable = {
		{"Radians",                  "Degrees",                  &Stack::convertRadiansToDegrees,  0.0},
		{"Degrees",                  "Radians",                  &Stack::convertDegreesToRadians,  0.0},
		{"Degrees",                  "Degrees.Minutes-Seconds",  &Stack::convertHoursToHms,        0.0},
		{"Degrees.Minutes-Seconds",  "Degrees",                  &Stack::convertHmsToHours,        0.0},
		{"Degrees.Minutes",          "Degrees",                  &Stack::convertHmToHours,         0.0},
		{"Degrees",                  "Degrees.Minutes",          &Stack::convertHoursToHm,         0.0}
	};
	conversionTable["Angle"] = angleTable;
	/* Power conversions */
	std::vector<Conversion> powerTable = {
		{"Watts",                "Kilowatts",            NULL,  1.0/1000.0},
		{"Kilowatts",            "Watts",                NULL,  1000.0},
		{"Watts",                "Horsepower (Mech)",    NULL,  1.0/745.69987158227022},
		{"Horsepower (Mech)",    "Watts",                NULL,  745.69987158227022},
		{"Horsepower (Metric)",  "Watts",                NULL,  735.49875},
		{"Watts",                "Horsepower (Metric)",  NULL,  1.0/735.49875},
		{"Megawatts",            "Watts",                NULL,  1e6},
		{"Watts",                "Megawatts",            NULL,  1.0/1e6},
		{"Calories Per Second",  "Watts",                NULL,  4.184},
		{"Watts",                "Calories Per Second",  NULL,  1.0/4.184}
		// "BTUs Per Hour", "BTU/h"
	};
	conversionTable["Power"] = powerTable;
	/* Frequency conversions */
	std::vector<Conversion> frequencyTable = {
		{"RPM",                 "Hertz",               &Stack::convertRPMToHertz,        0.0},
		{"Hertz",               "RPM",                 &Stack::convertHertzToRPM,        0.0},
		{"Radians Per Second",  "Hertz",               &Stack::convertRadPerSecToHertz,  0.0},
		{"Hertz",               "Radians Per Second",  &Stack::convertHertzToRadPerSec,  0.0}
	};
	conversionTable["Frequency"] = frequencyTable;
	/* Fuel economy conversions */
	std::vector<Conversion> fuelEconomyTable = {
		{"Miles Per Gallon",           "Miles Per Litre",            &Stack::convertMPGToMPL,                            0.0},
		{"Miles Per Litre",            "Miles Per Gallon",           &Stack::convertMPLToMPG,                            0.0},
		{"Miles Per Gallon",           "Miles Per US Gallon",        &Stack::convertMPGToUSMPG,                          0.0},
		{"Miles Per US Gallon",        "Miles Per Gallon",           &Stack::convertUSMPGToMPG,                          0.0},
		{"Kilometres Per Litre",       "Miles Per Litre",            &Stack::convertKMPLToMPL,                           0.0},
		{"Miles Per Litre",            "Kilometres Per Litre",       &Stack::convertMPLToKMPL,                           0.0},
		{"Kilometres Per Litre",       "Litres Per 100 Kilometres",  &Stack::convertKilometresPerLitreToLitresPer100KM,  0.0},
		{"Litres Per 100 Kilometres",  "Kilometres Per Litre",       &Stack::convertLitresPer100KMToKilometresPerLitre,  0.0}
	};
	conversionTable["Fuel Economy"] = fuelEconomyTable;
}

void Stack::registerCurrencies(std::map<std::string, double> wrtEuro)
{
	std::vector<Conversion> conversions;
	for (auto const& [name, euros] : wrtEuro) {
		if (currencyMap.contains(name)) {
			std::string currencyName = currencyMap[name];
			Conversion convFrom = {"Euros", currencyName, NULL, euros};
			Conversion convTo = {currencyName, "Euros", NULL, 1.0/euros};
			conversions.push_back(convFrom);
			conversions.push_back(convTo);
		}
		else {
			std::cerr << "Unknown currency: " << name << std::endl;
		}
	}
	if (conversions.size() > 0) {
		conversionTable["Currency"] = conversions;
		rawCurrencyData = wrtEuro;
	}
}

std::map<std::string, double> Stack::getRawCurrencyData()
{
	return rawCurrencyData;
}

bool Stack::seenConv(std::vector<Conversion> path, std::string to)
{
	for (Conversion p: path) {
		if ((p.to == to) || (p.from == to)) {
			return true;
		}
	}
	return false;
}

std::vector<Conversion> Stack::getConvPathsFrom(std::string type, std::string from, std::vector<Conversion> pathSoFar)
{
	std::vector<Conversion> result;
	std::vector<Conversion> conversionList = conversionTable[type];

	for (Conversion conversion: conversionList) {
		if ((conversion.from == from) && ( ! seenConv(pathSoFar, conversion.to))) {
			result.push_back(conversion);
		}
	}
	return result;
}

std::vector<Conversion> Stack::getConvPathsFromTo(std::string type, std::string from, std::string to, std::vector<Conversion> pathSoFar)
{
	std::vector<Conversion> result;
	std::vector<Conversion> pathList = getConvPathsFrom(type, from, pathSoFar);

	for (Conversion p : pathList) {
		if (p.to == to) {
			std::vector<Conversion> one;
			one.push_back(p);
			return one;
		}
	}

	/* Didn't find, so keep going */

	for (Conversion p : pathList) {
		std::vector<Conversion> psF = pathSoFar;
		psF.push_back(p);
		std::vector<Conversion> r = getConvPathsFromTo(type, p.to, to, psF);
		if (r.size() > 0) {
			/* Must have found it */
			result.push_back(p);
			for (Conversion q : r) {
				result.push_back(q);
			}
			return result;
		}
	}
	return result;
}

ErrorCode Stack::convert(std::string type, std::string from,  std::string to)
{
	bool foundFrom = false;
	bool foundTo = false;

	//std::cerr << "Converting " << peek().toString() << " from " << from << " to " << to << std::endl;

	if (from == to) {
		return NoError;
	}

	if ( ! conversionTable.contains(type)) {
		return UnknownConversion;
	}
	std::vector<Conversion> conversionList = conversionTable[type];

	for (Conversion conversion : conversionList) {
		if (conversion.from == from) {
			foundFrom = true;
		}
		if (conversion.to == to) {
			foundTo = true;
		}
		if (foundFrom && foundTo) {
			break;
		}
	}

	if ( ( ! foundFrom) || ( ! foundTo) ) {
		return UnknownConversion;
	}

	std::vector<Conversion> conversionPath = getConvPathsFromTo(type, from, to);
	if (conversionPath.size() == 0) {
		return UnknownConversion;
	}

	std::vector<AF> stackCopy = stack;
	ErrorCode result;
#ifdef CONVERT_DEBUG
	std::cerr
		<< "Converting from " << from
		<< " to " << to << std::endl;
	int step = 0;
#endif
	for (Conversion conv : conversionPath) {
#ifdef CONVERT_DEBUG
		std::cerr << " Path " << step++ << ": "
			<< conv.from << " to " << conv.to;
		if (conv.ConvFunc == NULL) {
			std::cerr << " with multiplier " << conv.multiplier << std::endl;
		}
		else {
			std::cerr << " with function" << std::endl;
		}
#endif
		if (conv.ConvFunc == NULL) {
			result = convertMultiplier(conv.multiplier);
		}
		else {
			result = (this->*(conv.ConvFunc))();
		}
		if (result != NoError) {
			stack = stackCopy;
			return result;
		}
	}
	return NoError;
}

std::set<std::string> Stack::getAvailableConversions(std::string from) 
{
	std::set<std::string> build_list;
	bool this_table = false;


	for (const auto & [key, conversionList] : conversionTable) {
		build_list.clear();
		for (Conversion conversion : conversionList) {
			std::string convFrom = conversion.from;
			build_list.insert(convFrom);
			if (convFrom == from) {
				this_table = true;
			}
		}
		if (this_table) {
			return build_list;
		}
	}
	build_list.clear();
	return build_list;
}

std::vector<std::string> Stack::getConversionCategories()
{
	std::vector<std::string> result;
	for (const auto & [key, conversionList] : conversionTable) {
		result.push_back(key);
	}
	std::sort(result.begin(), result.end());
	return result;
}

std::set<std::string> Stack::getAvailableUnits(std::string category)
{
	std::set<std::string> build_list;

	if (conversionTable.contains(category)) {
		for (Conversion conversion : conversionTable[category]) {
			build_list.insert(conversion.from);
		}
	}

	return build_list;
}

std::vector<Constant> Stack::getConstants()
{
	return constants;
}

std::vector<Density> Stack::getDensities()
{
	return densities;
}

/* Things that were done in a lambda in Kotlin: */
ErrorCode Stack::returnNoError()
{
	return NoError;
}

ErrorCode Stack::convertUKPintsToLitres() {
	return convertPintsToLitres();
}

ErrorCode Stack::convertLitresToUKPints() {
	return convertLitresToPints();
}

ErrorCode Stack::convertUSPintsToLitres()
{
	return convertPintsToLitres(true);
}

ErrorCode Stack::convertLitresToUSPints()
{
	return convertLitresToPints(true);
}

ErrorCode Stack::convertMPHToKMH()
{
	return convert("Distance", "Miles", "Kilometres");
}

ErrorCode Stack::convertKMHToMPH()
{
	return convert("Distance", "Kilometres", "Miles");
}

ErrorCode Stack::convertMPGToMPL() {
	return convert("Volume", "Litres", "Gallons");
}

ErrorCode Stack::convertMPLToMPG() {
	return convert("Volume", "Gallons", "Litres");
}

ErrorCode Stack::convertMPGToUSMPG() {
	return convert("Volume", "US Gallons", "Gallons");
}

ErrorCode Stack::convertUSMPGToMPG() {
	return convert("Volume", "Gallons", "US Gallons");
}

ErrorCode Stack::convertKMPLToMPL() {
	return convert("Distance", "Kilometres", "Miles");
}

ErrorCode Stack::convertMPLToKMPL() {
	return convert("Distance", "Miles", "Kilometres");
}
