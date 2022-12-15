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
#ifndef ARPFLOAT_H
#define ARPFLOAT_H

#include <string>
#include <mpfr.h>

class AF
{
	public:
		~AF();
		AF();
		AF(const AF& v);
		AF(double v);
		AF(long double v);
		AF(int v);
		AF(intmax_t v);
		AF(std::string v);

		AF& operator=(AF other);

		bool equals(const int other) const;
		bool equals(const int64_t other) const;
		bool equals(const double other) const;
		bool equals(const std::string other) const;
		bool equals(const AF other) const;

		int compareTo(const int other) const;
		int compareTo(const int64_t other) const;
		int compareTo(const double other) const;
		int compareTo(const std::string other) const;
		int compareTo(const AF other) const;

		AF unaryMinus();

		AF minus(AF b);
		AF minus(double b);

		AF times(AF b);
		AF times(double b);

		AF plus(AF b);
		AF plus(double b);

		AF div(AF b);
		AF div(double b);

		AF rem(AF b);
		AF rem(double b);

		AF remainder(AF b);
		AF remainder(double b);

		AF round();
		AF floor();
		AF ceil();
		AF pow(AF x);

		AF sqrt();
		AF cbrt();
		AF root(AF p);
		AF root(double p);

		AF cos();
		AF sin();
		AF tan();
		AF acos();
		AF asin();
		AF atan();

		AF cosh();
		AF sinh();
		AF tanh();
		AF acosh();
		AF asinh();
		AF atanh();

		AF abs();

		AF log();
		AF log10();

		double toDouble();
		intmax_t toLong();
		int toInt();

		std::string toString();

		void debugPrint();

		static AF from(int64_t v);
		static AF from(int v);
		static AF from(double v);
		static AF from(std::string v);
		static AF pi();
		static AF e();
		static bool isValidString(std::string s);

		mpfr_t vptr;

		mp_rnd_t rounding_mode = mpfr_get_default_rounding_mode();
		mp_prec_t precision = mpfr_get_default_prec();

		AF operator+(const AF & b);
		AF operator-(const AF & b);
		AF operator/(const AF & b);
		AF operator*(const AF & b);
		bool operator==(const AF & b) const;
		bool operator!=(const AF & b) const;
		bool operator<(const AF & b) const;
		bool operator>(const AF & b) const;
		bool operator<=(const AF & b) const;
		bool operator>=(const AF & b) const;

		AF operator+(double & b);
		AF operator-(double & b);
		AF operator/(double & b);
		AF operator*(double & b);
		bool operator==(double & b) const;
		bool operator!=(double & b) const;
		bool operator<(double & b) const;
		bool operator>(double & b) const;
		bool operator<=(double & b) const;
		bool operator>=(double & b) const;

	private:
		void init_mpfr();
		void set_precision(int digits);
};

#endif
