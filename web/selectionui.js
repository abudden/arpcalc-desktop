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
function pickFromList(title, options, callback) {
    var pickerWidth = $$("lytTop").$width;
    var pickerHeight = $$("lytStatus").$height +
        $$("lytStack").$height +
        $$("txtEntry").$height +
        $$("lytTabs").$height +
        $$("btnPad").$height;
    var picker = webix.ui({
        view: "window",
        modal: true,
        width: pickerWidth,
        height: pickerHeight,
        id: "winPicker",
        head: title,
        body: {
            rows: [
                {view: "list", template: "#title# #shortcut#", id: "lstPicker", on: {
                    onItemClick(id) {
                        const item = this.getItem(id);
                        console.log("Item clicked", item, item.title);
                        pickerCancelClicked();
                        callback(item.title);
                    }
                }},
                {view: "button", type: "htmlbutton", id: "btnPickerCancel", label: "Cancel", click: "pickerCancelClicked"}
            ]
        }
    });
    for (var i=0;i<options.length;i++) {
        var fields = {'title': options[i], 'shortcut': ''};
        if (i < 26) {
            fields['shortcut'] = '(' + String.fromCharCode(i+"A".charCodeAt(0)) + ")";
        }
        fields['callback'] = callback;
        $$("lstPicker").add(fields);
    }
    picker_g = picker;
    $$("winPicker").show();
}

function pickerPresent() {
    var picker = $$("winPicker");
    if (picker) {
        return true;
    }
    return false;
}

function pickerKeyPress(key) {
    var picker = $$("lstPicker");
    if (picker) {
        if (key == 'Esc') {
            pickerCancelClicked();
            return true;
        }
        if (key.length != 1) {
            return false;
        }
        var uckey = key.toUpperCase();
        var code = uckey.charCodeAt(0) - "A".charCodeAt(0);
        if ((code < 0) || (code >= 26)) {
            return false;
        }
        var shortcut = "(" + uckey + ")";
        for (var idx=0;idx<picker.count();idx++) {
            var item = picker.getItem(picker.getIdByIndex(idx));
            if (item['shortcut'] == shortcut) {
                pickerCancelClicked();
                item['callback'](item.title);
                return true;
            }
        }
    }
    return false;
}

function pickerCancelClicked() {
    $$("winPicker").close();
}
