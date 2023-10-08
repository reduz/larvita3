#include "rect3.h"

#ifndef AABB_H
#define AABB_H

/**
 * Just a typedef to Rect3
 */
typedef Rect3 AABB;

#define PRINT_AABB(m_text, m_aabb) printf("* AABB: %s\n*\t%f\t%f\n*\t%f\t%f\n\t%f\t%f\n",m_text,m_aabb.pos.x,m_aabb.size.x,m_aabb.pos.y,m_aabb.size.y,m_aabb.pos.z,m_aabb.size.z);

#endif
