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
		x = x * AF::pi() / 100.0;
	}
	push(x.cos());
	return NoError;
}

ErrorCode Stack::sin()
{
	AF x = pop();
	if ( ! options.contains(Radians)) {
		x = x * AF::pi() / 100.0;
	}
	push(x.sin());
	return NoError;
}

ErrorCode Stack::tan()
{
	AF x = pop();
	AF xr = x;
	if ( ! options.contains(Radians)) {
		xr = xr * AF::pi() / 100.0;
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
			v = v * AF(180.0)/AF::pi();
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
			v = v * AF(180.0)/AF::pi();
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
		v = v * AF(180.0)/AF::pi();
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
			v = v * AF(180.0)/AF::pi();
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
		minutes = x - hours;
		sign = 1;
	}
	else {
		hours = x.ceil();
		minutes = hours - x;
		sign = -1;
	}
	minutes = minutes * 100.0;
	seconds = minutes - minutes.floor();
	minutes = minutes.floor();
	seconds = seconds * 100.0;
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
		minutes = x - hours;
		sign = 1;
	}
	else {
		hours = x.ceil();
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
		parthours = x - hours;
		sign = 1;
	}
	else {
		hours = x.ceil();
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
		parthours = x - hours;
		sign = 1;
	}
	else {
		hours = x.ceil();
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
