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
#include "crashlog.h"

#ifdef _WIN32

#include <windows.h>
#include <cxxabi.h>
#include <cstdio>
#include <cstdlib>
#include <cwchar>
#include <ctime>
#include <exception>
#include <typeinfo>

// Everything the handlers need is worked out up front, as little as
// possible should be done once the process is in a bad state.
static wchar_t exeLogPath[MAX_PATH];
static wchar_t tempLogPath[MAX_PATH];
static const char *version = "";
static LPTOP_LEVEL_EXCEPTION_FILTER previousFilter = NULL;

static FILE *openLog()
{
	FILE *f = _wfopen(exeLogPath, L"a");
	if (f == NULL) {
		f = _wfopen(tempLogPath, L"a");
	}
	return f;
}

static void writeHeader(FILE *f, const char *what)
{
	time_t now = time(NULL);
	char stamp[64] = "";
	strftime(stamp, sizeof(stamp), "%Y-%m-%d %H:%M:%S", localtime(&now));
	fprintf(f, "==== %s: %s\n", stamp, what);
	fprintf(f, "Version: %s\n", version);
}

static void writeAddress(FILE *f, DWORD64 address)
{
	HMODULE module = NULL;
	wchar_t path[MAX_PATH] = L"";
	if (GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS
				| GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
				(LPCWSTR) address, &module)
			&& GetModuleFileNameW(module, path, MAX_PATH)) {
		const wchar_t *name = wcsrchr(path, L'\\');
		name = (name != NULL) ? (name + 1) : path;
		fprintf(f, "  %ls+0x%llx\n", name,
				(unsigned long long) (address - (DWORD64) module));
	}
	else {
		fprintf(f, "  0x%llx\n", (unsigned long long) address);
	}
}

static void writeBacktrace(FILE *f, CONTEXT context)
{
	fprintf(f, "Backtrace:\n");
#ifdef _WIN64
	// Walk the stack with the unwind tables (gcc emits them on x64 too)
	for (int i=0;i<64;i++) {
		DWORD64 pc = context.Rip;
		if (pc == 0) {
			break;
		}
		writeAddress(f, pc);

		DWORD64 imageBase = 0;
		PRUNTIME_FUNCTION function = RtlLookupFunctionEntry(pc, &imageBase, NULL);
		if (function != NULL) {
			PVOID handlerData = NULL;
			DWORD64 establisherFrame = 0;
			RtlVirtualUnwind(UNW_FLAG_NHANDLER, imageBase, pc, function,
					&context, &handlerData, &establisherFrame, NULL);
		}
		else {
			// Leaf function: the return address is on top of the stack
			context.Rip = *(DWORD64 *) context.Rsp;
			context.Rsp += 8;
		}
	}
#else
	writeAddress(f, context.Eip);
#endif
}

static void terminateHandler()
{
	FILE *f = openLog();
	if (f != NULL) {
		writeHeader(f, "std::terminate called");
		std::exception_ptr e = std::current_exception();
		if (e) {
			try {
				std::rethrow_exception(e);
			}
			catch (const std::exception &ex) {
				int status = 0;
				char *type = abi::__cxa_demangle(typeid(ex).name(), NULL, NULL, &status);
				fprintf(f, "Uncaught exception %s: %s\n",
						(type != NULL) ? type : typeid(ex).name(), ex.what());
				free(type);
			}
			catch (...) {
				fprintf(f, "Uncaught exception of unknown type\n");
			}
		}
		CONTEXT context;
		RtlCaptureContext(&context);
		writeBacktrace(f, context);
		fclose(f);
	}
	abort();
}

static LONG WINAPI unhandledExceptionFilter(EXCEPTION_POINTERS *info)
{
	static volatile LONG alreadyLogged = 0;
	DWORD code = info->ExceptionRecord->ExceptionCode;

	// gcc raises C++ exceptions as SEH exceptions with codes 0x2?474343
	// ("GCC").  The MinGW runtime's own filter (previousFilter) turns
	// uncaught ones into std::terminate(), where terminateHandler() logs
	// them, so only log other exceptions here.
	if (((code & 0xF0FFFFFF) != 0x20474343)
			&& (InterlockedExchange(&alreadyLogged, 1) == 0)) {
		FILE *f = openLog();
		if (f != NULL) {
			writeHeader(f, "unhandled exception");
			fprintf(f, "Exception code 0x%08lx", (unsigned long) code);
			if ((code == EXCEPTION_ACCESS_VIOLATION)
					&& (info->ExceptionRecord->NumberParameters >= 2)) {
				ULONG_PTR operation = info->ExceptionRecord->ExceptionInformation[0];
				fprintf(f, " (%s address 0x%llx)",
						(operation == 0) ? "reading" : ((operation == 1) ? "writing" : "executing"),
						(unsigned long long) info->ExceptionRecord->ExceptionInformation[1]);
			}
			fprintf(f, "\n");
			writeBacktrace(f, *info->ContextRecord);
			fclose(f);
		}
	}

	if (previousFilter != NULL) {
		return previousFilter(info);
	}
	return EXCEPTION_CONTINUE_SEARCH;
}

static void setLogPath(wchar_t *dest, const wchar_t *dir, const wchar_t *name)
{
	if ((wcslen(dir) + wcslen(name) + 1) < MAX_PATH) {
		wcscpy(dest, dir);
		wcscat(dest, name);
	}
}

void installCrashLogger(const char *versionInfo)
{
	version = versionInfo;

	wchar_t dir[MAX_PATH] = L"";
	DWORD length = GetModuleFileNameW(NULL, dir, MAX_PATH);
	if ((length > 0) && (length < MAX_PATH)) {
		wchar_t *slash = wcsrchr(dir, L'\\');
		if (slash != NULL) {
			slash[1] = L'\0';
			setLogPath(exeLogPath, dir, L"crash.log");
		}
	}
	length = GetTempPathW(MAX_PATH, dir);
	if ((length > 0) && (length < MAX_PATH)) {
		setLogPath(tempLogPath, dir, L"ARPCalc-crash.log");
	}

	std::set_terminate(terminateHandler);
	previousFilter = SetUnhandledExceptionFilter(unhandledExceptionFilter);
}

#else

void installCrashLogger(const char *versionInfo)
{
	(void) versionInfo;
}

#endif
