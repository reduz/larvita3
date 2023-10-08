#ifdef SDL_ENABLED
//
// C++ Interface: keycodes_sdl
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef PIGUIKEYCODES_SDL_H
#define PIGUIKEYCODES_SDL_H
#include "base/keyboard.h"

namespace GUI {

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class KeycodesSDL : public KeycodeTranslator {
public:
    KeycodesSDL();

    ~KeycodesSDL();

};

}

#endif

#endif
