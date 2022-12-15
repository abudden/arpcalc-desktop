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
Module.onRuntimeInitialized = _ => {
    calc = new Module.CommandHandler();

    console.log(localStorage);
    calc.restoreSettings();

    jscalc = calc;
    logger = console;

    tab_width = 96;
    btn_height = tab_width * 64/96;
    stack_height = 96;
    status_height = 15;
    entry_height = btn_height;
    size_name = "Large";
    key_logging = false;
    initial_window_width = 800;
    initial_window_height = 800;

    disclaimer_height = 96;

	var screenwidth = (window.innerWidth > 0) ? window.innerWidth : screen.width;
	var screenheight = (window.innerHeight > 0) ? window.innerHeight : screen.height;

	if (screenwidth < 1000) {
	    document.body.style.zoom = "260%";
	}
    init();
};
