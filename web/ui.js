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
var returnToNumPad = true;
var hypMode = false;
var lastTab = "numpad";
var storeMode = "None";
var lastStoreTab = "romanupperpad";
var buttonMap = {};
var btn_folder = "none";

var sc_standard_style;
var sc_option_style;
var sc_multi_style;
var sc_entry_style;
var sc_button_style;

var endlabel = 'By using this software you agree to the terms described in the software information page.  Click here for more information.';

function setupUi(sizeName, winWidth, winHeight) {
	btn_folder = "96";
	var full_width = (tab_width*5)-4;
	var spacer_width = 0;

	if (sizeName == "Scalable") {
		sc_standard_style = document.createElement('style');
		sc_standard_style.type = 'text/css';
		sc_standard_style.innerHTML = '.txtStandardScalable { font-size: 100%; }';
		sc_option_style = document.createElement('style');
		sc_option_style.type = 'text/css';
		sc_option_style.innerHTML = '.txtOptionScalable { font-size: 90%; }';
		sc_multi_style = document.createElement('style');
		sc_multi_style.type = 'text/css';
		sc_multi_style.innerHTML = '.txtMultiScalable { font-size: 80%; }';
		sc_entry_style = document.createElement('style');
		sc_entry_style.type = 'text/css';
		sc_entry_style.innerHTML = '.txtEntryScalable { font-size: 200%; }';
		sc_button_style = document.createElement('style');
		sc_button_style.type = 'text/css';
		sc_button_style.innerHTML = '.btnScalable button { font-size: 100%; }';
		var h = document.getElementsByTagName('head')[0];
		h.appendChild(sc_standard_style);
		h.appendChild(sc_option_style);
		h.appendChild(sc_multi_style);
		h.appendChild(sc_entry_style);
		h.appendChild(sc_button_style);

		var s = getSizes(winWidth, winHeight);
		tab_width = s['tab_width'];
		full_width = s['full_width'];
		btn_height = s['btn_height'];

		entry_height = s['entry_height'];
		stack_height = s['stack_height'];
		status_height = s['status_height'];
		spacer_width = s['spacer_width'];
		disclaimer_height = s['disclaimer_height'];
		updateFontSizes(s);
	}
	var optLblTemplate =
		'<div style="display: table; height: 100%;">' +
			'<div style="display: table-cell; vertical-align: middle; line-height: 1.2;">' +
				'#label#' +
			'</div>' +
		'</div>';
	ui = webix.ui({
		width: winWidth,
		id: "lytWindow",
		cols: [
			{width: spacer_width, id: 'lytSpacer0'},
			{width: full_width,
			id: "lytTop",
			rows: [
				{cols: [
					{view: "label", label: "EXP:10", id: "txtStatusExponent"},
					{view: "label", label: ""},
					{view: "label", label: "DEG", id: "txtStatusAngular"},
					{view: "label", label: ""},
					{view: "label", label: "DEC", id: "txtStatusBase"}
				], height: status_height, id: "lytStatus"},
				{cols: [
					{
						template: "",
						id: "txtStack", css: "txtMulti"
					},
					{
						template: "",
						id: "txtBase", css: "txtMulti"
					}
				], height: stack_height, id: "lytStack"},
				{
					view: "label", label: "", id: "txtEntry",
					height: entry_height,
					css: "txtEntry"
				},
				{cols: [
					{view: "button", type: "image", height: tab_width, width: tab_width, click: tabButtonClicked, id: "imNumPad", image: resource_root+"/images/"+btn_folder+"/numpad.png"},
					{view: "button", type: "image", height: tab_width, width: tab_width, click: tabButtonClicked, id: "imFuncPad", image: resource_root+"/images/"+btn_folder+"/funcpad.png"},
					{view: "button", type: "image", height: tab_width, width: tab_width, click: tabButtonClicked, id: "imConvPad", image: resource_root+"/images/"+btn_folder+"/convpad.png"},
					{view: "button", type: "image", height: tab_width, width: tab_width, click: tabButtonClicked, id: "imConstPad", image: resource_root+"/images/"+btn_folder+"/constpad.png"},
					{view: "button", type: "image", height: tab_width, width: tab_width, click: tabButtonClicked, id: "imOptPad", image: resource_root+"/images/"+btn_folder+"/optpad.png"},
				], id: "lytTabs"},

				{cols: [
					{rows: [
						{cols: [
							{view: "button", type: "htmlbutton", id: "b00", label: "", click: buttonClicked},
							{view: "button", type: "htmlbutton", id: "b01", label: "", click: buttonClicked},
							{view: "button", type: "htmlbutton", id: "b02", label: "", click: buttonClicked},
							{view: "button", type: "htmlbutton", id: "b03", label: "", click: buttonClicked},
							{view: "button", type: "htmlbutton", id: "b04", label: "", click: buttonClicked},
							{view: "button", type: "htmlbutton", id: "b05", label: "", click: buttonClicked}
						], height: btn_height, id: "lytBtns0"},
						{cols: [
							{view: "button", type: "htmlbutton", id: "b10", label: "", click: buttonClicked},
							{view: "button", type: "htmlbutton", id: "b11", label: "", click: buttonClicked},
							{view: "button", type: "htmlbutton", id: "b12", label: "", click: buttonClicked},
							{view: "button", type: "htmlbutton", id: "b13", label: "", click: buttonClicked},
							{view: "button", type: "htmlbutton", id: "b14", label: "", click: buttonClicked},
							{view: "button", type: "htmlbutton", id: "b15", label: "", click: buttonClicked}
						], height: btn_height, id: "lytBtns1"},
						{cols: [
							{view: "button", type: "htmlbutton", id: "b20", label: "", click: buttonClicked},
							{view: "button", type: "htmlbutton", id: "b21", label: "", click: buttonClicked},
							{view: "button", type: "htmlbutton", id: "b22", label: "", click: buttonClicked},
							{view: "button", type: "htmlbutton", id: "b23", label: "", click: buttonClicked},
							{view: "button", type: "htmlbutton", id: "b24", label: "", click: buttonClicked},
							{view: "button", type: "htmlbutton", id: "b25", label: "", click: buttonClicked}
						], height: btn_height, id: "lytBtns2"},
						{cols: [
							{view: "button", type: "htmlbutton", id: "b30", label: "", click: buttonClicked},
							{view: "button", type: "htmlbutton", id: "b31", label: "", click: buttonClicked},
							{view: "button", type: "htmlbutton", id: "b32", label: "", click: buttonClicked},
							{view: "button", type: "htmlbutton", id: "b33", label: "", click: buttonClicked},
							{view: "button", type: "htmlbutton", id: "b34", label: "", click: buttonClicked},
							{view: "button", type: "htmlbutton", id: "b35", label: "", click: buttonClicked}
						], height: btn_height, id: "lytBtns3"},
						{cols: [
							{view: "button", type: "htmlbutton", id: "b40", label: "", click: buttonClicked},
							{view: "button", type: "htmlbutton", id: "b41", label: "", click: buttonClicked},
							{view: "button", type: "htmlbutton", id: "b42", label: "", click: buttonClicked},
							{view: "button", type: "htmlbutton", id: "b43", label: "", click: buttonClicked},
							{view: "button", type: "htmlbutton", id: "b44", label: "", click: buttonClicked},
							{view: "button", type: "htmlbutton", id: "b45", label: "", click: buttonClicked}
						], height: btn_height, id: "lytBtns4"},
						{cols: [
							{view: "button", type: "htmlbutton", id: "b50", label: "", click: buttonClicked},
							{view: "button", type: "htmlbutton", id: "b51", label: "", click: buttonClicked},
							{view: "button", type: "htmlbutton", id: "b52", label: "", click: buttonClicked},
							{view: "button", type: "htmlbutton", id: "b53", label: "", click: buttonClicked},
							{view: "button", type: "htmlbutton", id: "b54", label: "", click: buttonClicked},
							{view: "button", type: "htmlbutton", id: "b55", label: "", click: buttonClicked}
						], height: btn_height, id: "lytBtns5"}
					], id: "btnPad"},
					// Opt Pads
					{rows: [
						{cols: [
							{id: "optBtn00", view: "button", type: "image", click: optButtonClicked, image: resource_root+"/images/"+btn_height+"/blank.png", width: btn_height},
							{id: "optLbl00", view: "label", label: "", css: "optLbl", template: optLblTemplate},
							{id: "optBtn01", view: "button", type: "image", click: optButtonClicked, image: resource_root+"/images/"+btn_height+"/blank.png", width: btn_height},
							{id: "optLbl01", view: "label", label: "", css: "optLbl", template: optLblTemplate}
						], height: btn_height, id: "lytBtns6"},
						{cols: [
							{id: "optBtn10", view: "button", type: "image", click: optButtonClicked, image: resource_root+"/images/"+btn_height+"/blank.png", width: btn_height},
							{id: "optLbl10", view: "label", label: "", css: "optLbl", template: optLblTemplate},
							{id: "optBtn11", view: "button", type: "image", click: optButtonClicked, image: resource_root+"/images/"+btn_height+"/blank.png", width: btn_height},
							{id: "optLbl11", view: "label", label: "", css: "optLbl", template: optLblTemplate}
						], height: btn_height, id: "lytBtns7"},
						{cols: [
							{id: "optBtn20", view: "button", type: "image", click: optButtonClicked, image: resource_root+"/images/"+btn_height+"/blank.png", width: btn_height},
							{id: "optLbl20", view: "label", label: "", css: "optLbl", template: optLblTemplate},
							{id: "optBtn21", view: "button", type: "image", click: optButtonClicked, image: resource_root+"/images/"+btn_height+"/blank.png", width: btn_height},
							{id: "optLbl21", view: "label", label: "", css: "optLbl", template: optLblTemplate}
						], height: btn_height, id: "lytBtns8"},
						{cols: [
							{id: "optBtn30", view: "button", type: "image", click: optButtonClicked, image: resource_root+"/images/"+btn_height+"/blank.png", width: btn_height},
							{id: "optLbl30", view: "label", label: "", css: "optLbl", template: optLblTemplate},
							{id: "optBtn31", view: "button", type: "image", click: optButtonClicked, image: resource_root+"/images/"+btn_height+"/blank.png", width: btn_height},
							{id: "optLbl31", view: "label", label: "", css: "optLbl", template: optLblTemplate}
						], height: btn_height, id: "lytBtns9"},
						{cols: [
							{id: "optBtn40", view: "button", type: "image", click: optButtonClicked, image: resource_root+"/images/"+btn_height+"/blank.png", width: btn_height},
							{id: "optLbl40", view: "label", label: "", css: "optLbl", template: optLblTemplate},
							{id: "optBtn41", view: "button", type: "image", click: optButtonClicked, image: resource_root+"/images/"+btn_height+"/blank.png", width: btn_height},
							{id: "optLbl41", view: "label", label: "", css: "optLbl", template: optLblTemplate}
						], height: btn_height, id: "lytBtns10"},
						{cols: [
							{id: "optBtn50", view: "button", type: "image", click: optButtonClicked, image: resource_root+"/images/"+btn_height+"/blank.png", width: btn_height},
							{id: "optLbl50", view: "label", label: "", css: "optLbl", template: optLblTemplate},
							{id: "toOtherOptPad", view: "button", type: "htmlbutton", label: "More...", click: toOtherOptPadClicked, width: btn_height*2}
						], height: btn_height, id: "lytBtns11"}
					], id: "optPad"}
				]},
				{id: "lblDisclaimer", view: "label", width: "100%", height: disclaimer_height, click: disclaimerClicked, template: '<span style="line-height: 150%;">'+endlabel+'</span>'}
			]}
		]
	});

	if (sizeName != "Scalable") {
		$$("lytWindow").removeView("lytSpacer0");
	}
	webix.html.addCss($$("txtEntry").getNode(), "txtEntry"+sizeName);
	webix.html.addCss($$("txtStatusExponent").getNode(), "txtStandard"+sizeName);
	webix.html.addCss($$("txtStatusAngular").getNode(), "txtStandard"+sizeName);
	webix.html.addCss($$("txtStatusBase").getNode(), "txtStandard"+sizeName);
	webix.html.addCss($$("txtStack").getNode(), "txtMulti"+sizeName);
	webix.html.addCss($$("txtBase").getNode(), "txtMulti"+sizeName);
	for (var r=0;r<6;r++) {
		for (var c=0;c<6;c++) {
			webix.html.addCss($$("b"+r+c).getNode(), "btn"+sizeName);
		}
	}
	webix.html.addCss($$("toOtherOptPad").getNode(), "btn"+sizeName);
	for (var or=0;or<6;or++) {
		for (var oc=0;(oc<2)&&((or<5)||(oc<1));oc++) {
			webix.html.addCss($$("optLbl"+or+oc).getNode(), "txtOption"+sizeName);
		}
	}
}

function getSizes(winWidth, winHeight) {
	var result = {
		"tab_width": 96,
		"stack_height": 96,
		"entry_height": 64,
		"status_height": 15,
		"spacer_width": 0,
		"btn_height": 64,
		"disclaimer_height": 96
	};

	// Assume everything is a proportion of tab_width
	var vfudge = 30;
	var hfudge = 20;
	var spacer_fudge = -15;

	// Window width = simply (tab_width*5)+hfudge
	var tw_from_width = (winWidth-hfudge)/5.0;
	// Window height = tab+(6*btn)+entry+stack+status+vfudge
	//  entry = btn = tab*64/96
	//  stack = tab
	//  status = tab*15/96
	// therefore:
	// Window height = tab*(((6+1)*64/96)+2+(15/96))+vfudge
	//               = tab*((448/96)+(192/96)+(15/96))+vfudge
	//               = tab*655/96 + vfudge
	var tw_from_height = (winHeight-vfudge)*96.0/655.0;
	var constraint = "";
	if (tw_from_height < tw_from_width) {
		result['tab_width'] = tw_from_height;
		constraint = "height";
	}
	else {
		result['tab_width'] = tw_from_width;
		constraint = "width";
	}
	result['stack_height'] = result['tab_width'];
	result['btn_height'] = result['tab_width']*64.0/96.0;
	result['entry_height'] = result['btn_height'];
	result['status_height'] = result['tab_width']*15.0/96.0;
	result['full_width'] = (result['tab_width']*5)-4;
	result['spacer_width'] = (winWidth+spacer_fudge-result['full_width'])/2.0;
	if (constraint == "width") {
		// Update stack height
		var height_calculated = vfudge + result['tab_width']*655/96.0;
		result['stack_height'] += (winHeight-height_calculated);
	}
	return result;
}

function updateFontSizes(sizeInfo) {
	var btn_font_size = (sizeInfo['btn_height'] * 100.0/64.0);
	sc_button_style.innerHTML = '.btnScalable button { font-size: ' + btn_font_size.toFixed(0) + '%; }';
	var entry_font_size = btn_font_size * 2.0;
	sc_entry_style.innerHTML = '.txtEntryScalable { font-size: ' + entry_font_size.toFixed(0) + '%; }';
	var standard_font_size = btn_font_size;
	sc_standard_style.innerHTML = '.txtStandardScalable { font-size: ' + standard_font_size.toFixed(0) + '%; }';
	var multi_font_size = btn_font_size*80.0/100.0;
	sc_multi_style.innerHTML = '.txtMultiScalable { font-size: ' + multi_font_size.toFixed(0) + '%; }';
	var option_font_size = btn_font_size*0.9;
	sc_option_style.innerHTML = '.txtOptionScalable { font-size: ' + option_font_size.toFixed(0) + '%; }';
}

function resizeHandler() {
	var s = getSizes(window_width, window_height);
	updateFontSizes(s);

	$$("lytBtns0").define("height", s['btn_height']);
	$$("lytBtns1").define("height", s['btn_height']);
	$$("lytBtns2").define("height", s['btn_height']);
	$$("lytBtns3").define("height", s['btn_height']);
	$$("lytBtns4").define("height", s['btn_height']);
	$$("lytBtns5").define("height", s['btn_height']);
	$$("lytBtns6").define("height", s['btn_height']);
	$$("lytBtns7").define("height", s['btn_height']);
	$$("lytBtns8").define("height", s['btn_height']);
	$$("lytBtns9").define("height", s['btn_height']);
	$$("lytBtns10").define("height", s['btn_height']);
	$$("lytBtns11").define("height", s['btn_height']);

	$$('lytStatus').define('height', s['status_height']);
	$$('lytStack').define('height', s['stack_height']);
	$$('txtEntry').define('height', s['entry_height']);

	$$("optBtn00").define("width", s['btn_height']);
	$$("optBtn01").define("width", s['btn_height']);
	$$("optBtn10").define("width", s['btn_height']);
	$$("optBtn11").define("width", s['btn_height']);
	$$("optBtn20").define("width", s['btn_height']);
	$$("optBtn21").define("width", s['btn_height']);
	$$("optBtn30").define("width", s['btn_height']);
	$$("optBtn31").define("width", s['btn_height']);
	$$("optBtn40").define("width", s['btn_height']);
	$$("optBtn41").define("width", s['btn_height']);
	$$("optBtn50").define("width", s['btn_height']);
	$$("toOtherOptPad").define("width", s['btn_height']*2);

	$$("imNumPad").define("width", s['tab_width']);
	$$("imNumPad").define("height", s['tab_width']);
	$$("imFuncPad").define("width", s['tab_width']);
	$$("imFuncPad").define("height", s['tab_width']);
	$$("imConvPad").define("width", s['tab_width']);
	$$("imConvPad").define("height", s['tab_width']);
	$$("imConstPad").define("width", s['tab_width']);
	$$("imConstPad").define("height", s['tab_width']);
	$$("imOptPad").define("width", s['tab_width']);
	$$("imOptPad").define("height", s['tab_width']);
	
	$$("lytTop").define("width", s['full_width']);
	$$("lytSpacer0").define("width", s['spacer_width']);
	$$("lytWindow").define("width", window_width);
	$$("lytWindow").resize(true);
}

function init() {
	webix.ready(function() {
		btn_folder = tab_width;
		setupUi(size_name, initial_window_width, initial_window_height);

		hideOptPads();
		configureKeyboard();

		cancelPressed();
		updateDisplays();

		getCurrencyData();
		webix.UIManager.setFocus($$("txtEntry"));
		logger.log("Initialised (in "+size_name.toLowerCase()+" size)");
	});
}

function getCurrencyData() {
	fetch("https://ww2.cgtk.co.uk/currency.zip")
		.then((response) => {
			console.log(response);
			if ( ! response.ok) {
				return;
			}
			return response.blob();
		})
		.then((blob) => {
			console.log(blob);
			const zipFileReader = new zip.BlobReader(blob);
			const zipReader = new zip.ZipReader(zipFileReader);
			return zipReader.getEntries();
		})
		.then((entries) => {
			const contentsWriter = new zip.TextWriter();
			const firstEntry = entries.shift();
			return firstEntry.getData(contentsWriter);
		})
		.then((contents) => {
			console.log(contents);

			var lines = contents.split("\n");
			var rows = [];
			for (const line of lines) {
				if (line.trim().length == 0) {
					continue;
				}
				rows.push(line.split(","));
			}

			var wrtEuro = {};
			var datestr = "";
			for (var i=0;i<rows[0].length;i++) {
				if (rows[0][i].trim() == "Date") {
					datestr = rows[1][i].trim();
				}
				else if (rows[0][i].trim().length == 0) {
				}
				else {
					wrtEuro[rows[0][i].trim()] = parseFloat(rows[1][i].trim());
				}
			}

			// TODO read date (d MMMM yyyy ?) and convert to yyyy-MM-dd
			var datecheck = calc.processCurrencyData(JSON.stringify(wrtEuro), datestr);
			
			if ( datecheck.length > 0) {
				showToast(datecheck);
			}
		});
}

function cancelPressed() {
	storeMode = "None";
	tabSelect("numpad");
}

function storePressed() {
	storeMode = "store";
	tabSelect(lastStoreTab);
	showToast("Please select location");
}

function recallPressed() {
	storeMode = "recall";
	tabSelect(lastStoreTab);
	showToast("Please select location");
}

function copyXValue() {
	// TODO
}

function showToast(html, isHelp) {
	//parent.showToast(html, isHelp);
	//TODO
}

function moreConversions() {
	var categories = JSON.parse(calc.getConversionCategories());

	pickFromList("Conversion Category", categories, function(category) {
		var units = JSON.parse(calc.getAvailableUnits(category));
		pickFromList("Convert from", units, function(from_unit) {
			pickFromList("Convert to", units, function(to_unit) {
				buttonPress("Convert_" +
					category + "_" +
					from_unit + "_" +
					to_unit);
			});
		});
	});
}

function constByName() {
	var categories = JSON.parse(calc.getConstantCategories());
	pickFromList("Constant Category", categories, function(category) {
		var constants = JSON.parse(calc.getConstantsInCategory(category));
		pickFromList("Constant", constants, function(constant) {
			buttonPress("Const-" + constant);
		});
	});
}

function toggleHypMode() {
	hypMode = ! hypMode;
	tabSelect("funcpad");
}

function buttonClicked(args) {
	setTimeout(clearTooltips, 250);
	webix.UIManager.setFocus($$("txtEntry"));
	if (args in buttonMap) {
		var command = buttonMap[args]['Command'];
		buttonPress(command);
	}
}

function disclaimerClicked() {
	window.open("https://www.cgtk.co.uk/software/arpcalc", "_blank");
}

function optButtonClicked(args) {
	setTimeout(clearTooltips, 250);
	var optBtn = $$(args);
	var optKey = optBtn.data["arpcalc_option"];
	jscalc.callOptionClickAction(optKey);
	var icon = jscalc.callOptionIconAction(optKey);
	var icon_path = jscalc.getImageForDrawable(icon);
	var image_path = resource_root+"/images/"+btn_height+icon_path;
	optBtn.define({'image': image_path});
	optBtn.refresh();
	if (optBtn.data["requires_restart"]) {
		showToast("Restart for changes to take effect");
	}
	updateDisplays();
}

function toOtherOptPadClicked(args) {
	setTimeout(clearTooltips, 250);
	if (lastTab == "optpad2") {
		$$("toOtherOptPad").data.label = "More...";
		$$("toOtherOptPad").refresh();
		tabSelect("optpad1");
	}
	else {
		$$("toOtherOptPad").data.label = "Back...";
		$$("toOtherOptPad").refresh();
		tabSelect("optpad2");
	}
}

function nextTab() {
	switch (lastTab) {
		case "numpad":
			tabSelect("funcpad");
			break;
		case "funcpad":
		case "hypfuncpad":
			tabSelect("convpad");
			break;
		case "convpad":
			tabSelect("constpad");
			break;
		case "constpad":
			tabSelect("optpad1");
			break;
		case "optpad1":
			tabSelect("optpad2");
			break;
		case "romanupperpad":
			tabSelect("romanlowerpad");
			break;
		case "romanlowerpad":
			tabSelect("greekupperpad");
			break;
		case "greekupperpad":
			tabSelect("greeklowerpad");
			break;
		case "greeklowerpad":
			tabSelect("romanupperpad");
			break;
		default: // to catch SI pad
		case "optpad2":
			cancelPressed();
			break;
	}
}

function quitGracefully() {
	//parent.quitGracefully();
}

function enableFirebug() {
	//parent.enableFirebug();
}

function keypressHandler(code, e) {
	var modifiers = "";
	console.log("Keypress:", code);
	if (e.ctrlKey) {
		modifiers += "Ctrl"
	}
	if (e.shiftKey) {
		modifiers += "Shift";
	}
	if (e.altKey) {
		modifiers += "Alt";
	}
	if (modifiers.length == 0) {
		modifiers = "plain";
	}
	else {
		modifiers = modifiers.substr(0, 1).toLowerCase() + modifiers.substr(1);
	}

	var keyname = "";

	if ((code <= 90) && (code >= 48)) {
		keyname = String.fromCharCode(code);
	}

	if (code == 9)  { keyname = "Tab"; }
	else if (code == 8)  { keyname = "Backspace"; }
	else if (code == 13) { keyname = "Enter"; }
	else if (code == 27)  { keyname = "Esc"; }
	else if (code == 37) { keyname = "Left"; }
	else if (code == 38) { keyname = "Up"; }
	else if (code == 39) { keyname = "Right"; }
	else if (code == 40) { keyname = "Down"; }
	else if (code == 46)  { keyname = "Del"; }
	else if (code == 106) { keyname = "*"; }
	else if (code == 107) { keyname = "+"; }
	else if (code == 109) { keyname = "-"; }
	else if (code == 110) { keyname = "."; }
	else if (code == 111) { keyname = "/"; }
	else if (code == 187) { keyname = "="; }
	else if (code == 189) { keyname = "-"; }
	else if (code == 190) { keyname = "."; }
	else if (code == 191) { keyname = "/"; }
	else if (code == 220) { keyname = "\\"; }
	else if (code == 223) { keyname = "`"; }
	else if (code == 0xA3) { keyname = "sterling"; }
	else if (code == 8364) { keyname = "euro"; }
	else {}

	//console.log("Pressed key code", code, "with modifiers", modifiers, ":", keyname);
	if (keyname.length == 0) {
		console.log("Unknown key code", code);
		return true;
	}

	if (pickerPresent()) {
		return ! pickerKeyPress(keyname);
	}

	var result = JSON.parse(calc.handleKey(keyname, modifiers, lastTab));
	//console.log(result);

	if (result["HandlerResult"] == "NotHandled") {
		return true;
	}
	else if (result["HandlerResult"] == "Handled") {
		if (returnToNumPad) {
			tabSelect("numpad");
		}
		else {
			tabSelect("lastTab");
		}
	}
	else if (result["HandlerResult"] == "Cancel") {
		cancelPressed();
	}
	else if (result["HandlerResult"] == "NextTab") {
		nextTab();
	}
	else if (result["HandlerResult"] == "MoreConversions") {
		moreConversions();
	}
	else if (result["HandlerResult"] == "ConstByName") {
		constByName();
	}
	else if (result["HandlerResult"] == "SI") {
		buttonPress("SI");
	}
	else if (result["HandlerResult"] == "Store") {
		storePressed();
	}
	else if (result["HandlerResult"] == "Recall") {
		recallPressed();
	}
	else if (result["HandlerResult"] == "CopyToClipboard") {
		// Not handled in web version
	}
	else if (result["HandlerResult"] == "PasteFromClipboard") {
		// Not handled in web version
	}
	else if (result["HandlerResult"] == "Quit") {
		// Not handled in web version
	}
	else {
		console.log("Unrecognised handler result:", result["HandlerResult"]);
	}

	handleErrorCode(result["ErrorCode"]);
	updateDisplays();
	return false;
}

function configureKeyboard() {
	$$("txtEntry").attachEvent("onKeyPress", keypressHandler);
	webix.attachEvent("onFocusChange", function(current_view, prev_view) {
		if (current_view != $$("txtEntry")) {
			webix.UIManager.setFocus($$("txtEntry"));
			setTimeout(function() {
				webix.UIManager.setFocus($$("txtEntry"));
			}, 50);
		}
	});
}

function buttonPress(command) {
	var handled = true;

	switch (command) {
		case "SI"              : tabSelect("sipad"); break;
		case "hypmode"         : toggleHypMode(); break;
		case "store"           : storePressed(); break;
		case "recall"          : recallPressed(); break;
		case "NOP"             : break;
		case "CANCEL"          : cancelPressed(); break;
		case "MoreConversions" : moreConversions(); break;
		case "ConstByName"     : constByName(); break;
		default                : handled = false;
	}

	if (( ! handled) && command.startsWith("Store")) {
		handled = true;
		if (storeMode === "store") {
			calc.store(command);
		}
		else if (storeMode === "recall") {
			calc.recall(command);
		}
		else {
			cancelPressed();
		}
		tabSelect("numpad");
	}

	if ((command != "store") && (command != "recall")) {
		storeMode = "None";
	}

	if ( ! handled) {
		hypMode = false;
		var result = jscalc.keypress_js(/*calc, */command);
		handleErrorCode(result);

		if (returnToNumPad) {
			tabSelect("numpad");
		}
		else {
			tabSelect(lastTab);
		}
	}

	updateDisplays();
}

function handleErrorCode(ec) {
	switch (ec) {
		case "NoFunction"         : showToast("Unknown function: " + command); break;
		case "NotImplemented"     : showToast("Not implemented yet: " + command); break;
		case "DivideByZero"       : showToast("Divide by zero error"); break;
		case "InvalidRoot"        : showToast("Invalid root error"); break;
		case "InvalidLog"         : showToast("Invalid log error"); break;
		case "InvalidTan"         : showToast("Invalid tangent error"); break;
		case "InvalidInverseTrig" : showToast("Invalid inverse trigonometry error"); break;
		case "InvalidInverseHypTrig" : showToast("Invalid inverse hyperbolic trigonometry error"); break;
		case "UnknownConstant"    : showToast("Unknown constant error"); break;
		case "UnknownConversion"  : showToast("Unknown conversion error"); break;
		case "InvalidConversion"  : showToast("Conversion error"); break;
		case "UnknownSI"          : showToast("Unknown SI unit"); break;
			// no default - do nothing if no error
	}
}

function updateDisplays() {
	var xText = calc.getXDisplay();
	var stackDisplay = calc.getStackDisplay();
	var baseDisplay = calc.getBaseDisplay();

	setEntryLabel(xText);
	writeStack(stackDisplay.split("<br>"), 5);
	setBaseLabel(baseDisplay);

	var stExp = calc.getStatusExponent();
	var stAng = calc.getStatusAngularUnits();
	var stBase = calc.getStatusBase();
	
	setLabelText("txtStatusExponent", stExp);
	setLabelText("txtStatusAngular", stAng);
	setLabelText("txtStatusBase", stBase);

	// TODO: Scrolling when implemented


	// Make sure focus stays with txtEntry
	webix.UIManager.setFocus($$("txtEntry"));

	fixTooltips();
}

function fixTooltips() {
	// Make tooltips support html content
	$(document).tooltip({
	    content: function() {
	        return $(this).prop("title");
	    },
	    show: {
			delay: 1000
		}
	});
}

function clearTooltips() {
	$('.ui-tooltip').remove();
}

function setTabButtonImage(button, im, help) {
	var image_path = resource_root+"/images/"+btn_folder+"/"+im+".png";
	$$(button).define({'image': image_path});
	$$(button).data.tooltip = help;
	$$(button).refresh();
}

function setTabButtonImages(normal) {
	var helpSuffix = " (double press to stay on this tab)";
	var numpadHelp = "Switch to number pad";

	if (normal) {
		setTabButtonImage("imNumPad", "numpad", "Main keypad");
		setTabButtonImage("imFuncPad", "funcpad", "Maths functions" + helpSuffix);
		setTabButtonImage("imConvPad", "convpad", "Unit conversion" + helpSuffix);
		setTabButtonImage("imConstPad", "constpad", "Scientific constants" + helpSuffix);
		setTabButtonImage("imOptPad", "optpad", "Configuration");
	}
	else {
		setTabButtonImage("imNumPad", "numpad", "Go back to the main keypad");
		setTabButtonImage("imFuncPad", "romanupper", "Use Roman Upper Case storage locations");
		setTabButtonImage("imConvPad", "romanlower", "Use Roman Lower Case storage locations");
		setTabButtonImage("imConstPad", "greekupper", "Use Greek Upper Case storage locations");
		setTabButtonImage("imOptPad", "greeklower", "Use Greek Lower Case storage locations");
	}
}

function tabSelect(tab) {
	var local_tab = tab;
	var store_tabs = ["romanupperpad", "romanlowerpad",
		"greekupperpad", "greeklowerpad"];
	var tab_map = {
		'funcpad': 'romanupperpad',
		'convpad': 'romanlowerpad',
		'constpad': 'greekupperpad',
		'optpad1': 'greeklowerpad',
		'optpad': 'greeklowerpad'
	};

	if ((local_tab == lastTab) && (local_tab != "numpad")) {
		returnToNumPad = false;
	}
	else {
		returnToNumPad = true;
	}
	if ((store_tabs.indexOf(local_tab) != -1) || ((storeMode != "None") && (local_tab != "numpad"))) {
		returnToNumPad = true;
		setTabButtonImages(false);

		if (local_tab in tab_map) {
			local_tab = tab_map[local_tab];
		}
	}
	else {
		storeMode = "None";
		setTabButtonImages(true);
	}

	if ((local_tab == "optpad1") || (local_tab == "optpad")) {
		local_tab = "optpad1";
		lastTab = local_tab;
		showOptPad1();
		return;
	}
	if (local_tab == "optpad2") {
		lastTab = local_tab;
		showOptPad2();
		return;
	}
	if (hypMode && (local_tab == "funcpad")) {
		local_tab = "hypfuncpad";
	}

	var tabGridJSON = jscalc.getJSCompatibleGrid(/*calc, */local_tab);
	tabGrid = JSON.parse(tabGridJSON)/*["list"]*/;

	for (rowIndex=0;rowIndex<tabGrid.length;rowIndex++) {
		var buttonIncrement = 0;
		var row = tabGrid[rowIndex];
		for (colIndex=0;colIndex<row.length;colIndex++) {
			bi = row[colIndex];
			buttonName = "b"+rowIndex+(colIndex+buttonIncrement);
			buttonMap[buttonName] = {'Command': bi.name, 'Help': bi.helpText};
			var gravity = 1;
			if (bi.hidden) {
				$$(buttonName).disable();
			}
			else if (bi.doubleWidth) {
				gravity = 2;
				buttonIncrement += 1;
				var hiddenName = "b"+rowIndex+(colIndex+buttonIncrement);
				setButtonLabel(hiddenName, "", "", 0);
				$$(buttonName).enable();
			}
			else {
				$$(buttonName).enable();
			}
			var help = bi.helpText;
			var shortcuts = JSON.parse(calc.getShortcutKeys(bi.name));
			var spanstyle = '<span style="font-family: monospace; font-size: 120%;">';
			if ((shortcuts.length > 0) && (help.length > 0)) {
				help += " (" +
					spanstyle +
					shortcuts.join("</span> or "+spanstyle) + 
					'</span>' +
					")";
			}

			setButtonLabel(buttonName, bi.display, help, gravity);
		}
	}
	hideOptPads();

	ui.adjust();

	lastTab = local_tab;

	// Make sure focus stays with txtEntry
	webix.UIManager.setFocus($$("txtEntry"));
}

function showOptPad1() {
	showOptPad(1);
}

function showOptPad(num) {
	var optListJson = jscalc.getOptionsAsJson(/*calc*/);
	var optList = JSON.parse(optListJson)/*["map"]*/;
	var r, c, page, or, oc, btn, lbl;

	$$("optPad").show();
	$$("btnPad").hide();

	// initialise: hide everything
	for (or=0;or<6;or++) {
		for (oc=0;(oc<2)&&((or<5)||(oc<1));oc++) {
			btn = $$("optBtn"+or+oc);
			lbl = $$("optLbl"+or+oc);

			btn.define({'image': ''});
			btn.define({"arpcalc_option": ""});
			btn.define({"requires_restart": false});
			btn.data.tooltip = "";
			btn.disable();

            lbl.data.tooltip = "";
			lbl.data.label = "";
			lbl.disable();
		}
	}

	for (var key in optList) {
		if (key == "BLANK") {
			continue;
		}
		if ((size_name == "Scalable") && (key == "Window Size")) {
			continue;
		}
		if (optList[key]["hideOnWeb"]) {
			continue;
		}
		page = optList[key]["location"]["page"];
		r = optList[key]["location"]["row"];
		c = optList[key]["location"]["col"];
		// optList columns are 0 and 3 instead of 0 and 1
		if (c == 3) {
			c = 1;
		}
		if (page != (num-1)) {
			continue;
		}
		lbl = $$("optLbl"+r+c);
		lbl.data.label = key;
		lbl.data.tooltip = optList[key]["helpText"];
		lbl.enable();

		var icon = jscalc.callOptionIconAction(key);
		var icon_path = jscalc.getImageForDrawable(icon);
		var image_path = resource_root+"/images/"+btn_folder+icon_path;
		btn = $$("optBtn"+r+c);
		btn.define({'image': image_path});
		btn.define({"arpcalc_option": key});
		btn.define({"requires_restart": optList[key]["requiresRestart"]});
		btn.data.tooltip = optList[key]["helpText"];
		btn.enable();
	}

	for (or=0;or<6;or++) {
		for (oc=0;(oc<2)&&((oc<5)||(oc1));oc++) {
			btn = $$("optBtn"+or+oc);
			lbl = $$("optLbl"+or+oc);
			if (btn === undefined) {
				continue;
			}
			btn.refresh();
			lbl.refresh();
		}
	}
}

function showOptPad2() {
	showOptPad(2);
}

function hideOptPads() {
	$$("optPad").hide();
	$$("btnPad").show();
}

function writeStack(lines, lineCount) {
	var txtStack = $$("txtStack");
	var text = "";
	var startIndex = 0;
	if (lines.length > lineCount) {
		startIndex = lines.length - lineCount;
	}
	for (var b=0;b<(lineCount-lines.length);b++) {
		text += "<br>";
	}
	for (var i=startIndex;i<lines.length;i++) {
		text += "<br>";
		text += lines[i];
	}
	txtStack.$view.innerHTML = text;
}

function setButtonLabel(btn, label, tooltip, gravity) {
	if (typeof(gravity) === "undefined") {
		gravity = 1;
	}
	$$(btn).data.gravity = gravity;
	$$(btn).data.label = label;
	$$(btn).data.tooltip = tooltip;
	$$(btn).refresh();
}

function setBaseLabel(entry) {
	$$("txtBase").$view.innerHTML = entry;
}

function setLabelText(label, text) {
	$$(label).data.label = text;
	$$(label).refresh();
}

function setEntryLabel(entry) {
	setLabelText("txtEntry", entry+"<sup><small>&nbsp;</sup><small>");
}

function tabButtonClicked(args) {
	setTimeout(clearTooltips, 250);
	tabSelect(args.substring(2).toLowerCase());
	webix.UIManager.setFocus($$("txtEntry"));
}

/* vim: set noet : */
