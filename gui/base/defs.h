
#ifndef PIGUI_DEFS_H
#define PIGUI_DEFS_H


#include "string/rstring.h"
#include "base/keyboard.h"
#include "base/memory.h"

#include <stddef.h>

namespace GUI {


//#define PRINT_ERROR(m_err)


void __print_error(const char *p_file,int p_line,const char* p_error);
#define PRINT_ERROR(m_err) __print_error(__FILE__,__LINE__,m_err);

typedef int FontID;
typedef int BitmapID;
typedef int TimerID;

enum InvalidValues {
	
	INVALID_TIMER_ID=-1,
 	INVALID_BITMAP_ID=-1,
  	INVALID_FONT_ID=-1
};


enum MouseButton { 

	BUTTON_LEFT=1, ///< Left Mouse Button
	BUTTON_MIDDLE=2, ///< Middle Mouse Button
	BUTTON_RIGHT=3, ///< Right Mouse Button
	BUTTON_WHEEL_UP=4, ///< Mouse wheel UP
	BUTTON_WHEEL_DOWN=5, ///< Mouse wheel DOWN
};

enum MouseButtonMask {

	BUTTON_MASK_LEFT=(1<<(BUTTON_LEFT-1)),
	BUTTON_MASK_MIDDLE=(1<<(BUTTON_MIDDLE-1)),
	BUTTON_MASK_RIGHT=(1<<(BUTTON_RIGHT-1)),
};

enum Margin {
	
	MARGIN_LEFT, 
	MARGIN_TOP,	
	MARGIN_RIGHT,	
	MARGIN_BOTTOM,
	
};
	

enum Direction {
	
	UP,
	LEFT,
	DOWN,
	RIGHT
};

enum FocusMode {
	FOCUS_NONE, ///< Frame can't be focused in any way
	FOCUS_CLICK, ///< This type of focus means the frame will gain focus only when it's clicked
	FOCUS_ALL ///< This focus is like FOCUS_CLICK, but also adds posibility of keyboard focus.
};

enum Orientation {

	VERTICAL,
	HORIZONTAL
};

enum BitmapMode {

	MODE_ALPHA_MASK, ///< Alpha pixmap, blit with a color, using alpha as a mask
	MODE_PIXMAP, ///< RGB pixmap
};

//needing to include math.h/cmath for an integer/float abs function is asking too much
#ifndef ABS
#define ABS( m_val ) ( ( (m_val) < 0 ) ? -(m_val) : (m_val) )
#endif

#ifndef MIN
#define MIN(m_a,m_b) (((m_a)<(m_b))?(m_a):(m_b))
#define MAX(m_a,m_b) (((m_a)>(m_b))?(m_a):(m_b))
#endif

#ifndef SGN
#define SGN(x) ((x)>0 ? 1 : ((x)==0 ? 0:(-1)))
#endif

#ifdef _MSVC

typedef unsigned __int64 gui_u64;

#else

typedef unsigned long long gui_u64;

#endif

} //end of namespace



#endif
