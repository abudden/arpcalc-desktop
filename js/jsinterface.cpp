#include <emscripten/bind.h>
#include <emscripten/val.h>

#include <iostream>
#include <string>

#include "commands.h"
#include "json.hpp"
#include "strutils.h"

using namespace emscripten;

using json = nlohmann::json;

void to_json(json& j, const BI& bi) {
	j = json{{"name", bi.name},
		{"display", bi.display},
		{"helpText", bi.helpText},
		{"scale", bi.scale},
		{"doubleWidth", bi.doubleWidth},
		{"hidden", bi.hidden}
	};
}

void to_json(json& j, const Option& opt) {
	j = json{{"helpText", opt.helpText},
		{"location", {{"page", opt.location.page},
						 {"row", opt.location.row},
						 {"col", opt.location.col}}},
		{"hideOnWeb", opt.hideOnWeb},
		{"requiresRestart", opt.requiresRestart}
	};
}

class JSI {
	public:
		JSI() {
			calc.setDefaultOptions();
		}

		// Direct map string functions
		std::string getXDisplay() { return calc.getXDisplay(); }
		std::string getStackDisplay() { return calc.getStackDisplay(); }
		std::string getBaseDisplay() { return calc.getBaseDisplay(); }
		std::string getStatusExponent() { return calc.getStatusExponent(); }
		std::string getStatusAngularUnits() { return calc.getStatusAngularUnits(); }
		std::string getStatusBase() { return calc.getStatusBase(); }

		// Direct map other functions
		bool isHexadecimal() { return calc.isHexadecimal(); }
		void store(std::string name) { calc.store(name); }
		void recall(std::string name) { calc.recall(name); }

		// jscalc custom functions
		void setBitCountByValue(int i) {
			switch (i) {
				case 8: calc.setBitCount(bc8); break;
				case 16: calc.setBitCount(bc16); break;
				default:
				case 32: calc.setBitCount(bc32); break;
				case 64: calc.setBitCount(bc64); break;
			}
		}

		int getBitCountAsValue() {
			switch (calc.getBitCount()) {
				case bc8: return 8;
				case bc16: return 16;
				default:
				case bc32: return 32;
				case bc64: return 64;
			}
		}

		void toggleOptionByName(std::string n) {
			bool existing = calc.getOptionByName(n);
			calc.setOptionByName(n, ! existing);

			saveSettings();
		}

		bool getOptionByName(std::string n) {
			return calc.getOptionByName(n);
		}

		void setOptionByName(std::string n, bool v = true) {
			calc.setOptionByName(n, v);
		}

		std::string keypress_js(std::string key) {
			ErrorCode ec = calc.keypress(key);
			saveStack();
			return getErrorCodeAsName(ec);
		}

		void restoreSettings() {
			restoreDefaults();

			val localStorage = val::global("localStorage");

			for (std::string name: calc.getOptionNames()) {
				val r = localStorage.call<emscripten::val>("getItem", name);
				if ( ! r.isNull()) {
					std::string value = r.as<std::string>();
					if (value == "true") {
						calc.setOptionByName(name, true);
					}
					else if (value == "false") {
						calc.setOptionByName(name, false);
					}
					else {
						// Leave as default
					}
				}
			}

			std::string base = "DEC";
			if (calc.getOption(SaveBaseOnExit)) {
				val b = localStorage.call<emscripten::val>("getItem", std::string("SavedBase"));
				if ( ! b.isNull()) {
					std::cout << "Non-null base " << " value " << b.as<std::string>() << std::endl;
					base = b.as<std::string>();
				}
			}
			calc.setStatusBase(base);

			// TODO: currency
			int storeKeyCount = localStorage["length"].as<int>();
			std::map<std::string, AF> vMap;
			for (int i=0;i<storeKeyCount;i++) {
				std::string key = localStorage.call<emscripten::val>("key", i).as<std::string>();
				if (startsWith(key, "VARSTORE-")) {
					std::string value = localStorage.call<emscripten::val>("getItem", key).as<std::string>();
					vMap[key.substr(9)] = AF(value);
				}
			}
			calc.loadVarStore(vMap);

			val bc = localStorage.call<emscripten::val>("getItem", std::string("BitCount"));
			if ( ! bc.isNull() ) {
				std::cout << "Non-null bitcount " << " value " << bc.as<int>() << std::endl;
				setBitCountByValue(bc.as<int>());
			}

			restoreStack();
		}

		void restoreStack() {
			val localStorage = val::global("localStorage");

			int stackSize = localStorage.call<emscripten::val>("getItem", std::string("SavedStackLength")).as<int>();
			for (int i=0;i<stackSize;i++) {
				std::string valueStr = localStorage.call<emscripten::val>("getItem",
						"SavedStack-" + std::to_string(i)).as<std::string>();
				AF v(valueStr);
				// Bypass history stuff by pushing directly
				calc.st.push(v);
			}
		}

		void restoreDefaults() {
			calc.setDefaultOptions();
		}

		void saveSettings() {
			val localStorage = val::global("localStorage");

			for (std::string name: calc.getOptionNames()) {
				localStorage.call<void>("setItem", name, calc.getOptionByName(name));
			}

			std::string base = "DEC";
			if (calc.getOption(SaveBaseOnExit)) {
				base = calc.getStatusBase();
			}
			localStorage.call<void>("setItem", std::string("SavedBase"), base);

			// TODO: currency
			if (calc.varStoreHasChanged()) {
				for (auto & [key, value] : calc.varStore) {
					localStorage.call<void>("setItem", "VARSTORE-" + key,
							value.toString());
				}
			}

			localStorage.call<void>("setItem", std::string("BitCount"), getBitCountAsValue());

			saveStack();
		}

		void saveStack() {
			std::vector<AF> storeStack;
			if (calc.getOption(SaveStackOnExit)) {
				// Abusing encapsulation here a lot - not the best way to do it!
				storeStack = calc.st.stack;
			}

			val localStorage = val::global("localStorage");
			localStorage.call<void>("setItem", std::string("SavedStackLength"), (int) storeStack.size());
			for (int i=0;i<storeStack.size();i++) {
				localStorage.call<void>("setItem", "SavedStack-" + std::to_string(i),
						storeStack[i].toString());
			}
		}

		std::string callOptionIconAction(std::string option) {
			if (options.empty()) {
				options = calc.getOptions();
			}
			if ( ! options.contains(option)) {
				return "none";
			}

			return "/" + options[option].iconAction(&calc) + ".png";
		}

		void callOptionClickAction(std::string option) {
			if (options.empty()) {
				options = calc.getOptions();
			}
			if ( ! options.contains(option)) {
				return;
			}
			options[option].clickAction(&calc);
			saveSettings();
		}

		std::string getImageForDrawable(std::string name) {
			// Kotlin version got an integer, which needed to
			// be looked up.  C++ version gets a string directly.
			// Kept this function to minimise JS changes.
			return name;
		}

		std::string getJSCompatibleGrid(std::string grid) {
			std::vector< std::vector<BI> > fullgrid = calc.getGrid(grid);
			json j = fullgrid;
			return j.dump();
		}

		std::string getOptionsAsJson() {
			if (options.empty()) {
				options = calc.getOptions();
			}
			json j = options;
			return j.dump();
		}

		std::string getConversionCategories() {
			std::vector<std::string> categories = calc.st.getConversionCategories();
			json j = categories;
			return j.dump();
		}

		std::string getAvailableUnits(std::string category) {
			std::set<std::string> units = calc.st.getAvailableUnits(category);
			json j = units;
			return j.dump();
		}

		std::string getConstantCategories() {
			std::set<std::string> categories;
			for (const auto &c : calc.st.constants) {
				categories.insert(c.category);
			}
			json j = categories;
			return j.dump();
		}

		std::string getConstantsInCategory(std::string category) {
			std::vector<std::string> constants;
			for (const auto &c : calc.st.constants) {
				if (category == c.category) {
					constants.push_back(c.name);
				}
			}
			json j = constants;
			return j.dump();
		}

		std::string getDensityCategories() {
			std::set<std::string> categories;
			for (const auto &c : calc.st.densities) {
				categories.insert(c.category);
			}
			json j = categories;
			return j.dump();
		}

		std::string getDensitiesInCategory(std::string category) {
			std::vector<std::string> densities;
			for (const auto &c : calc.st.densities) {
				if (category == c.category) {
					std::string displayName = c.name;
					displayName += " (" + AF(c.value).toString() +
						"&nbsp;kg/m&#xB3)";
					densities.push_back(displayName);
				}
			}
			json j = densities;
			return j.dump();
		}

		std::string processCurrencyData(std::string jsonData, std::string datestr) {
			json j = json::parse(jsonData);
			// even easier with structured bindings (C++17)
			std::map<std::string, double> wrtEuro;
			for (auto& [key, value] : j.items()) {
				wrtEuro[key] = value;
			}
			std::string datecheck = calc.processCurrencyData(wrtEuro, datestr);
			return datecheck;
		}

		std::string handleKey(std::string key, std::string modifiers, std::string tab) {
			std::map<std::string, std::string> result;
			ErrorCode ec = NoError;
			keyHandlerResult r = calc.handleKey(key, modifiers, tab, ec);
			switch (r) {
				default:
				case Key_NotHandled: result["HandlerResult"] = "NotHandled"; break;
				case Key_Handled: result["HandlerResult"] = "Handled"; break;
				case Key_Cancel: result["HandlerResult"] = "Cancel"; break;
				case Key_NextTab: result["HandlerResult"] = "NextTab"; break;
				case Key_MoreConversions: result["HandlerResult"] = "MoreConversions"; break;
				case Key_ConstByName: result["HandlerResult"] = "ConstByName"; break;
				case Key_SI: result["HandlerResult"] = "SI"; break;
				case Key_Store: result["HandlerResult"] = "Store"; break;
				case Key_Recall: result["HandlerResult"] = "Recall"; break;
				case Key_CopyToClipboard: result["HandlerResult"] = "CopyToClipboard"; break;
				case Key_PasteFromClipboard: result["HandlerResult"] = "PasteFromClipboard"; break;
				case Key_Quit: result["HandlerResult"] = "Quit"; break;
			}
			saveStack();
			result["ErrorCode"] = getErrorCodeAsName(ec);
			json j = result;
			return j.dump();
		}

		std::string getShortcutKeys(std::string name) {
			std::vector<std::string> result = calc.getShortcutKeys(name);
			json j = result;
			return j.dump();
		}

	private:
		std::map<std::string, Option> options;

		std::string getErrorCodeAsName(ErrorCode e) {
			switch (e) {
				case NoError: return "NoError";
				case DivideByZero: return "DivideByZero";
				case InvalidRoot: return "InvalidRoot";
				case InvalidLog: return "InvalidLog";
				case InvalidTan: return "InvalidTan";
				case InvalidInverseTrig: return "InvalidInverseTrig";
				case InvalidInverseHypTrig: return "InvalidInverseHypTrig";
				case UnknownConstant: return "UnknownConstant";
				case UnknownConversion: return "UnknownConversion";
				case InvalidConversion: return "InvalidConversion";
				case UnknownSI: return "UnknownSI";
				case NoFunction: return "NoFunction";
				case NoHistorySaved: return "NoHistorySaved";
				default:
				case NotImplemented: return "NotImplemented";
			}
		}

		CommandHandler calc;
};

EMSCRIPTEN_BINDINGS(arpcalc_jsi) {
	class_<JSI>("CommandHandler")
		.constructor()

		.function("getXDisplay", &JSI::getXDisplay)
		.function("getStackDisplay", &JSI::getStackDisplay)
		.function("getBaseDisplay", &JSI::getBaseDisplay)
		.function("getStatusExponent", &JSI::getStatusExponent)
		.function("getStatusAngularUnits", &JSI::getStatusAngularUnits)
		.function("getStatusBase", &JSI::getStatusBase)

		.function("isHexadecimal", &JSI::isHexadecimal)
		.function("store", &JSI::store)
		.function("recall", &JSI::recall)

		.function("setBitCountByValue", &JSI::setBitCountByValue)
		.function("getBitCountAsValue", &JSI::getBitCountAsValue)
		.function("toggleOptionByName", &JSI::toggleOptionByName)
		.function("getOptionByName", &JSI::getOptionByName)
		.function("setOptionByName", &JSI::setOptionByName)
		.function("keypress_js", &JSI::keypress_js)
		.function("restoreSettings", &JSI::restoreSettings)
		.function("restoreDefaults", &JSI::restoreDefaults)
		.function("saveSettings", &JSI::saveSettings)
		.function("callOptionIconAction", &JSI::callOptionIconAction)
		.function("callOptionClickAction", &JSI::callOptionClickAction)

		.function("getImageForDrawable", &JSI::getImageForDrawable)

		.function("getJSCompatibleGrid", &JSI::getJSCompatibleGrid)
		.function("getOptionsAsJson", &JSI::getOptionsAsJson)

		.function("getConversionCategories", &JSI::getConversionCategories)
		.function("getAvailableUnits", &JSI::getAvailableUnits)

		.function("getConstantCategories", &JSI::getConstantCategories)
		.function("getConstantsInCategory", &JSI::getConstantsInCategory)
		.function("getDensityCategories", &JSI::getDensityCategories)
		.function("getDensitiesInCategory", &JSI::getDensitiesInCategory)

		.function("handleKey", &JSI::handleKey)
		.function("getShortcutKeys", &JSI::getShortcutKeys)

		.function("processCurrencyData", &JSI::processCurrencyData)
		;
}
