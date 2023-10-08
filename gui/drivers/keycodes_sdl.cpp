#ifdef SDL_ENABLED

//
// C++ Implementation: keycodes_sdl
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "keycodes_sdl.h"

#include <SDL_keysym.h>

namespace GUI {

KeycodesSDL::KeycodesSDL()
{
	
	
	add_code( SDLK_UNKNOWN,     KEY_UNKNOWN);
	add_code( SDLK_FIRST,       KEY_FIRST);
	add_code( SDLK_BACKSPACE,   KEY_BACKSPACE);
	add_code( SDLK_TAB,         KEY_TAB);
	add_code( SDLK_CLEAR,       KEY_CLEAR);
	add_code( SDLK_RETURN,      KEY_RETURN);
	add_code( SDLK_PAUSE,       KEY_PAUSE);
	add_code( SDLK_ESCAPE,      KEY_ESCAPE);
	add_code( SDLK_SPACE,       KEY_SPACE);
	add_code( SDLK_EXCLAIM,     KEY_EXCLAIM);
	add_code( SDLK_QUOTEDBL,    KEY_QUOTEDBL);
	add_code( SDLK_HASH,        KEY_HASH);
	add_code( SDLK_DOLLAR,      KEY_DOLLAR);
	add_code( SDLK_AMPERSAND,   KEY_AMPERSAND);
	add_code( SDLK_QUOTE,       KEY_QUOTE);
	add_code( SDLK_LEFTPAREN,   KEY_LEFTPAREN);
	add_code( SDLK_RIGHTPAREN,  KEY_RIGHTPAREN);
	add_code( SDLK_ASTERISK,    KEY_ASTERISK);
	add_code( SDLK_PLUS,        KEY_PLUS);
	add_code( SDLK_COMMA,       KEY_COMMA);
	add_code( SDLK_MINUS,       KEY_MINUS);
	add_code( SDLK_PERIOD,      KEY_PERIOD);
	add_code( SDLK_SLASH,       KEY_SLASH);
	add_code( SDLK_0,           KEY_0);
	add_code( SDLK_1,           KEY_1);
	add_code( SDLK_2,           KEY_2);
	add_code( SDLK_3,           KEY_3);
	add_code( SDLK_4,           KEY_4);
	add_code( SDLK_5,           KEY_5);
	add_code( SDLK_6,           KEY_6);
	add_code( SDLK_7,           KEY_7);
	add_code( SDLK_8,           KEY_8);
	add_code( SDLK_9,           KEY_9);
	add_code( SDLK_COLON,       KEY_COLON);
	add_code( SDLK_SEMICOLON,   KEY_SEMICOLON);
	add_code( SDLK_LESS,        KEY_LESS);
	add_code( SDLK_EQUALS,      KEY_EQUALS);
	add_code( SDLK_GREATER,     KEY_GREATER);
	add_code( SDLK_QUESTION,    KEY_QUESTION);
	add_code( SDLK_AT,          KEY_AT);
	add_code( SDLK_LEFTBRACKET, KEY_LEFTBRACKET);
	add_code( SDLK_BACKSLASH,   KEY_BACKSLASH);
	add_code( SDLK_RIGHTBRACKET,KEY_RIGHTBRACKET);
	add_code( SDLK_CARET,       KEY_CARET);
	add_code( SDLK_UNDERSCORE,  KEY_UNDERSCORE);
	add_code( SDLK_BACKQUOTE,   KEY_BACKQUOTE);
	add_code( SDLK_a,           KEY_a);
	add_code( SDLK_b,           KEY_b);
	add_code( SDLK_c,           KEY_c);
	add_code( SDLK_d,           KEY_d);
	add_code( SDLK_e,           KEY_e);
	add_code( SDLK_f,           KEY_f);
	add_code( SDLK_g,           KEY_g);
	add_code( SDLK_h,           KEY_h);
	add_code( SDLK_i,           KEY_i);
	add_code( SDLK_j,           KEY_j);
	add_code( SDLK_k,           KEY_k);
	add_code( SDLK_l,           KEY_l);
	add_code( SDLK_m,           KEY_m);
	add_code( SDLK_n,           KEY_n);
	add_code( SDLK_o,           KEY_o);
	add_code( SDLK_p,           KEY_p);
	add_code( SDLK_q,           KEY_q);
	add_code( SDLK_r,           KEY_r);
	add_code( SDLK_s,           KEY_s);
	add_code( SDLK_t,           KEY_t);
	add_code( SDLK_u,           KEY_u);
	add_code( SDLK_v,           KEY_v);
	add_code( SDLK_w,           KEY_w);
	add_code( SDLK_x,           KEY_x);
	add_code( SDLK_y,           KEY_y);
	add_code( SDLK_z,           KEY_z);
	add_code( SDLK_DELETE,      KEY_DELETE);
	add_code( SDLK_WORLD_0,     KEY_WORLD_0);
	add_code( SDLK_WORLD_1,     KEY_WORLD_1);
	add_code( SDLK_WORLD_2,     KEY_WORLD_2);
	add_code( SDLK_WORLD_3,     KEY_WORLD_3);
	add_code( SDLK_WORLD_4,     KEY_WORLD_4);
	add_code( SDLK_WORLD_5,     KEY_WORLD_5);
	add_code( SDLK_WORLD_6,     KEY_WORLD_6);
	add_code( SDLK_WORLD_7,     KEY_WORLD_7);
	add_code( SDLK_WORLD_8,     KEY_WORLD_8);
	add_code( SDLK_WORLD_9,     KEY_WORLD_9);
	add_code( SDLK_WORLD_10,    KEY_WORLD_10);
	add_code( SDLK_WORLD_11,    KEY_WORLD_11);
	add_code( SDLK_WORLD_12,    KEY_WORLD_12);
	add_code( SDLK_WORLD_13,    KEY_WORLD_13);
	add_code( SDLK_WORLD_14,    KEY_WORLD_14);
	add_code( SDLK_WORLD_15,    KEY_WORLD_15);
	add_code( SDLK_WORLD_16,    KEY_WORLD_16);
	add_code( SDLK_WORLD_17,    KEY_WORLD_17);
	add_code( SDLK_WORLD_18,    KEY_WORLD_18);
	add_code( SDLK_WORLD_19,    KEY_WORLD_19);
	add_code( SDLK_WORLD_20,    KEY_WORLD_20);
	add_code( SDLK_WORLD_21,    KEY_WORLD_21);
	add_code( SDLK_WORLD_22,    KEY_WORLD_22);
	add_code( SDLK_WORLD_23,    KEY_WORLD_23);
	add_code( SDLK_WORLD_24,    KEY_WORLD_24);
	add_code( SDLK_WORLD_25,    KEY_WORLD_25);
	add_code( SDLK_WORLD_26,    KEY_WORLD_26);
	add_code( SDLK_WORLD_27,    KEY_WORLD_27);
	add_code( SDLK_WORLD_28,    KEY_WORLD_28);
	add_code( SDLK_WORLD_29,    KEY_WORLD_29);
	add_code( SDLK_WORLD_30,    KEY_WORLD_30);
	add_code( SDLK_WORLD_31,    KEY_WORLD_31);
	add_code( SDLK_WORLD_32,    KEY_WORLD_32);
	add_code( SDLK_WORLD_33,    KEY_WORLD_33);
	add_code( SDLK_WORLD_34,    KEY_WORLD_34);
	add_code( SDLK_WORLD_35,    KEY_WORLD_35);
	add_code( SDLK_WORLD_36,    KEY_WORLD_36);
	add_code( SDLK_WORLD_37,    KEY_WORLD_37);
	add_code( SDLK_WORLD_38,    KEY_WORLD_38);
	add_code( SDLK_WORLD_39,    KEY_WORLD_39);
	add_code( SDLK_WORLD_40,    KEY_WORLD_40);
	add_code( SDLK_WORLD_41,    KEY_WORLD_41);
	add_code( SDLK_WORLD_42,    KEY_WORLD_42);
	add_code( SDLK_WORLD_43,    KEY_WORLD_43);
	add_code( SDLK_WORLD_44,    KEY_WORLD_44);
	add_code( SDLK_WORLD_45,    KEY_WORLD_45);
	add_code( SDLK_WORLD_46,    KEY_WORLD_46);
	add_code( SDLK_WORLD_47,    KEY_WORLD_47);
	add_code( SDLK_WORLD_48,    KEY_WORLD_48);
	add_code( SDLK_WORLD_49,    KEY_WORLD_49);
	add_code( SDLK_WORLD_50,    KEY_WORLD_50);
	add_code( SDLK_WORLD_51,    KEY_WORLD_51);
	add_code( SDLK_WORLD_52,    KEY_WORLD_52);
	add_code( SDLK_WORLD_53,    KEY_WORLD_53);
	add_code( SDLK_WORLD_54,    KEY_WORLD_54);
	add_code( SDLK_WORLD_55,    KEY_WORLD_55);
	add_code( SDLK_WORLD_56,    KEY_WORLD_56);
	add_code( SDLK_WORLD_57,    KEY_WORLD_57);
	add_code( SDLK_WORLD_58,    KEY_WORLD_58);
	add_code( SDLK_WORLD_59,    KEY_WORLD_59);
	add_code( SDLK_WORLD_60,    KEY_WORLD_60);
	add_code( SDLK_WORLD_61,    KEY_WORLD_61);
	add_code( SDLK_WORLD_62,    KEY_WORLD_62);
	add_code( SDLK_WORLD_63,    KEY_WORLD_63);
	add_code( SDLK_WORLD_64,    KEY_WORLD_64);
	add_code( SDLK_WORLD_65,    KEY_WORLD_65);
	add_code( SDLK_WORLD_66,    KEY_WORLD_66);
	add_code( SDLK_WORLD_67,    KEY_WORLD_67);
	add_code( SDLK_WORLD_68,    KEY_WORLD_68);
	add_code( SDLK_WORLD_69,    KEY_WORLD_69);
	add_code( SDLK_WORLD_70,    KEY_WORLD_70);
	add_code( SDLK_WORLD_71,    KEY_WORLD_71);
	add_code( SDLK_WORLD_72,    KEY_WORLD_72);
	add_code( SDLK_WORLD_73,    KEY_WORLD_73);
	add_code( SDLK_WORLD_74,    KEY_WORLD_74);
	add_code( SDLK_WORLD_75,    KEY_WORLD_75);
	add_code( SDLK_WORLD_76,    KEY_WORLD_76);
	add_code( SDLK_WORLD_77,    KEY_WORLD_77);
	add_code( SDLK_WORLD_78,    KEY_WORLD_78);
	add_code( SDLK_WORLD_79,    KEY_WORLD_79);
	add_code( SDLK_WORLD_80,    KEY_WORLD_80);
	add_code( SDLK_WORLD_81,    KEY_WORLD_81);
	add_code( SDLK_WORLD_82,    KEY_WORLD_82);
	add_code( SDLK_WORLD_83,    KEY_WORLD_83);
	add_code( SDLK_WORLD_84,    KEY_WORLD_84);
	add_code( SDLK_WORLD_85,    KEY_WORLD_85);
	add_code( SDLK_WORLD_86,    KEY_WORLD_86);
	add_code( SDLK_WORLD_87,    KEY_WORLD_87);
	add_code( SDLK_WORLD_88,    KEY_WORLD_88);
	add_code( SDLK_WORLD_89,    KEY_WORLD_89);
	add_code( SDLK_WORLD_90,    KEY_WORLD_90);
	add_code( SDLK_WORLD_91,    KEY_WORLD_91);
	add_code( SDLK_WORLD_92,    KEY_WORLD_92);
	add_code( SDLK_WORLD_93,    KEY_WORLD_93);
	add_code( SDLK_WORLD_94,    KEY_WORLD_94);
	add_code( SDLK_WORLD_95,    KEY_WORLD_95);
	add_code( SDLK_KP0,         KEY_KP0);
	add_code( SDLK_KP1,         KEY_KP1);
	add_code( SDLK_KP2,         KEY_KP2);
	add_code( SDLK_KP3,         KEY_KP3);
	add_code( SDLK_KP4,         KEY_KP4);
	add_code( SDLK_KP5,         KEY_KP5);
	add_code( SDLK_KP6,         KEY_KP6);
	add_code( SDLK_KP7,         KEY_KP7);
	add_code( SDLK_KP8,         KEY_KP8);
	add_code( SDLK_KP9,         KEY_KP9);
	add_code( SDLK_KP_PERIOD,   KEY_KP_PERIOD);
	add_code( SDLK_KP_DIVIDE,   KEY_KP_DIVIDE);
	add_code( SDLK_KP_MULTIPLY, KEY_KP_MULTIPLY);
	add_code( SDLK_KP_MINUS,    KEY_KP_MINUS);
	add_code( SDLK_KP_PLUS,     KEY_KP_PLUS);
	add_code( SDLK_KP_ENTER,    KEY_KP_ENTER);
	add_code( SDLK_KP_EQUALS,   KEY_KP_EQUALS);
	add_code( SDLK_UP,          KEY_UP);
	add_code( SDLK_DOWN,        KEY_DOWN);
	add_code( SDLK_RIGHT,       KEY_RIGHT);
	add_code( SDLK_LEFT,        KEY_LEFT);
	add_code( SDLK_INSERT,      KEY_INSERT);
	add_code( SDLK_HOME,        KEY_HOME);
	add_code( SDLK_END,         KEY_END);
	add_code( SDLK_PAGEUP,      KEY_PAGEUP);
	add_code( SDLK_PAGEDOWN,    KEY_PAGEDOWN);
	add_code( SDLK_F1,          KEY_F1);
	add_code( SDLK_F2,          KEY_F2);
	add_code( SDLK_F3,          KEY_F3);
	add_code( SDLK_F4,          KEY_F4);
	add_code( SDLK_F5,          KEY_F5);
	add_code( SDLK_F6,          KEY_F6);
	add_code( SDLK_F7,          KEY_F7);
	add_code( SDLK_F8,          KEY_F8);
	add_code( SDLK_F9,          KEY_F9);
	add_code( SDLK_F10,         KEY_F10);
	add_code( SDLK_F11,         KEY_F11);
	add_code( SDLK_F12,         KEY_F12);
	add_code( SDLK_F13,         KEY_F13);
	add_code( SDLK_F14,         KEY_F14);
	add_code( SDLK_F15,         KEY_F15);
	add_code( SDLK_NUMLOCK,     KEY_NUMLOCK);
	add_code( SDLK_CAPSLOCK,    KEY_CAPSLOCK);
	add_code( SDLK_SCROLLOCK,   KEY_SCROLLOCK);
	add_code( SDLK_RSHIFT,      KEY_RSHIFT);
	add_code( SDLK_LSHIFT,      KEY_LSHIFT);
	add_code( SDLK_RCTRL,       KEY_RCTRL);
	add_code( SDLK_LCTRL,       KEY_LCTRL);
	add_code( SDLK_RALT,        KEY_RALT);
	add_code( SDLK_LALT,        KEY_LALT);
	add_code( SDLK_RMETA,       KEY_RMETA);
	add_code( SDLK_LMETA,       KEY_LMETA);
	add_code( SDLK_LSUPER,      KEY_LSUPER);
	add_code( SDLK_RSUPER,      KEY_RSUPER);
	add_code( SDLK_MODE,        KEY_MODE);
	add_code( SDLK_COMPOSE,     KEY_COMPOSE);
	add_code( SDLK_HELP,        KEY_HELP);
	add_code( SDLK_PRINT,       KEY_PRINT);
	add_code( SDLK_SYSREQ,      KEY_SYSREQ);
	add_code( SDLK_BREAK,       KEY_BREAK);
	add_code( SDLK_MENU,        KEY_MENU);
	add_code( SDLK_POWER,       KEY_POWER);
	add_code( SDLK_EURO,        KEY_EURO);
	add_code( SDLK_UNDO,        KEY_UNDO);

}


KeycodesSDL::~KeycodesSDL()
{
}


}

#endif
