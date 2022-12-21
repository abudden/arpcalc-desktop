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
#include "arpfloat.h"
#include <stdio.h>
#include <cmath> // std::ceil


void AF::init_mpfr()
{
	const int digits = 256;
	set_precision(digits);
	mpfr_init2(vptr, precision);
}

void AF::set_precision(int digits)
{
	// This sets the class variable appropriately; it doesn't
	// change the actual precision of the internal variable
	// as that has to be done at initialisation
	const double log2_10 = 3.32192809488736262580;
	precision = std::ceil(digits * log2_10);
}

AF::AF()
{
	init_mpfr();
	mpfr_set_d(vptr, 0, rounding_mode);
}

AF::AF(const AF& v)
{
	init_mpfr();
	mpfr_set(vptr, v.vptr, rounding_mode);
}

AF::AF(double v)
{
	init_mpfr();
	mpfr_set_d(vptr, v, rounding_mode);
}

AF::AF(long double v)
{
	init_mpfr();
	mpfr_set_ld(vptr, v, rounding_mode);
}

AF::AF(int v)
{
	init_mpfr();
	mpfr_set_d(vptr, (double) v, rounding_mode);
}

AF::AF(intmax_t v)
{
	init_mpfr();
	mpfr_set_sj(vptr, v, rounding_mode);
}

AF::AF(std::string v)
{
	init_mpfr();
	mpfr_set_str(vptr, v.c_str(), 10, rounding_mode);
}

AF::~AF()
{
	mpfr_clear(vptr);
}

AF& AF::operator=(AF other)
{
	mpfr_swap(vptr, other.vptr);
	return *this;
}

bool AF::equals(const int other) const
{
	return compareTo(other) == 0;
}

bool AF::equals(const intmax_t other) const
{
	return compareTo(other) == 0;
}

bool AF::equals(const double other) const
{
	return compareTo(other) == 0;
}

bool AF::equals(const std::string other) const
{
	return compareTo(other) == 0;
}

bool AF::equals(const AF other) const
{
	return compareTo(other) == 0;
}

int AF::compareTo(const int other) const
{
	return compareTo(AF(other));
}

int AF::compareTo(const intmax_t other) const
{
	return compareTo(AF(other));
}

int AF::compareTo(const double other) const
{
	return compareTo(AF(other));
}

int AF::compareTo(const std::string other) const
{
	return compareTo(AF(other));
}

int AF::compareTo(const AF other) const
{
	return mpfr_cmp(vptr, other.vptr);
}

AF AF::unaryMinus()
{
	AF zero(0);
	AF r;
	mpfr_sub(r.vptr, zero.vptr, vptr, rounding_mode);
	return r;
}

AF AF::minus(AF b)
{
	AF r;
	mpfr_sub(r.vptr, vptr, b.vptr, rounding_mode);
	return r;
}

AF AF::minus(double b)
{
	AF r;
	AF ba(b);
	mpfr_sub(r.vptr, vptr, ba.vptr, rounding_mode);
	return r;
}


AF AF::times(AF b)
{
	AF r;
	mpfr_mul(r.vptr, vptr, b.vptr, rounding_mode);
	return r;
}

AF AF::times(double b)
{
	AF r;
	AF ba(b);
	mpfr_mul(r.vptr, vptr, ba.vptr, rounding_mode);
	return r;
}


AF AF::plus(AF b)
{
	AF r;
	mpfr_add(r.vptr, vptr, b.vptr, rounding_mode);
	return r;
}

AF AF::plus(double b)
{
	AF r;
	AF ba(b);
	mpfr_add(r.vptr, vptr, ba.vptr, rounding_mode);
	return r;
}


AF AF::div(AF b)
{
	AF r;
	mpfr_div(r.vptr, vptr, b.vptr, rounding_mode);
	return r;
}

AF AF::div(double b)
{
	AF r;
	AF ba(b);
	mpfr_div(r.vptr, vptr, ba.vptr, rounding_mode);
	return r;
}


AF AF::rem(AF b)
{
	AF r;
	mpfr_remainder(r.vptr, vptr, b.vptr, rounding_mode);
	return r;
}

AF AF::rem(double b)
{
	AF r;
	AF ba(b);
	mpfr_remainder(r.vptr, vptr, ba.vptr, rounding_mode);
	return r;
}


AF AF::remainder(AF b)
{
	return rem(b);
}

AF AF::remainder(double b)
{
	return rem(b);
}


AF AF::round()
{
	AF r;
	mpfr_round(r.vptr, vptr);
	return r;
}

void AF::reduce_precision()
{
	// Used in conversions where only small number of decimal places
	// are required.  This is a bit of a crude hack, but seems to work.
	// I mainly implemented this to deal with some strange rounding
	// effects associated with convertHoursToHms and convertHmsToHours
	precision -= 2;
	mpfr_prec_round(vptr, precision, rounding_mode);
}

AF AF::floor()
{
	AF r;
	mpfr_floor(r.vptr, vptr);
	return r;
}

AF AF::ceil()
{
	AF r;
	mpfr_ceil(r.vptr, vptr);
	return r;
}

AF AF::pow(AF x)
{
	AF r;
	mpfr_pow(r.vptr, vptr, x.vptr, rounding_mode);
	return r;
}


AF AF::sqrt()
{
	AF r;
	mpfr_sqrt(r.vptr, vptr, rounding_mode);
	return r;
}

AF AF::cbrt()
{
	AF r;
	mpfr_cbrt(r.vptr, vptr, rounding_mode);
	return r;
}

AF AF::root(AF p)
{
	AF r;
	AF one(1);
	AF power;
	power = one.div(p);
	mpfr_pow(r.vptr, vptr, power.vptr, rounding_mode);
	return r;
}

AF AF::root(double p)
{
	AF pa(p);
	return root(pa);
}


AF AF::cos()
{
	AF r;
	mpfr_cos(r.vptr, vptr, rounding_mode);
	return r;
}

AF AF::sin()
{
	AF r;
	mpfr_sin(r.vptr, vptr, rounding_mode);
	return r;
}

AF AF::tan()
{
	AF r;
	mpfr_tan(r.vptr, vptr, rounding_mode);
	return r;
}

AF AF::acos()
{
	AF r;
	mpfr_acos(r.vptr, vptr, rounding_mode);
	return r;
}

AF AF::asin()
{
	AF r;
	mpfr_asin(r.vptr, vptr, rounding_mode);
	return r;
}

AF AF::atan()
{
	AF r;
	mpfr_atan(r.vptr, vptr, rounding_mode);
	return r;
}


AF AF::cosh()
{
	AF r;
	mpfr_cosh(r.vptr, vptr, rounding_mode);
	return r;
}

AF AF::sinh()
{
	AF r;
	mpfr_sinh(r.vptr, vptr, rounding_mode);
	return r;
}

AF AF::tanh()
{
	AF r;
	mpfr_tanh(r.vptr, vptr, rounding_mode);
	return r;
}

AF AF::acosh()
{
	AF r;
	mpfr_acosh(r.vptr, vptr, rounding_mode);
	return r;
}

AF AF::asinh()
{
	AF r;
	mpfr_asinh(r.vptr, vptr, rounding_mode);
	return r;
}

AF AF::atanh()
{
	AF r;
	mpfr_atanh(r.vptr, vptr, rounding_mode);
	return r;
}


AF AF::abs()
{
	AF r;
	mpfr_abs(r.vptr, vptr, rounding_mode);
	return r;
}


AF AF::log()
{
	AF r;
	mpfr_log(r.vptr, vptr, rounding_mode);
	return r;
}

AF AF::log10()
{
	AF r;
	mpfr_log10(r.vptr, vptr, rounding_mode);
	return r;
}


double AF::toDouble()
{
	return mpfr_get_d(vptr, rounding_mode);
}

intmax_t AF::toLong()
{
	return mpfr_get_sj(vptr, rounding_mode);
}

int AF::toInt()
{
	return (int) toLong();
}


std::string AF::toString()
{
	char format[] = "%.256RNg";
	char *buffer = NULL;
	std::string out;

	if ( ! (mpfr_asprintf(&buffer, format, vptr) < 0))
	{
		out = std::string(buffer);
		mpfr_free_str(buffer);
	}
	return out;
}

void AF::debugPrint()
{
	printf("Debug print value: ");
	mpfr_out_str(stdout, 10, 0, vptr, MPFR_RNDD);
	printf("\n");
}


AF AF::from(intmax_t v)
{
	return AF(v);
}

AF AF::from(int v)
{
	return AF(v);
}

AF AF::from(double v)
{
	return AF(v);
}

AF AF::from(std::string v)
{
	return AF(v);
}

AF AF::pi()
{
	AF r;
	mpfr_const_pi(r.vptr, r.rounding_mode);
	return r;
}

AF AF::e()
{
	AF r;
	AF one(1);
	mpfr_exp(r.vptr, one.vptr, r.rounding_mode);
	return r;
}


AF AF::operator+(const AF & b)
{
	return plus(b);
}

AF AF::operator-(const AF & b)
{
	return minus(b);
}

AF AF::operator/(const AF & b)
{
	return div(b);
}

AF AF::operator*(const AF & b)
{
	return times(b);
}

bool AF::operator==(const AF & b) const
{
	return equals(b);
}

bool AF::operator!=(const AF & b) const
{
	return ( ! equals(b));
}

bool AF::operator<(const AF & b) const
{
	if (compareTo(b) < 0) {
		return true;
	}
	else {
		return false;
	}
}

bool AF::operator>(const AF & b) const
{
	if (compareTo(b) > 0) {
		return true;
	}
	else {
		return false;
	}
}

bool AF::operator<=(const AF & b) const
{
	if (compareTo(b) > 0) {
		return false;
	}
	else {
		return true;
	}
}

bool AF::operator>=(const AF & b) const
{
	if (compareTo(b) < 0) {
		return false;
	}
	else {
		return true;
	}
}

AF AF::operator+(double & b)
{
	AF ba(b);
	return plus(b);
}

AF AF::operator-(double & b)
{
	AF ba(b);
	return minus(b);
}

AF AF::operator/(double & b)
{
	AF ba(b);
	return div(b);
}

AF AF::operator*(double & b)
{
	AF ba(b);
	return times(b);
}

bool AF::operator==(double & b) const
{
	return equals(b);
}

bool AF::operator!=(double & b) const
{
	return ( ! equals(b));
}

bool AF::operator<(double & b) const
{
	if (compareTo(b) < 0) {
		return true;
	}
	else {
		return false;
	}
}

bool AF::operator>(double & b) const
{
	if (compareTo(b) > 0) {
		return true;
	}
	else {
		return false;
	}
}

bool AF::operator<=(double & b) const
{
	if (compareTo(b) > 0) {
		return false;
	}
	else {
		return true;
	}
}

bool AF::operator>=(double & b) const
{
	if (compareTo(b) < 0) {
		return false;
	}
	else {
		return true;
	}
}

bool AF::isValidString(std::string s)
{
	AF test_af(0);

	int result = mpfr_set_str(test_af.vptr, s.c_str(), 10, mpfr_get_default_rounding_mode());

	if (result == 0) {
		return true;
	}

	return false;
}


