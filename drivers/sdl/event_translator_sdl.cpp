//
// C++ Implementation: event_translator_sdl
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "event_translator_sdl.h"

#ifdef SDL_ENABLED

#define SDL_JOY_MAX 32768
#include "os/memory_pool_static.h"

const unsigned int EventTranslator_SDL::sdl_key_table[KEY_MAX]={
	
	SDLK_UNKNOWN,
	SDLK_FIRST,
	SDLK_BACKSPACE,
	SDLK_TAB,
	SDLK_CLEAR,
	SDLK_RETURN,
	SDLK_PAUSE,
	SDLK_ESCAPE,
	SDLK_SPACE,
	SDLK_EXCLAIM,
	SDLK_QUOTEDBL,
	SDLK_HASH,
	SDLK_DOLLAR,
	SDLK_AMPERSAND,
	SDLK_QUOTE,
	SDLK_LEFTPAREN,
	SDLK_RIGHTPAREN,
	SDLK_ASTERISK,
	SDLK_PLUS,
	SDLK_COMMA,
	SDLK_MINUS,
	SDLK_PERIOD,
	SDLK_SLASH,
	SDLK_0,
	SDLK_1,
	SDLK_2,
	SDLK_3,
	SDLK_4,
	SDLK_5,
	SDLK_6,
	SDLK_7,
	SDLK_8,
	SDLK_9,
	SDLK_COLON,
	SDLK_SEMICOLON,
	SDLK_LESS,
	SDLK_EQUALS,
	SDLK_GREATER,
	SDLK_QUESTION,
	SDLK_AT,
	SDLK_LEFTBRACKET,
	SDLK_BACKSLASH,
	SDLK_RIGHTBRACKET,
	SDLK_CARET,
	SDLK_UNDERSCORE,
	SDLK_BACKQUOTE,
	SDLK_a,
	SDLK_b,
	SDLK_c,
	SDLK_d,
	SDLK_e,
	SDLK_f,
	SDLK_g,
	SDLK_h,
	SDLK_i,
	SDLK_j,
	SDLK_k,
	SDLK_l,
	SDLK_m,
	SDLK_n,
	SDLK_o,
	SDLK_p,
	SDLK_q,
	SDLK_r,
	SDLK_s,
	SDLK_t,
	SDLK_u,
	SDLK_v,
	SDLK_w,
	SDLK_x,
	SDLK_y,
	SDLK_z,
	SDLK_DELETE,
	SDLK_WORLD_0,
	SDLK_WORLD_1,
	SDLK_WORLD_2,
	SDLK_WORLD_3,
	SDLK_WORLD_4,
	SDLK_WORLD_5,
	SDLK_WORLD_6,
	SDLK_WORLD_7,
	SDLK_WORLD_8,
	SDLK_WORLD_9,
	SDLK_WORLD_10,
	SDLK_WORLD_11,
	SDLK_WORLD_12,
	SDLK_WORLD_13,
	SDLK_WORLD_14,
	SDLK_WORLD_15,
	SDLK_WORLD_16,
	SDLK_WORLD_17,
	SDLK_WORLD_18,
	SDLK_WORLD_19,
	SDLK_WORLD_20,
	SDLK_WORLD_21,
	SDLK_WORLD_22,
	SDLK_WORLD_23,
	SDLK_WORLD_24,
	SDLK_WORLD_25,
	SDLK_WORLD_26,
	SDLK_WORLD_27,
	SDLK_WORLD_28,
	SDLK_WORLD_29,
	SDLK_WORLD_30,
	SDLK_WORLD_31,
	SDLK_WORLD_32,
	SDLK_WORLD_33,
	SDLK_WORLD_34,
	SDLK_WORLD_35,
	SDLK_WORLD_36,
	SDLK_WORLD_37,
	SDLK_WORLD_38,
	SDLK_WORLD_39,
	SDLK_WORLD_40,
	SDLK_WORLD_41,
	SDLK_WORLD_42,
	SDLK_WORLD_43,
	SDLK_WORLD_44,
	SDLK_WORLD_45,
	SDLK_WORLD_46,
	SDLK_WORLD_47,
	SDLK_WORLD_48,
	SDLK_WORLD_49,
	SDLK_WORLD_50,
	SDLK_WORLD_51,
	SDLK_WORLD_52,
	SDLK_WORLD_53,
	SDLK_WORLD_54,
	SDLK_WORLD_55,
	SDLK_WORLD_56,
	SDLK_WORLD_57,
	SDLK_WORLD_58,
	SDLK_WORLD_59,
	SDLK_WORLD_60,
	SDLK_WORLD_61,
	SDLK_WORLD_62,
	SDLK_WORLD_63,
	SDLK_WORLD_64,
	SDLK_WORLD_65,
	SDLK_WORLD_66,
	SDLK_WORLD_67,
	SDLK_WORLD_68,
	SDLK_WORLD_69,
	SDLK_WORLD_70,
	SDLK_WORLD_71,
	SDLK_WORLD_72,
	SDLK_WORLD_73,
	SDLK_WORLD_74,
	SDLK_WORLD_75,
	SDLK_WORLD_76,
	SDLK_WORLD_77,
	SDLK_WORLD_78,
	SDLK_WORLD_79,
	SDLK_WORLD_80,
	SDLK_WORLD_81,
	SDLK_WORLD_82,
	SDLK_WORLD_83,
	SDLK_WORLD_84,
	SDLK_WORLD_85,
	SDLK_WORLD_86,
	SDLK_WORLD_87,
	SDLK_WORLD_88,
	SDLK_WORLD_89,
	SDLK_WORLD_90,
	SDLK_WORLD_91,
	SDLK_WORLD_92,
	SDLK_WORLD_93,
	SDLK_WORLD_94,
	SDLK_WORLD_95,
	SDLK_KP0,
	SDLK_KP1,
	SDLK_KP2,
	SDLK_KP3,
	SDLK_KP4,
	SDLK_KP5,
	SDLK_KP6,
	SDLK_KP7,
	SDLK_KP8,
	SDLK_KP9,
	SDLK_KP_PERIOD,
	SDLK_KP_DIVIDE,
	SDLK_KP_MULTIPLY,
	SDLK_KP_MINUS,
	SDLK_KP_PLUS,
	SDLK_KP_ENTER,
	SDLK_KP_EQUALS,
	SDLK_UP,
	SDLK_DOWN,
	SDLK_RIGHT,
	SDLK_LEFT,
	SDLK_INSERT,
	SDLK_HOME,
	SDLK_END,
	SDLK_PAGEUP,
	SDLK_PAGEDOWN,
	SDLK_F1,
	SDLK_F2,
	SDLK_F3,
	SDLK_F4,
	SDLK_F5,
	SDLK_F6,
	SDLK_F7,
	SDLK_F8,
	SDLK_F9,
	SDLK_F10,
	SDLK_F11,
	SDLK_F12,
	SDLK_F13,
	SDLK_F14,
	SDLK_F15,
	SDLK_NUMLOCK,
	SDLK_CAPSLOCK,
	SDLK_SCROLLOCK,
	SDLK_RSHIFT,
	SDLK_LSHIFT,
	SDLK_RCTRL,
	SDLK_LCTRL,
	SDLK_RALT,
	SDLK_LALT,
	SDLK_RMETA,
	SDLK_LMETA,
	SDLK_LSUPER,
	SDLK_RSUPER,
	SDLK_MODE,
	SDLK_COMPOSE,
	SDLK_HELP,
	SDLK_PRINT,
	SDLK_SYSREQ,
	SDLK_BREAK,
	SDLK_MENU,
	SDLK_POWER,
	SDLK_EURO,
	SDLK_UNDO
	
};

bool EventTranslator_SDL::translate_event(const SDL_Event& p_sdl_event, InputEvent& p_ievent) {

	p_ievent.type=InputEvent::NONE; //no p_sdl_event by default
	p_ievent.device_index = 0;

	switch(p_sdl_event.type) {
		case SDL_KEYUP:
		case SDL_KEYDOWN: {
				
			p_ievent.type=InputEvent::KEY;
			InputEventKey &k=p_ievent.key;

			k.mod.shift=(  p_sdl_event.key.keysym.mod & (KMOD_LSHIFT|KMOD_RSHIFT));
			k.mod.alt=(  p_sdl_event.key.keysym.mod & (KMOD_LALT|KMOD_RALT));
			k.mod.control=(  p_sdl_event.key.keysym.mod & (KMOD_LCTRL|KMOD_RCTRL));
			k.mod.meta=(  p_sdl_event.key.keysym.mod & (KMOD_LMETA|KMOD_RMETA));

			int scancode=-1;
			for (int i=0;i<KEY_MAX;i++) {
				
				if (sdl_key_table[i]==p_sdl_event.key.keysym.sym) {
					
					scancode=i;
				}
			}
			if (scancode==-1)
				break;
				
			if (p_sdl_event.type == SDL_KEYUP) {
				last_key_pressed = -1;
			} else {

				if (p_sdl_event.key.keysym.unicode) {
					if (last_key_pressed == scancode) {

						k.echo = true;
					};
					last_key_pressed = scancode;
				};
			};
			
				
			k.scancode=scancode;
			k.unicode=p_sdl_event.key.keysym.unicode;
			k.pressed=p_sdl_event.key.state==SDL_PRESSED;
		} break;

		case SDL_MOUSEBUTTONUP:
		case SDL_MOUSEBUTTONDOWN: {

			p_ievent.type=InputEvent::MOUSE_BUTTON;
			InputEventMouseButton &b=p_ievent.mouse_button;

			Uint32 last_click_delta=0;

			if (p_sdl_event.button.type==SDL_MOUSEBUTTONDOWN && p_sdl_event.button.button==SDL_BUTTON_LEFT) {

				static Uint32 last_click_tick=SDL_GetTicks();
					
				last_click_delta=SDL_GetTicks()-last_click_tick;
				last_click_tick=SDL_GetTicks();
			}

			SDLMod	mod_state=SDL_GetModState();


			b.mod.shift=(  mod_state & (KMOD_LSHIFT|KMOD_RSHIFT));
			b.mod.alt=(  mod_state & (KMOD_LALT|KMOD_RALT));
			b.mod.control=(  mod_state & (KMOD_LCTRL|KMOD_RCTRL));
			b.mod.meta=(  mod_state & (KMOD_LMETA|KMOD_RMETA));

			b.button_index=p_sdl_event.button.button;
			b.pressed=p_sdl_event.button.type==SDL_MOUSEBUTTONDOWN;
			b.x=p_sdl_event.button.x;
			b.y=p_sdl_event.button.y;
			b.global_x=b.global_y=0; // ???


			if (/*can_dblclick &&*/ last_click_delta<250 && p_sdl_event.button.type==SDL_MOUSEBUTTONDOWN && p_sdl_event.button.button==SDL_BUTTON_LEFT) {
				b.doubleclick=true;

			} else
				b.doubleclick=false;


		} break;

		case SDL_MOUSEMOTION: {

			p_ievent.type=InputEvent::MOUSE_MOTION;
			InputEventMouseMotion &m=p_ievent.mouse_motion;
			p_ievent.device_index = 0;
			m.pointer_index = 0;

			/* Motion compensation, in case there are MANY motion events pending */
			SDL_Event compensator;
			SDL_Event mevent=p_sdl_event;

			while (SDL_PeepEvents(&compensator,1,SDL_GETEVENT,SDL_EVENTMASK(SDL_MOUSEMOTION))>0) {

				mevent.motion.xrel+=compensator.motion.xrel;
				mevent.motion.yrel+=compensator.motion.yrel;
				mevent.motion.state=compensator.motion.state;
				mevent.motion.x=compensator.motion.x;
				mevent.motion.y=compensator.motion.y;
			}

			SDLMod	mod_state=SDL_GetModState();


			m.mod.shift=(  mod_state & (KMOD_LSHIFT|KMOD_RSHIFT));
			m.mod.alt=(  mod_state & (KMOD_LALT|KMOD_RALT));
			m.mod.control=(  mod_state & (KMOD_LCTRL|KMOD_RCTRL));
			m.mod.meta=(  mod_state & (KMOD_LMETA|KMOD_RMETA));

			m.x=mevent.motion.x;
			m.y=mevent.motion.y;
			m.relative_x=mevent.motion.xrel;
			m.relative_y=mevent.motion.yrel;
			m.button_mask=mevent.motion.state;
		} break;

		case SDL_JOYAXISMOTION: {

			p_ievent.type = InputEvent::JOYSTICK_MOTION;
			p_ievent.device_index = p_sdl_event.jaxis.which;
			InputEventJoystickMotion &j = p_ievent.joy_motion;
			j.axis = p_sdl_event.jaxis.axis;
				
			int value = p_sdl_event.jaxis.value;
			j.axis_value = (int)(value * ((float)JOY_AXIS_MAX / (float)SDL_JOY_MAX));
					
		} break;
			
		case SDL_JOYBUTTONUP:
		case SDL_JOYBUTTONDOWN: {

			p_ievent.type = InputEvent::JOYSTICK_BUTTON;
			p_ievent.device_index = p_sdl_event.jbutton.which;
			InputEventJoystickButton &j = p_ievent.joy_button;
			j.button_index = p_sdl_event.jbutton.button;
			j.pressed = p_sdl_event.type == SDL_JOYBUTTONDOWN?true:false;
		} break;
		default: { 
						
			return false;
		};
	}
	
	return true;
}


EventTranslator_SDL::EventTranslator_SDL()
{

	last_key_pressed = -1;
}


EventTranslator_SDL::~EventTranslator_SDL()
{
}

#endif
