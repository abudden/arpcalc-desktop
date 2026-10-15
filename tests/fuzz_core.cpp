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
 * Random key-press tester for the calculator core (no Qt needed).
 *
 * Presses random buttons and keys (weighted towards clear / C / backspace)
 * and renders every display after each press, as CalcWindow does.  Build
 * it with AddressSanitizer/UBSan (see run_fuzz.sh) so that memory errors
 * are reported where they happen rather than as a later, unrelated crash.
 *
 * Usage: fuzz_core [seed] [steps]; set TRACE=1 to print each press.
 */
#include <cstdlib>
#include <iostream>
#include <random>
#include <set>
#include "commands.h"

int main(int argc, char **argv)
{
	unsigned seed = (argc > 1) ? atoi(argv[1]) : 1;
	int steps = (argc > 2) ? atoi(argv[2]) : 2000;
	bool trace = (getenv("TRACE") != NULL);
	std::mt19937 rng(seed);

	// Every command on the main button grids
	const std::vector<std::string> grids = {
		"numpad", "funcpad", "altfuncpad", "convpad", "constpad", "sipad"
	};
	std::set<std::string> cmdset;
	{
		CommandHandler c;
		for (auto &g : grids) {
			for (auto &row : c.getGrid(g)) {
				for (auto &b : row) {
					cmdset.insert(b.name);
				}
			}
		}
	}
	const std::vector<std::string> cmds(cmdset.begin(), cmdset.end());

	const std::vector<std::string> keys = {
		"C", "c", "`", "Del", "Backspace", "Backspace", "Enter", "0", "1",
		"5", "9", ".", "E", "Up", "Down", "Left", "Right", "-", "+", "*",
		"/", "A", "B", "F", "J", "%", "^"
	};
	const std::vector<std::string> mods = {"plain", "shift", "ctrl", "alt"};
	const std::vector<std::string> storeTabs = {
		"romanupperpad", "romanlowerpad", "greekupperpad", "greeklowerpad", "sipad"
	};

	CommandHandler c;
	c.setDefaultOptions();
	const std::vector<std::string> optionNames = c.getOptionNames();

	for (int i=0;i<steps;i++) {
		int r = rng() % 100;
		std::string desc;
		ErrorCode ec;

		if (r < 20) {
			desc = "button clear";
			c.keypress("clear");
		}
		else if (r < 30) {
			desc = "key C";
			c.handleKey("C", "plain", "numpad", ec);
		}
		else if (r < 70) {
			desc = "button " + cmds[rng() % cmds.size()];
			c.keypress(desc.substr(7));
		}
		else if (r < 92) {
			std::string k = keys[rng() % keys.size()];
			std::string m = mods[rng() % mods.size()];
			desc = "key " + k + " (" + m + ")";
			c.handleKey(k, m, "numpad", ec);
		}
		else if (r < 95) {
			std::string k(1, 'A' + (rng() % 26));
			std::string t = storeTabs[rng() % storeTabs.size()];
			desc = "key " + k + " on " + t;
			c.handleKey(k, "plain", t, ec);
		}
		else if (r < 98) {
			std::string n = optionNames[rng() % optionNames.size()];
			desc = "option " + n;
			c.setOptionByName(n, rng() % 2);
		}
		else {
			desc = "next places";
			c.nextPlaces();
		}

		if (trace) {
			std::cerr << i << ": " << desc << std::endl;
		}

		c.getXDisplay();
		c.getStackDisplay();
		c.getBaseDisplay();
		c.getStatusBase();
		c.getXValue().toString();
	}
	return 0;
}
