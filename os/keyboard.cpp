//
// C++ Implementation: keyboard
//
// Description: 
//
//
// Author: Juan Linietsky <red@lunatea>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "keyboard.h"


const char *Keyboard::key_names[KEY_MAX] = { /* List Taken from SDL */
	
	"Unknown",
	"First",
	"BackSpace",
	"Tab",
	"Clear",
	"Return",
	"Pause",
	"Escape",
	"Space",
	"Exclaim",
	"DoubleQuote",
	"Hash",
	"Dollar",
	"Ampersand",
	"Quote",
	"Left Pharentesis",
	"Right Pharentesis",
	"Asterisk",
	"Plus",
	"Comma",
	"Minus",
	"Period",
	"Slash",
	"0",
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9",
	"Colon",
	"Semicolon",
	"Less",
	"Equals",
	"Greater",
	"Question",
	"At",
	"Left Bracket",
	"BackSlash",
	"Right Bracket",
	"Caret",
	"Underscore",
	"Backquote",
	"A",
	"B",
	"C",
	"D",
	"E",
	"F",
	"G",
	"H",
	"I",
	"J",
	"K",
	"L",
	"M",
	"N",
	"O",
	"P",
	"Q",
	"R",
	"S",
	"T",
	"U",
	"V",
	"W",
	"X",
	"Y",
	"Z",
	"Delete",
	"Intl. 0",
	"Intl. 1",
	"Intl. 2",
	"Intl. 3",
	"Intl. 4",
	"Intl. 5",
	"Intl. 6",
	"Intl. 7",
	"Intl. 8",
	"Intl. 9",
	"Intl. 10",
	"Intl. 11",
	"Intl. 12",
	"Intl. 13",
	"Intl. 14",
	"Intl. 15",
	"Intl. 16",
	"Intl. 17",
	"Intl. 18",
	"Intl. 19",
	"Intl. 20",
	"Intl. 21",
	"Intl. 22",
	"Intl. 23",
	"Intl. 24",
	"Intl. 25",
	"Intl. 26",
	"Intl. 27",
	"Intl. 28",
	"Intl. 29",
	"Intl. 30",
	"Intl. 31",
	"Intl. 32",
	"Intl. 33",
	"Intl. 34",
	"Intl. 35",
	"Intl. 36",
	"Intl. 37",
	"Intl. 38",
	"Intl. 39",
	"Intl. 40",
	"Intl. 41",
	"Intl. 42",
	"Intl. 43",
	"Intl. 44",
	"Intl. 45",
	"Intl. 46",
	"Intl. 47",
	"Intl. 48",
	"Intl. 49",
	"Intl. 50",
	"Intl. 51",
	"Intl. 52",
	"Intl. 53",
	"Intl. 54",
	"Intl. 55",
	"Intl. 56",
	"Intl. 57",
	"Intl. 58",
	"Intl. 59",
	"Intl. 60",
	"Intl. 61",
	"Intl. 62",
	"Intl. 63",
	"Intl. 64",
	"Intl. 65",
	"Intl. 66",
	"Intl. 67",
	"Intl. 68",
	"Intl. 69",
	"Intl. 70",
	"Intl. 71",
	"Intl. 72",
	"Intl. 73",
	"Intl. 74",
	"Intl. 75",
	"Intl. 76",
	"Intl. 77",
	"Intl. 78",
	"Intl. 79",
	"Intl. 80",
	"Intl. 81",
	"Intl. 82",
	"Intl. 83",
	"Intl. 84",
	"Intl. 85",
	"Intl. 86",
	"Intl. 87",
	"Intl. 88",
	"Intl. 89",
	"Intl. 90",
	"Intl. 91",
	"Intl. 92",
	"Intl. 93",
	"Intl. 94",
	"Intl. 95",
	"KeyPad 0",
	"KeyPad 1",
	"KeyPad 2",
	"KeyPad 3",
	"KeyPad 4",
	"KeyPad 5",
	"KeyPad 6",
	"KeyPad 7",
	"KeyPad 8",
	"KeyPad 9",
	"KeyPad Period",
	"KeyPad Divide",
	"KeyPad Multiply",
	"KeyPad Minus",
	"KeyPad Plus",
	"KeyPad Enter",
	"KeyPad Equals",
	"Up",
	"Down",
	"Right",
	"Left",
	"Insert",
	"Home",
	"End",
	"Pageup",
	"Pagedown",
	"F1",
	"F2",
	"F3",
	"F4",
	"F5",
	"F6",
	"F7",
	"F8",
	"F9",
	"F10",
	"F11",
	"F12",
	"F13",
	"F14",
	"F15",
	"Num Lock",
	"Caps Lock",
	"Scrol Lock",
	"Right Shift",
	"Left Shift",
	"Right Control",
	"Left Control",
	"Right Alt",
	"Left Alt",
	"Right Meta",
	"Left Meta",
	"Left Super",
	"Right Super",
	"Mode",
	"Compose",
	"Help",
	"Print",
	"Sysreq",
	"Break",
	"Menu",
	"Power",
	"Euro",
	"Undo"
};

String Keyboard::get_code_name(unsigned int p_code) { ///< Keycode and modifiers accepted
	
	int code=p_code&KEY_CODE_MASK;
	
	if (code>=KEY_MAX)
		code=0; //unknown
	
	String code_str=key_names[code]?key_names[code]:"";
	
	if (code==0)
		return code_str;

	if (p_code&KEY_MASK_META)
		code_str="Meta + "+code_str;
	if (p_code&KEY_MASK_CTRL)
		code_str="Ctrl + "+code_str;
	if (p_code&KEY_MASK_SHIFT)
		code_str="Shift + "+code_str;
	if (p_code&KEY_MASK_ALT)
		code_str="Alt + "+code_str;

		
	return code_str;			
	
}


Keyboard::Keyboard() {
	
	
	
	
}


Keyboard::~Keyboard()
{
}




	
void KeycodeTranslator::add_code(unsigned int p_foreign,KeyScanCode p_code) {
	
	if (p_code==KEY_MAX)
		return; //pointless
	
	table[p_code]=p_foreign;
	
}
KeyScanCode KeycodeTranslator::get_code(unsigned int p_foreign) {
	
	/* not the fastest ever.. but works. Since it's done once per keypress, it's not an issue.
	May change to hashtable later, but dont think so */
	
	for (int i=0;i<KEY_MAX;i++) {
		
		if (table[i]==p_foreign)
			return (KeyScanCode)i; //this should be safe
	}
	
	return KEY_UNKNOWN;
}

KeycodeTranslator::KeycodeTranslator() {
	
	for (int i=0;i<KEY_MAX;i++)
		table[i]=0; // all unknown by default!
	
}
KeycodeTranslator::~KeycodeTranslator() {
	
	
}
