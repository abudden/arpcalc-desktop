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


Stack::Stack()
{
	populateConversionTable();
	populateConstants();

	// Seed the random number generator
	uint32_t seed_val = static_cast<long unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count());

	rng.seed(seed_val);
}

std::vector<AF> Stack::getStackForDisplay()
{
	std::vector<AF> result;
	/* Use indices to get the right range, but use peek
	 * to ensure the correct order regardless of actual
	 * implementation.
	 */
	for (unsigned int i=0;i<(unsigned int)stack.size();i++) {
		result.push_back(peekAt(i));
	}
	return result;
}

void Stack::debugStackPrint()
{
	for (unsigned int i=0;i<(unsigned int)stack.size();i++) {
		std::cerr << "Stack " << i << ": " << peekAt(i).toString() << std::endl;
	}
}

void Stack::setOption(CalcOpt o, bool v)
{
	if (v) {
		options.insert(o);
	}
	else {
		options.erase(o);
	}
}

bool Stack::getOption(CalcOpt o)
{
	return options.contains(o);
}

const std::map<std::string, CalcOpt> Stack::getOptionNameMap()
{
	return CalcOptNames;
}

intmax_t Stack::getBitMask()
{
	switch (bitCount) {
		case bc8:
			return 0xFFL;
		case bc16:
			return 0xFFFFL;
		case bc32:
			return 0xFFFFFFFFL;
		default:
		case bc64:
			return 0x7FFFFFFFFFFFFFFFL;
	}
}

void Stack::setBitCount(BitCount b) {
	bitCount = b;
}

BitCount Stack::getBitCount() {
	return bitCount;
}

void Stack::saveHistory()
{
	if (options.contains(SaveHistory)) {
		std::vector<AF> stackCopy(stack);
		history.push_back(stackCopy);
		if (history.size() > MAX_HIST) {
			history.pop_front();
		}
	}
}


void Stack::clear()
{
	stack.clear();
}


void Stack::push(AF v)
{
	stack.push_back(v);
	if (options.contains(ReplicateStack)) {
		while (stack.size() > 4) {
			// Remove first element
			stack.erase(stack.begin());
		}
	}
}


void Stack::push(std::vector<AF> vs)
{
	for (AF n: vs) {
		stack.push_back(n);
	}
	if (options.contains(ReplicateStack)) {
		while (stack.size() < 4) {
			// Remove first element
			stack.erase(stack.begin());
		}
	}
}


AF Stack::pop()
{
	AF result;
	if (stack.empty()) {
		result = AF(0.0);
	}
	else {
		if (options.contains(ReplicateStack)) {
			if (stack.size() == 4) {
				stack.insert(stack.begin(), stack.front());
			}
		}
		result = stack.back();
		stack.pop_back();
	}
	return result;
}

AF Stack::peek()
{
	AF result;
	if (stack.empty()) {
		result = AF(0.0);
	}
	else {
		result = stack.back();
	}
	return result;
}

AF Stack::peekAt(int index)
{
	AF result;
	if (index < (int) stack.size()) {
		result = stack.at(stack.size() - (1+index));
	}
	else {
		result = AF(0.0);
	}
	return result;
}


ErrorCode Stack::rollUp()
{
	if (stack.size() > 0) {
		AF v = stack.back();
		stack.pop_back();
		stack.insert(stack.begin(), v);
	}
	return NoError;
}

ErrorCode Stack::rollDown()
{
	if (stack.size() > 0) {
		stack.push_back(stack.front());
		// Remove first element
		stack.erase(stack.begin());
	}
	return NoError;
}


ErrorCode Stack::undo()
{
	if ( ! options.contains(SaveHistory)) {
		return NoHistorySaved;
	}
	if (history.size() > 0) {
		stack = history.back();
		history.pop_back();
	}
	else {
		stack.clear();
	}
	return NoError;
}

void Stack::printThisStack(std::vector<AF> s) {
	for (int i=0;i<(int)s.size();i++) {
		std::cerr << "-> " << i << ": " << s.at(i).toString() << std::endl;
	}
}

void Stack::printHistory()
{
	if ( ! options.contains(SaveHistory)) {
		std::cerr << "No history saved" << std::endl;
	}

	std::cerr << "Current stack:" << std::endl;
	printThisStack(stack);

	int index = 0;
	for (auto & l : history) {
		std::cerr << "History entry " << index << std::endl;

		printThisStack(l);

		index += 1;
	}
}
