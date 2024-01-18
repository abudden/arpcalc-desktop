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

ErrorCode Stack::random()
{
	std::uniform_real_distribution<double> dist(0,32768);
	push(AF(dist(rng)));
	return NoError;
}

ErrorCode Stack::plus()
{
	push(pop() + pop());
	return NoError;
}

ErrorCode Stack::minus()
{
	AF x = pop();
	AF y = pop();
	push(y - x);
	return NoError;
}

ErrorCode Stack::times()
{
	push(pop()*pop());
	return NoError;
}

ErrorCode Stack::divide()
{
	AF x = pop();
	if (x == AF(0.0)) {
		push(x);
		return DivideByZero;
	}
	else {
		AF y = pop();
		push(y/x);
	}
	return NoError;
}

ErrorCode Stack::xrooty()
{
	AF x = pop();
	AF y = pop();
	if ((x == AF(0.0)) || (y < 0.0)) {
		push(y);
		push(x);
		return InvalidRoot;
	}
	else {
		push(y.root(x));
	}
	return NoError;
}

ErrorCode Stack::invert()
{
	push(AF(0.0) - pop());
	return NoError;
}

ErrorCode Stack::etox()
{
	AF e = AF::e();
	push(e.pow(pop()));
	return NoError;
}

ErrorCode Stack::tentox()
{
	AF ten = AF(10.0);
	push(ten.pow(pop()));
	return NoError;
}

ErrorCode Stack::twotox()
{
	AF two = AF(2.0);
	push(two.pow(pop()));
	return NoError;
}

ErrorCode Stack::percent()
{
	/* This implementation matches the HP calculator one:
	 * calculate x %of y, so if y is 70 and x is 35.  The result
	 * is (35/100)*70.  By default PercentLeavesY is set and
	 * hence y stays as 70 and x becomes 24.5.
	 */
	AF x = pop();
	AF y = pop();
	if (options.contains(PercentLeavesY)) {
		push(y);
	}
	push((x/100.0)*y);
	return NoError;
}

ErrorCode Stack::percentchange()
{
	/* This implementation matches the HP calculator one:
	 * calculate the percentage change of Y that would give X.
	 * This is calculated as ((x-y)/y)*100
	 * By default PercentLeavesY is set.
	 */
	AF x = pop();
	AF y = pop();
	if (options.contains(PercentLeavesY)) {
		push(y);
	}
	push(((x-y)/y)*100.0);
	return NoError;
}

ErrorCode Stack::square()
{
	AF x = pop();
	push(x*x);
	return NoError;
}

ErrorCode Stack::cube()
{
	AF x = pop();
	push(x*x*x);
	return NoError;
}

ErrorCode Stack::reciprocal()
{
	AF x = pop();
	if (x == AF(0.0)) {
		push(x);
		return DivideByZero;
	}
	else {
		AF one(1.0);
		push(one/x);
	}
	return NoError;
}

ErrorCode Stack::integerpart()
{
	AF x = pop();
	if (x > 0.0) {
		push(x.floor());
	}
	else {
		push(x.ceil());
	}
	return NoError;
}

ErrorCode Stack::floatingpart()
{
	AF x = pop();
	if (x > 0.0) {
		push(x - x.floor());
	}
	else {
		push(x - x.ceil());
	}
	return NoError;
}

ErrorCode Stack::integerdivide()
{
	AF x = pop();
	if (x == AF(0.0)) {
		push(x);
		return DivideByZero;
	}
	else {
		AF y = pop();
		AF v = y / x;
		if (v >= 0.0) {
			push(v.floor());
		}
		else {
			push(v.ceil());
		}
	}
	return NoError;
}

ErrorCode Stack::bitwiseand()
{
	intmax_t xl = pop().round().toLong();
	intmax_t yl = pop().round().toLong();
	intmax_t mask = getBitMask();
	xl &= mask;
	yl &= mask;
	intmax_t v = xl & yl;
	v &= mask;
	push(AF(v));
	return NoError;
}

ErrorCode Stack::bitwiseor()
{
	intmax_t xl = pop().round().toLong();
	intmax_t yl = pop().round().toLong();
	intmax_t mask = getBitMask();
	xl &= mask;
	yl &= mask;
	intmax_t v = xl | yl;
	v &= mask;
	push(AF(v));
	return NoError;
}

ErrorCode Stack::bitwisexor()
{
	intmax_t xl = pop().round().toLong();
	intmax_t yl = pop().round().toLong();
	intmax_t mask = getBitMask();
	xl &= mask;
	yl &= mask;
	intmax_t v = xl ^ yl;
	v &= mask;
	push(AF(v));
	return NoError;
}

ErrorCode Stack::bitwisenot()
{
	intmax_t xl = pop().round().toLong();
	intmax_t mask = getBitMask();
	xl &= mask;
	xl = ~xl;
	xl &= mask;
	push(AF(xl));
	return NoError;
}

ErrorCode Stack::twoscomplement()
{
	intmax_t xl = pop().round().toLong();
	intmax_t mask = getBitMask();
	xl &= mask;
	xl = ~xl;
	xl += 1;
	xl &= mask;
	push(AF(xl));
	return NoError;
}

ErrorCode Stack::absolute()
{
	push(pop().abs());
	return NoError;
}

ErrorCode Stack::log10()
{
	AF x = pop();
	if (x <= 0.0) {
		push(x);
		return InvalidLog;
	}
	else {
		push(x.log10());
	}
	return NoError;
}

ErrorCode Stack::loge()
{
	AF x = pop();
	if (x <= 0.0) {
		push(x);
		return InvalidLog;
	}
	else {
		push(x.log());
	}
	return NoError;
}

ErrorCode Stack::log2()
{
	AF x = pop();
	if (x <= 0.0) {
		push(x);
		return InvalidLog;
	}
	else {
		AF two = AF(2.0);
		push(x.log() / two.log());
	}
	return NoError;
}

ErrorCode Stack::ceiling()
{
	push(pop().ceil());
	return NoError;
}

ErrorCode Stack::floor()
{
	push(pop().floor());
	return NoError;
}

ErrorCode Stack::cos()
{
	AF x = pop();
	if ( ! options.contains(Radians)) {
		x = x * AF::pi() / AF(180);
	}
	push(x.cos());
	return NoError;
}

ErrorCode Stack::sin()
{
	AF x = pop();
	if ( ! options.contains(Radians)) {
		x = x * AF::pi() / AF(180);
	}
	push(x.sin());
	return NoError;
}

ErrorCode Stack::tan()
{
	AF x = pop();
	AF xr = x;
	if ( ! options.contains(Radians)) {
		xr = xr * AF::pi() / AF(180);
	}
	if ((xr.remainder(AF::pi()/2.0) == AF(0.0)) && ((xr/(AF::pi()/2.0)).remainder(2.0) == AF(1.0))) {
		push(x);
		return InvalidTan;
	}
	else {
		push(xr.tan());
	}
	return NoError;
}

ErrorCode Stack::inversecos()
{
	AF x = pop();
	if ((x < -1.0) || (x > 1.0)) {
		push(x);
		return InvalidInverseTrig;
	}
	else {
		AF v = x.acos();
		if ( ! options.contains(Radians)) {
			v = v * AF(180)/AF::pi();
		}
		push(v);
	}
	return NoError;
}

ErrorCode Stack::inversesin()
{
	AF x = pop();
	if ((x < -1.0) || (x > 1.0)) {
		push(x);
		return InvalidInverseTrig;
	}
	else {
		AF v = x.asin();
		if ( ! options.contains(Radians)) {
			v = v * AF(180)/AF::pi();
		}
		push(v);
	}
	return NoError;
}

ErrorCode Stack::inversetan()
{
	AF x = pop();
	AF v = x.atan();
	if ( ! options.contains(Radians)) {
		v = v * AF(180)/AF::pi();
	}
	push(v);
	return NoError;
}

ErrorCode Stack::inversetan2()
{
	AF x = pop();
	AF y = pop();

	if ((x == AF(0)) && (y == AF(0))) {
		push(y);
		push(x);
		return DivideByZero;
	}
	else {
		AF v;
		if (x > 0.0) {
			v = y / x;
			v = v.atan();
		}
		else if ((x < 0.0) && (y >= 0.0)) {
			v = y / x;
			v = v.atan() + AF::pi();
		}
		else if ((x < 0.0) && (y < 0.0)) {
			v = y / x;
			v = v.atan() - AF::pi();
		}
		else if ((x == AF(0)) && (y > 0.0)) {
			v = AF::pi()/2.0;
		}
		else if ((x == AF(0)) && (y < 0.0)) {
			v = AF(0.0)-AF::pi()/2.0;
		}
		else {
			// Shouldn't get here, but just in case:
			push(y);
			push(x);
			return DivideByZero;
		}

		if ( ! options.contains(Radians)) {
			v = v * AF(180)/AF::pi();
		}
		push(v);
	}
	return NoError;
}

ErrorCode Stack::cosh()
{
	AF x = pop();
	push(x.cosh());
	return NoError;
}

ErrorCode Stack::sinh()
{
	AF x = pop();
	push(x.sinh());
	return NoError;
}

ErrorCode Stack::tanh()
{
	AF x = pop();
	push(x.tanh());
	return NoError;
}

ErrorCode Stack::inversecosh()
{
	/* Implementation from kotlin:
	AF x = pop();
	AF v1 = (x * x) - 1.0;
	if (v1 < 0.0) {
		push(x);
		return InvalidInverseHypTrig;
	}
	v1 = x + v1.sqrt();
	if (v1 <= 0.0) {
		push(x);
		return InvalidInverseHypTrig;
	}
	push(v1.log());
	*/
	AF x = pop();
	push(x.acosh());
	return NoError;
}

ErrorCode Stack::inversesinh()
{
	/* Implementation from kotlin:
	AF x = pop()
	AF v1(1.0+(x*x));
	v1 = v1.sqrt();
	if (v1 <= 0.0) {
		push(x)
		return InvalidInverseHypTrig;
	}
	push(v1.log());
	*/
	AF x = pop();
	push(x.asinh());
	return NoError;
}

ErrorCode Stack::inversetanh()
{
	/* Implementation from kotlin:
	AF x = pop();
	if (x == AF(1.0)) {
		push(x);
		return DivideByZero;
	}
	AF v1((1.0+x)/(1.0-x));
	if (v1 <= 0.0) {
		push(x);
		return InvalidInverseHypTrig;
	}
	push(0.5*v1.log());
	*/

	AF x = pop();
	push(x.atanh());
	return NoError;
}

ErrorCode Stack::inversetanh2()
{
	AF x = pop();
	if (x == AF(0.0)) {
		push(x);
		return DivideByZero;
	}
	AF y = pop();
	AF v1 = y / x;
	if (v1 == AF(1.0)) {
		push(y);
		push(x);
		return DivideByZero;
	}
	v1 = (AF(1.0) + v1) / (AF(1.0) - v1);
	if (v1 <= 0.0) {
		push(y);
		push(x);
		return InvalidInverseHypTrig;
	}
	push(AF(0.5)*v1.log());
	return NoError;
}

ErrorCode Stack::round()
{
	push(pop().round());
	return NoError;
}

ErrorCode Stack::squareroot()
{
	push(pop().sqrt());
	return NoError;
}

ErrorCode Stack::cuberoot()
{
	push(pop().cbrt());
	return NoError;
}

ErrorCode Stack::power()
{
	AF x = pop();
	AF y = pop();
	push(y.pow(x));
	return NoError;;
}

ErrorCode Stack::swap()
{
	AF x = pop();
	AF y = pop();
	push(x);
	push(y);
	return NoError;
}

ErrorCode Stack::duplicate()
{
	push(peek());
	return NoError;
}

ErrorCode Stack::remainder()
{
	AF x = pop();
	if (x == AF(0.0)) {
		push(x);
		return DivideByZero;
	}
	else {
		AF y = pop();
		push(y.remainder(x));
	}
	return NoError;
}

ErrorCode Stack::drop()
{
	pop();
	return NoError;
}

ErrorCode Stack::convertHmsToHours()
{
	AF hours, minutes, seconds;
	int sign;
	AF x = pop();
	if (x >= 0.0) {
		hours = x.floor();
		x.reduce_precision();
		minutes = x - hours;
		sign = 1;
	}
	else {
		hours = x.ceil();
		x.reduce_precision();
		minutes = hours - x;
		sign = -1;
	}
	minutes = minutes * 100;
	minutes.reduce_precision();
	seconds = minutes - minutes.floor();

	minutes = minutes.floor();
	seconds = seconds * 100;
	seconds.reduce_precision();
	push(hours + (AF(sign)*((minutes+(seconds/60.0))/60.0)));
	return NoError;
}

ErrorCode Stack::convertHmToHours()
{
	AF hours, minutes;
	int sign;
	AF x = pop();
	if (x >= AF("0.0")) {
		hours = x.floor();
		x.reduce_precision();
		minutes = x - hours;
		sign = 1;
	}
	else {
		hours = x.ceil();
		x.reduce_precision();
		minutes = hours - x;
		sign = -1;
	}
	minutes = minutes * AF("100.0");
	push(hours + (AF(sign)*(minutes / AF("60.0"))));
	return NoError;
}

ErrorCode Stack::convertHoursToHms()
{
	AF parthours, hours, minutes, seconds;
	int sign;
	AF x = pop();
	if (x >= AF("0.0")) {
		hours = x.floor();
		x.reduce_precision();
		parthours = x - hours;
		sign = 1;
	}
	else {
		hours = x.ceil();
		x.reduce_precision();
		parthours = hours - x;
		sign = -1;
	}
	minutes = parthours * AF("60.0");
	seconds = minutes - minutes.floor();
	seconds = seconds * AF("60.0");
	minutes = minutes.floor();
	while (seconds >= AF("60.0")) {
		seconds = seconds - AF("60.0");
		minutes = minutes + AF("1.0");
	}
	while (minutes >= AF("60.0")) {
		minutes = minutes - AF("60.0");
		hours = hours + AF(sign);
	}
	push(hours + (AF(sign)*((minutes + (seconds / AF("100.0")))/AF("100.0"))));
	return NoError;
}

ErrorCode Stack::convertHoursToHm()
{
	AF parthours, hours, minutes;
	int sign;
	AF x = pop();
	if (x >= 0.0) {
		hours = x.floor();
		x.reduce_precision();
		parthours = x - hours;
		sign = 1;
	}
	else {
		hours = x.ceil();
		x.reduce_precision();
		parthours = hours - x;
		sign = -1;
	}
	minutes = parthours * 60.0;
	while (minutes >= 60.0) {
		minutes = minutes - 60.0;
		hours = hours + AF(sign);
	}
	push(hours + (AF(sign)*(minutes / 100.0)));
	return NoError;
}

void Stack::populateConstants()
{
	constants.clear();
	constants.push_back(
		Constant{.name="Pi",                         .symbol="&pi;",                                  .value=AF::pi(),             .unit="m&nbsp;s<sup><small>-1</small></sup>",                                                            .category="Universal"});
	constants.push_back(
		Constant{.name="Euler's Number",             .symbol="e<sup><small>1</small></sup>",          .value=AF::e(),              .unit="m&nbsp;s<sup><small>-1</small></sup>",                                                            .category="Universal"});
	constants.push_back(
		Constant{.name="Speed of Light in Vacuum",   .symbol="c",                                     .value=AF("299792458.0"),      .unit="m&nbsp;s<sup><small>-1</small></sup>",                                                            .category="Universal"});
	constants.push_back(
		Constant{.name="Gravitational Constant",     .symbol="G",                                     .value=AF("6.67408e11"),       .unit="m<sup><small>3</small></sup>&nbsp;kg<sup><small>-1</small></sup>&nbsp;s<sup><small>-2</small></sup>",  .category="Universal"});
	constants.push_back(
		Constant{.name="Planck Constant",            .symbol="h",                                     .value=AF("6.62607004e-34"),   .unit="J&nbsp;s",                                                                                        .category="Universal"});
	constants.push_back(
		Constant{.name="Reduced Planck Constant",    .symbol="&#295;",                                .value=AF("1.0545718e-34"),    .unit="J&nbsp;s",                                                                                        .category="Universal"});
	constants.push_back(
		Constant{.name="Permeability of Free Space", .symbol="&mu;<sub><small>0</small></sub>",       .value=AF("1.256637061e-6"),   .unit="N&nbsp;A<sup><small>-2</small></sup>",                                                            .category="Electromagnetic"});
	constants.push_back(
		Constant{.name="Permittivity of Free Space", .symbol="&epsilon;<sub><small>0</small></sub>",  .value=AF("8.854187818e-12"),  .unit="F&nbsp;m<sup><small>-1</small></sup>",                                                            .category="Electromagnetic"});
	constants.push_back(
		Constant{.name="Elementary Charge",          .symbol="e",                                     .value=AF("1.602176621e-19"),  .unit="C",                                                                                          .category="Electromagnetic"});
	constants.push_back(
		Constant{.name="Magnetic Flux Quantum",      .symbol="&phi;<sub><small>0</small></sub>",      .value=AF("2.067833831e-15"),  .unit="Wb",                                                                                         .category="Electromagnetic"});
	constants.push_back(
		Constant{.name="Conductance Quantum",        .symbol="G<sub><small>0</small></sub>",          .value=AF("7.748091731e-5"),   .unit="S",                                                                                          .category="Electromagnetic"});
	constants.push_back(
		Constant{.name="Electron Mass",              .symbol="m<sub><small>e</small></sub>",          .value=AF("9.10938356e-31"),   .unit="kg",                                                                                         .category="Atomic"});
	constants.push_back(
		Constant{.name="Proton Mass",                .symbol="m<sub><small>p</small></sub>",          .value=AF("1.672621898e-27"),  .unit="kg",                                                                                         .category="Atomic"});
	constants.push_back(
		Constant{.name="Fine Structure Constant",    .symbol="&alpha;",                               .value=AF("0.007297353"),      .unit="",                                                                                           .category="Atomic"});
	constants.push_back(
		Constant{.name="Rydberg Constant",           .symbol="R<sub><small>&infin;</small></sub>",    .value=AF("10973731.57"),      .unit="m<sup><small>-1</small></sup>",                                                              .category="Atomic"});
	constants.push_back(
		Constant{.name="Bohr Radius",                .symbol="a<sub><small>0</small></sub>",          .value=AF("5.291772107e-11"),  .unit="m",                                                                                          .category="Atomic"});
	constants.push_back(
		Constant{.name="Classical Electron Radius",  .symbol="r<sub><small>e</small></sub>",          .value=AF("2.817940323e-15"),  .unit="m",                                                                                          .category="Atomic"});
	constants.push_back(
		Constant{.name="Atomic Mass Unit",           .symbol="u",                                     .value=AF("1.66053904e-27"),   .unit="kg",                                                                                         .category="Physical"});
	constants.push_back(
		Constant{.name="Avogadro Constant",          .symbol="N<sub><small>A</small></sub>",          .value=AF("6.022140857e23"),   .unit="mol<sup><small>-1</small></sup>",                                                            .category="Physical"});
	constants.push_back(
		Constant{.name="Faraday Constant",           .symbol="F",                                     .value=AF("96485.33289"),      .unit="C&nbsp;mol<sup><small>-1</small></sup>",                                                          .category="Physical"});
	constants.push_back(
		Constant{.name="Molar Gas Constant",         .symbol="R",                                     .value=AF("8.3144598"),        .unit="J&nbsp;mol<sup><small>-1</small></sup>&nbsp;K<sup><small>-1</small></sup>",                            .category="Physical"});
	constants.push_back(
		Constant{.name="Boltzmann Constant",         .symbol="k",                                     .value=AF("1.38064852e-23"),   .unit="J&nbsp;K<sup><small>-1</small></sup>",                                                            .category="Physical"});
	constants.push_back(
		Constant{.name="Stefan-Boltzmann Constant",  .symbol="&sigma;",                               .value=AF("5.670367e-8"),      .unit="W&nbsp;m<sup><small>-2</small></sup>&nbsp;K<sup><small>-4</small></sup>",                              .category="Physical"});
	constants.push_back(
		Constant{.name="Electron Volt",              .symbol="eV",                                    .value=AF("1.602176621e-19"),  .unit="J",                                                                                          .category="Physical"});
	constants.push_back(
		Constant{.name="Standard Gravity",           .symbol="g<sub><small>0</small></sub>",          .value=AF("9.80665"),          .unit="m&nbsp;s<sup><small>-2</small></sup>",                                                            .category="Other"});
}

ErrorCode Stack::constant(std::string name)
{
	std::regex re(" ");

	for (Constant constant: constants) {
		if ((constant.name == name) || (std::regex_replace(constant.name, re, "") == name)) {
			push(constant.value);
			return NoError;
		}
	}
	return UnknownConstant;
}

void Stack::populateDensities()
{
	densities.clear();

	densities.push_back(Density{.name="Aluminium Bronze",       .value=AF("7700"), .category="Metal"});
	densities.push_back(Density{.name="Aluminium",              .value=AF("2700"), .category="Metal"});
	densities.push_back(Density{.name="Antimony",               .value=AF("6700"), .category="Metal"});
	densities.push_back(Density{.name="Beryllium",              .value=AF("1850"), .category="Metal"});
	densities.push_back(Density{.name="Bismuth",                .value=AF("9800"), .category="Metal"});
	densities.push_back(Density{.name="Brass",                  .value=AF("8610"), .category="Metal"});
	densities.push_back(Density{.name="Bronze",                 .value=AF("8815"), .category="Metal"});
	densities.push_back(Density{.name="Cadmium",                .value=AF("8640"), .category="Metal"});
	densities.push_back(Density{.name="Cast Iron",              .value=AF("7200"), .category="Metal"});
	densities.push_back(Density{.name="Chromium",               .value=AF("7100"), .category="Metal"});
	densities.push_back(Density{.name="Cobalt",                 .value=AF("8800"), .category="Metal"});
	densities.push_back(Density{.name="Copper",                 .value=AF("8790"), .category="Metal"});
	densities.push_back(Density{.name="Gallium",                .value=AF("5900"), .category="Metal"});
	densities.push_back(Density{.name="Gold",                   .value=AF("19290"), .category="Metal"});
	densities.push_back(Density{.name="Lead",                   .value=AF("11350"), .category="Metal"});
	densities.push_back(Density{.name="Lithium",                .value=AF("530"), .category="Metal"});
	densities.push_back(Density{.name="Magnesium",              .value=AF("1740"), .category="Metal"});
	densities.push_back(Density{.name="Manganese",              .value=AF("7430"), .category="Metal"});
	densities.push_back(Density{.name="Molybdenum",             .value=AF("10200"), .category="Metal"});
	densities.push_back(Density{.name="Nickel",                 .value=AF("8900"), .category="Metal"});
	densities.push_back(Density{.name="Osmium",                 .value=AF("22480"), .category="Metal"});
	densities.push_back(Density{.name="Palladium",              .value=AF("12000"), .category="Metal"});
	densities.push_back(Density{.name="Phosphor Bronze",        .value=AF("8800"), .category="Metal"});
	densities.push_back(Density{.name="Phosphorus",             .value=AF("1820"), .category="Metal"});
	densities.push_back(Density{.name="Potassium",              .value=AF("860"), .category="Metal"});
	densities.push_back(Density{.name="Silver",                 .value=AF("10500"), .category="Metal"});
	densities.push_back(Density{.name="Sodium",                 .value=AF("980"), .category="Metal"});
	densities.push_back(Density{.name="Steel",                  .value=AF("7820"), .category="Metal"});
	densities.push_back(Density{.name="Tantalum",               .value=AF("16600"), .category="Metal"});
	densities.push_back(Density{.name="Tin",                    .value=AF("7280"), .category="Metal"});
	densities.push_back(Density{.name="Titanium",               .value=AF("4500"), .category="Metal"});
	densities.push_back(Density{.name="Tungsten Carbide",       .value=AF("14500"), .category="Metal"});
	densities.push_back(Density{.name="Tungsten",               .value=AF("19200"), .category="Metal"});
	densities.push_back(Density{.name="Uranium",                .value=AF("19100"), .category="Metal"});
	densities.push_back(Density{.name="Vanadium",               .value=AF("6100"), .category="Metal"});
	densities.push_back(Density{.name="Zinc",                   .value=AF("7120"), .category="Metal"});
	densities.push_back(Density{.name="Agate",                  .value=AF("2600"), .category="Mineral"});
	densities.push_back(Density{.name="Amber",                  .value=AF("1080"), .category="Mineral"});
	densities.push_back(Density{.name="Basalt",                 .value=AF("2750"), .category="Mineral"});
	densities.push_back(Density{.name="Bauxite",                .value=AF("1280"), .category="Mineral"});
	densities.push_back(Density{.name="Borax",                  .value=AF("850"), .category="Mineral"});
	densities.push_back(Density{.name="Brick, Fire",            .value=AF("2300"), .category="Mineral"});
	densities.push_back(Density{.name="Brick",                  .value=AF("1900"), .category="Mineral"});
	densities.push_back(Density{.name="Calcium",                .value=AF("1550"), .category="Mineral"});
	densities.push_back(Density{.name="Carbon",                 .value=AF("3510"), .category="Mineral"});
	densities.push_back(Density{.name="Clay",                   .value=AF("2200"), .category="Mineral"});
	densities.push_back(Density{.name="Diamond",                .value=AF("3250"), .category="Mineral"});
	densities.push_back(Density{.name="Flint",                  .value=AF("2600"), .category="Mineral"});
	densities.push_back(Density{.name="Glass",                  .value=AF("2600"), .category="Mineral"});
	densities.push_back(Density{.name="Granite",                .value=AF("2700"), .category="Mineral"});
	densities.push_back(Density{.name="Graphite",               .value=AF("2500"), .category="Mineral"});
	densities.push_back(Density{.name="Marble",                 .value=AF("2700"), .category="Mineral"});
	densities.push_back(Density{.name="Opal",                   .value=AF("2200"), .category="Mineral"});
	densities.push_back(Density{.name="Pyrex",                  .value=AF("2250"), .category="Mineral"});
	densities.push_back(Density{.name="Quartz",                 .value=AF("2650"), .category="Mineral"});
	densities.push_back(Density{.name="Silicon",                .value=AF("2330"), .category="Mineral"});
	densities.push_back(Density{.name="Slate",                  .value=AF("2950"), .category="Mineral"});
	densities.push_back(Density{.name="Sulphur",                .value=AF("2000"), .category="Mineral"});
	densities.push_back(Density{.name="Topaz",                  .value=AF("3550"), .category="Mineral"});
	densities.push_back(Density{.name="Beeswax",                .value=AF("960"), .category="Other"});
	densities.push_back(Density{.name="Cardboard",              .value=AF("700"), .category="Other"});
	densities.push_back(Density{.name="Leather",                .value=AF("860"), .category="Other"});
	densities.push_back(Density{.name="Paper",                  .value=AF("925"), .category="Other"});
	densities.push_back(Density{.name="Paraffin",               .value=AF("900"), .category="Other"});
	densities.push_back(Density{.name="ABS",                    .value=AF("1060"), .category="Plastic"});
	densities.push_back(Density{.name="Acetal",                 .value=AF("1420"), .category="Plastic"});
	densities.push_back(Density{.name="Acrylic",                .value=AF("1190"), .category="Plastic"});
	densities.push_back(Density{.name="Bakelite",               .value=AF("1360"), .category="Plastic"});
	densities.push_back(Density{.name="Epoxy Cast Resin",       .value=AF("1255"), .category="Plastic"});
	densities.push_back(Density{.name="Expanded Polystyrene",   .value=AF("22"), .category="Plastic"});
	densities.push_back(Density{.name="HDPE",                   .value=AF("960"), .category="Plastic"});
	densities.push_back(Density{.name="LDPE",                   .value=AF("910"), .category="Plastic"});
	densities.push_back(Density{.name="Nylon",                  .value=AF("1145"), .category="Plastic"});
	densities.push_back(Density{.name="PBT",                    .value=AF("1350"), .category="Plastic"});
	densities.push_back(Density{.name="PET",                    .value=AF("1350"), .category="Plastic"});
	densities.push_back(Density{.name="PMMA",                   .value=AF("1200"), .category="Plastic"});
	densities.push_back(Density{.name="POM",                    .value=AF("1400"), .category="Plastic"});
	densities.push_back(Density{.name="PP",                     .value=AF("925"), .category="Plastic"});
	densities.push_back(Density{.name="PS",                     .value=AF("1030"), .category="Plastic"});
	densities.push_back(Density{.name="PTFE",                   .value=AF("2290"), .category="Plastic"});
	densities.push_back(Density{.name="PU",                     .value=AF("30"), .category="Plastic"});
	densities.push_back(Density{.name="PVC",                    .value=AF("1405"), .category="Plastic"});
	densities.push_back(Density{.name="Poly Carbonate",         .value=AF("1200"), .category="Plastic"});
	densities.push_back(Density{.name="Teflon",                 .value=AF("2200"), .category="Plastic"});
	densities.push_back(Density{.name="Alder",                  .value=AF("550"), .category="Wood"});
	densities.push_back(Density{.name="Apple",                  .value=AF("750"), .category="Wood"});
	densities.push_back(Density{.name="Ash, European",          .value=AF("710"), .category="Wood"});
	densities.push_back(Density{.name="Aspen",                  .value=AF("420"), .category="Wood"});
	densities.push_back(Density{.name="Balsa",                  .value=AF("125"), .category="Wood"});
	densities.push_back(Density{.name="Bamboo",                 .value=AF("355"), .category="Wood"});
	densities.push_back(Density{.name="Beech",                  .value=AF("800"), .category="Wood"});
	densities.push_back(Density{.name="Birch",                  .value=AF("640"), .category="Wood"});
	densities.push_back(Density{.name="Box",                    .value=AF("1055"), .category="Wood"});
	densities.push_back(Density{.name="Cedar of Lebanon",       .value=AF("580"), .category="Wood"});
	densities.push_back(Density{.name="Cedar, Western Red",     .value=AF("380"), .category="Wood"});
	densities.push_back(Density{.name="Cherry",                 .value=AF("630"), .category="Wood"});
	densities.push_back(Density{.name="Chestnut, Sweet",        .value=AF("560"), .category="Wood"});
	densities.push_back(Density{.name="Cypress",                .value=AF("510"), .category="Wood"});
	densities.push_back(Density{.name="Douglas Fir",            .value=AF("530"), .category="Wood"});
	densities.push_back(Density{.name="Ebony",                  .value=AF("1220"), .category="Wood"});
	densities.push_back(Density{.name="Elm",                    .value=AF("570"), .category="Wood"});
	densities.push_back(Density{.name="Greenheart",             .value=AF("1040"), .category="Wood"});
	densities.push_back(Density{.name="Hemlock, Western",       .value=AF("500"), .category="Wood"});
	densities.push_back(Density{.name="Hickory",                .value=AF("765"), .category="Wood"});
	densities.push_back(Density{.name="Holly",                  .value=AF("760"), .category="Wood"});
	densities.push_back(Density{.name="Iroko",                  .value=AF("660"), .category="Wood"});
	densities.push_back(Density{.name="Juniper",                .value=AF("560"), .category="Wood"});
	densities.push_back(Density{.name="Larch",                  .value=AF("530"), .category="Wood"});
	densities.push_back(Density{.name="Lignum Vitae",           .value=AF("1250"), .category="Wood"});
	densities.push_back(Density{.name="Lime, European",         .value=AF("560"), .category="Wood"});
	densities.push_back(Density{.name="Magnolia",               .value=AF("570"), .category="Wood"});
	densities.push_back(Density{.name="Mahogany",               .value=AF("675"), .category="Wood"});
	densities.push_back(Density{.name="Maple",                  .value=AF("685"), .category="Wood"});
	densities.push_back(Density{.name="Meranti",                .value=AF("710"), .category="Wood"});
	densities.push_back(Density{.name="Myrtle",                 .value=AF("660"), .category="Wood"});
	densities.push_back(Density{.name="Oak",                    .value=AF("750"), .category="Wood"});
	densities.push_back(Density{.name="Pear",                   .value=AF("670"), .category="Wood"});
	densities.push_back(Density{.name="Pecan",                  .value=AF("770"), .category="Wood"});
	densities.push_back(Density{.name="Pine, Pitch",            .value=AF("840"), .category="Wood"});
	densities.push_back(Density{.name="Pine, Scots",            .value=AF("510"), .category="Wood"});
	densities.push_back(Density{.name="Pine, White",            .value=AF("425"), .category="Wood"});
	densities.push_back(Density{.name="Pine, Yellow",           .value=AF("420"), .category="Wood"});
	densities.push_back(Density{.name="Plane, European",        .value=AF("640"), .category="Wood"});
	densities.push_back(Density{.name="Plum",                   .value=AF("720"), .category="Wood"});
	densities.push_back(Density{.name="Plywood",                .value=AF("540"), .category="Wood"});
	densities.push_back(Density{.name="Poplar",                 .value=AF("425"), .category="Wood"});
	densities.push_back(Density{.name="Redwood, American",      .value=AF("450"), .category="Wood"});
	densities.push_back(Density{.name="Redwood, European",      .value=AF("510"), .category="Wood"});
	densities.push_back(Density{.name="Rosewood, Bolivian",     .value=AF("820"), .category="Wood"});
	densities.push_back(Density{.name="Rosewood, East Indian",  .value=AF("900"), .category="Wood"});
	densities.push_back(Density{.name="Sapele",                 .value=AF("640"), .category="Wood"});
	densities.push_back(Density{.name="Spruce",                 .value=AF("450"), .category="Wood"});
	densities.push_back(Density{.name="Sycamore",               .value=AF("500"), .category="Wood"});
	densities.push_back(Density{.name="Teak, Indian",           .value=AF("820"), .category="Wood"});
	densities.push_back(Density{.name="Teak, African",          .value=AF("980"), .category="Wood"});
	densities.push_back(Density{.name="Teak, Burma",            .value=AF("740"), .category="Wood"});
	densities.push_back(Density{.name="Utile",                  .value=AF("660"), .category="Wood"});
	densities.push_back(Density{.name="Walnut",                 .value=AF("670"), .category="Wood"});
	densities.push_back(Density{.name="Walnut, American Black", .value=AF("630"), .category="Wood"});
	densities.push_back(Density{.name="Walnut, European",       .value=AF("570"), .category="Wood"});
	densities.push_back(Density{.name="Willow",                 .value=AF("500"), .category="Wood"});
	densities.push_back(Density{.name="Yew",                    .value=AF("670"), .category="Wood"});
	densities.push_back(Density{.name="Zebrawood",              .value=AF("790"), .category="Wood"});
}

ErrorCode Stack::density(std::string name)
{
	std::regex re(" ");

	for (Density density: densities) {
		if ((density.name == name) || (std::regex_replace(density.name, re, "") == name)) {
			push(density.value);
			return NoError;
		}
	}
	return UnknownConstant;
}
