//
// C++ Interface: editor_iapi_icons
//
// Description: 
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef EDITOR_IAPI_ICONS_H
#define EDITOR_IAPI_ICONS_H


#include "base/skin.h"
#include "table.h"

/**
	@author ,,, <red@lunatea>
*/
class EditorIAPIIcons {

	Table<String,int> internal;
	Table<String,GUI::BitmapID> custom;	
	GUI::Skin *skin;
public:

		
	GUI::BitmapID get(String p_type);
	void add(String p_type,GUI::BitmapID p_bitmap);
	EditorIAPIIcons(GUI::Skin * p_skin);
	~EditorIAPIIcons();

};

#endif
