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
#ifndef COMMANDS_H
#define COMMANDS_H

#include <list>
#include <map>
#include <set>
#include <string>
#include <stddef.h>

#include "arpfloat.h"
#include "stack.h"

typedef enum _DisplayBase {
	baseDecimal,
	baseHexadecimal,
	baseOctal,
	baseBinary
} DisplayBase;

typedef struct _DisplayState {
	bool forcedEngDisplay;
	long forcedEngFactor;
	std::string enteredText;
	AF enteredValue;
	bool entering;
	bool justPressedEnter;
	bool justPressedBase;
	bool showAll;
} DisplayState;

typedef struct _ValueAndExp {
	AF value;
	int exponent;
} ValueAndExp;

typedef enum _DispOpt {
	EngNotation,
	BinaryPrefixes,
	PowerExponentView,
	SINotation,
	TrimZeroes,
	AlwaysShowDecimal,
	ThousandsSeparator,
	SpaceAsThousandsSeparator,
	SaveStackOnExit,
	SaveBaseOnExit,
	DecimalBaseBias,
	CheckForNewVersions,
	AmericanUnits,
	EuropeanDecimal,
	ShowHelpOnStart
	// If adding any here, make sure they're added to getDispOptionByName
	// and the various settings saving places.
} DispOpt;

#define NAME(x) #x, x
const std::map<std::string, DispOpt> DispOptNames = {
	{NAME(EngNotation)},
	{NAME(BinaryPrefixes)},
	{NAME(PowerExponentView)},
	{NAME(SINotation)},
	{NAME(TrimZeroes)},
	{NAME(AlwaysShowDecimal)},
	{NAME(ThousandsSeparator)},
	{NAME(SpaceAsThousandsSeparator)},
	{NAME(SaveStackOnExit)},
	{NAME(SaveBaseOnExit)},
	{NAME(DecimalBaseBias)},
	{NAME(CheckForNewVersions)},
	{NAME(AmericanUnits)},
	{NAME(EuropeanDecimal)},
	{NAME(ShowHelpOnStart)}
};

typedef struct _DisplayOptions {
	std::set<DispOpt> bOptions;
	int decimalPlaces;
	int expNegMinDisplay;
	int expPosMaxDisplay;
} DisplayOptions;

typedef struct _BI {
	std::string name;
	std::string display;
	std::string helpText = "";
	double scale = 1.0;
	bool doubleWidth = false;
	bool hidden = false;
} BI;

typedef struct _OptLoc {
	int page = 0;
	int row = -1;
	int col = -1;
} OptLoc;

// Forward declaration for struct
class CommandHandler;

typedef struct _Option {
	std::string helpText;
	void (*clickAction)(CommandHandler *);
	std::string (*iconAction)(CommandHandler *);
	OptLoc location;
	bool hideOnPhone = false;
	bool hideOnPC = false;
	bool hideOnWeb = false;
	bool requiresRestart = false;
} Option;

typedef void (*keyHandler)(CommandHandler *);
typedef enum _keyHandlerResult {
	Key_NotHandled,
	Key_Handled,
	Key_Cancel,
	Key_NextTab,
	Key_MoreConversions,
	Key_ConstByName,
	Key_DensityByName,
	Key_SI,
	Key_Store,
	Key_Recall,
	Key_CopyToClipboard,
	Key_PasteFromClipboard,
	Key_Quit,
} keyHandlerResult;
typedef struct _chkeymap {
	std::string plainCmd = "NOP";
	keyHandler plainHandler = NULL;
	bool plainHidden = false;

	std::string shiftCmd = "NOP";
	keyHandler shiftHandler = NULL;
	bool shiftHidden = false;

	std::string ctrlCmd = "NOP";
	keyHandler ctrlHandler = NULL;
	bool ctrlHidden = false;

	std::string altCmd = "NOP";
	keyHandler altHandler = NULL;
	bool altHidden = false;

	std::string ctrlShiftCmd = "NOP";
	keyHandler ctrlShiftHandler = NULL;
	bool ctrlShiftHidden = false;

	std::string altShiftCmd = "NOP";
	keyHandler altShiftHandler = NULL;
	bool altShiftHidden = false;

	std::string ctrlAltCmd = "NOP";
	keyHandler ctrlAltHandler = NULL;
	bool ctrlAltHidden = false;
} KeyMap;

class CommandHandler
{
	public:
		CommandHandler();

		bool varStoreHasChanged();
		void loadVarStore(std::map<std::string, AF> stored);
		void setOptionByName(std::string o, bool v = true);
		bool getOptionByName(std::string o);
		std::vector<std::string> getOptionNames();
		void setOption(CalcOpt o, bool v = true);
		bool getOption(CalcOpt o);
		void toggleOption(CalcOpt o);
		void setPlaces(int v);
		int getPlaces();
		void setOption(DispOpt o, bool v = true);
		bool getOption(DispOpt o);
		void toggleOption(DispOpt o);
		void startEntering();
		void completeEntering(bool needValue);
		void updateValueFromText();
		void clearButton();
		void numInput(std::string value);
		void hexInput(std::string value);
		void dotInput();
		void enter();
		void exponent();
		void backspace();
		void plusMinus();
		AF getXValue();
		std::string getXDisplay();
		std::string getStackDisplay();
		std::string getStatusAngularUnits();
		std::string getStatusBase();
		void setStatusBase(std::string b);
		std::string getStatusExponent();
		std::string getBaseDisplay();
		void nextBase();
		void selectBase(DisplayBase base);
		void nextWindowSize();
		std::string getSizeName();
		void setSizeName(std::string name);
		void nextPlaces();
		void setBitCount(BitCount v);
		BitCount getBitCount();
		void nextBitCount();
		std::map<std::string, std::string> getDisplayContents();
		void optionsChanged();
		std::string getParseableX();
		bool isDecimal();
		bool isHexadecimal();
		void store(std::string name);
		void recall(std::string name);
		ErrorCode keypresses(std::string charKeys);
		ErrorCode keypresses(std::list<std::string> commands);
		ErrorCode keypress(std::string key);
		std::string addPeriodic(std::string source, int spacing, std::string insertion);
		std::string addThinSpaces(std::string source, int spacing = 4);
		std::string addThousandsSeparator(std::string source, int spacing = 3);
		std::string europeanDecimal(std::string source);
		std::string formatBase(AF value, DisplayBase base, bool isX = false);
		ValueAndExp getValueAndExponent(AF value, bool isX);
		std::string formatEnteredText(std::string value);
		std::string formatDecimal(AF value, bool isX, bool constHelpMode = false);
		std::string powerExponent(std::string preformatted, bool isX);
		void engRotate(int direction);
		AF RoundToDecimalPlaces(AF d, int c);
		std::string processCurrencyData(std::map<std::string, double> wrtEuro, std::string date);
		std::map<std::string, double> getRawCurrencyData();

		void setDefaultOptions();

		// SI
		void initialiseSIPrefixes();
		ErrorCode SI(std::string name);
		std::vector<std::string> getBinaryPrefixSymbols();
		std::vector<std::string> getDecimalPrefixSymbols();
		std::vector<std::string> getBinaryPrefixNames();
		std::vector<std::string> getDecimalPrefixNames();

		// Grids
		std::string getCheckIcon(bool v);
		std::string getPlacesIcon();
		std::string getBitCountIcon();
		std::string getBaseIcon();
		std::string getWindowSizeIcon();
		std::map<std::string, Option> getOptions();
		std::string getStoreHelpText(std::string register_);
		std::vector< std::vector<BI> > getGrid(std::string grid);

		// Keys
		keyHandlerResult handleKey(std::string key, std::string modifiers, std::string tab, ErrorCode &ec);
		std::vector<std::string> getShortcutKeys(std::string name);

		void debugStackPrint();
		Stack st;
		std::map<std::string, AF> varStore;
		std::string last_currency_date = "";

	private:
		void populateKeyMaps();
		keyHandlerResult runHandler(std::string modifiers, KeyMap map, ErrorCode &ec);
		std::map<std::string, KeyMap> keyMap;
		std::map<std::string, KeyMap> siKeyMap;
		void hex_key(std::string key);
		void shift_hex_key(std::string key);

		DisplayOptions dspOptions;
		DisplayState dspState;
		DisplayBase dspBase = baseDecimal;
		bool varStoreChanged = false;
		std::map<std::string, int> SIDecimalPrefixes;
		std::map<std::string, int> SIBinaryPrefixes;
		std::map<std::string, std::string> SISymbols;

		std::string sizeName = "Large";

		const std::map<std::string, std::string> roman_to_greek = {
			{"A", "Alpha"}, {"B", "Beta"}, {"G", "Gamma"}, {"D", "Delta"}, {"E", "Epsilon"}, {"Z", "Zeta"},
			{"H", "Eta"}, {"Q", "Theta"}, {"I", "Iota"}, {"K", "Kappa"}, {"L", "Lambda"}, {"M", "Mu"},
			{"N", "Nu"}, {"C", "Xi"}, {"O", "Omicron"}, {"P", "Pi"}, {"R", "Rho"}, {"S", "Sigma"},
			{"T", "Tau"}, {"U", "Upsilon"}, {"V", "Phi"}, {"X", "Chi"}, {"Y", "Psi"}, {"W", "Omega"}
		};

};

#endif
