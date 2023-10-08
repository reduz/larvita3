//
// C++ Interface: desktop
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef PIGUIDESKTOP_H
#define PIGUIDESKTOP_H

namespace Pigui {

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class Desktop{
public:
	
	virtual Painter* register_window(Window *p_window);
	
	virtual void update_window(Window * p_window)=0;
	virtual void update_window(Window * p_window, const Rect &p_area)=0;
	
	Desktop();
	~Desktop();

};

}

#endif
