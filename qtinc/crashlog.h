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
#ifndef CRASHLOG_H
#define CRASHLOG_H

// On Windows, append a report to crash.log (next to the executable, or
// ARPCalc-crash.log in %TEMP% if that can't be written) if the app dies from
// an uncaught C++ exception or a fault such as an access violation.  The
// report gives the faulting module and a backtrace as module+offset, which
// can be turned into source lines with addr2line (see README.md).
//
// Does nothing on other platforms.
void installCrashLogger(const char *versionInfo);

#endif // CRASHLOG_H
