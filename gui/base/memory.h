//
// C++ Interface: memory
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//


#ifndef GUI_EXTERNAL_MEMORY

namespace GUI {

#define GUI_NEW( m_type ) new m_type
#define GUI_DELETE( m_type ) delete m_type

#define GUI_NEW_ARRAY( m_type, m_count ) new m_type[m_count]
#define GUI_DELETE_ARRAY( m_type ) delete[] m_type

};

#else

#include GUI_EXTERNAL_MEMORY

#endif

