
#ifndef TYPEDEFS_H
#define TYPEDEFS_H

/**
 * Basic definitions and simple functions to be used everywhere..
 */

#ifndef NDS
#include <stddef.h>
#else
typedef unsigned int size_t;
#endif

#ifdef NDS_ENABLED
 #include <nds.h>
 #ifdef DEBUG_SUB
  extern int tcp_printf(const char* format, ...);
  //#define printf tcp_printf
 #endif
#else
 #define iprintf printf
#endif



#if defined(__GNUC__) && (__GNUC__ >= 4 )
#    define _FORCE_INLINE_ __attribute__((always_inline)) inline
#elif defined(_MSVC)
#	define _FORCE_INLINE_ __forceinline
#else
#    define _FORCE_INLINE_ inline
#endif


#include "error_macros.h"
#include "error_list.h"


/**
 * Some platforms (devices) not define NULL
 */

#ifndef NULL
#define NULL 0
#endif

/**
 * Windows defines a lot of badly stuff we'll never ever use. undefine it.
 */

#ifdef _WIN32
#	undef min // override standard definition
#	undef max // override standard definition
#	undef ERROR // override (really stupid) wingdi.h standard definition
#	undef DELETE // override (another really stupid) winnt.h standard definition
#	undef MessageBox // override winuser.h standard definition
#endif

/**
 * Types defined for portability.
 * libSDL uses the same convention, so if libSDL is in use, we just use SDL ones.
 */
#ifdef SDL_ENABLED
#include <SDL/SDL.h>
#else
typedef unsigned char   Uint8;
typedef signed char     Sint8;
typedef unsigned short  Uint16;
typedef signed short    Sint16;
typedef unsigned int    Uint32;
typedef signed int      Sint32;
typedef long long	Sint64;
typedef unsigned long long Uint64;
#endif

/* <windows_sucks> */
#ifdef WINDOWS_ENABLED
/**
 * Round 2.0, newer microsoft compilers make this not work again.
 * SO WE BE MORE DRASTIC :( BAD MICROSOFT!
 */

#ifdef min
#undef min
#else
#define min min
#endif

#ifdef max
#undef max
#else
#define max max
#endif

#ifdef OK
#undef OK
#else
#define OK OK
#endif

#ifdef ERROR
#undef ERROR
#else
#define ERROR ERROR
#endif

#ifdef Error
#undef Error
#else
#define Error Error
#endif

#endif

/* </windows_sucks> */

/** Generic ABS function, that works for both floating/fixed point */
#ifndef ABS
template<class T>
static inline T ABS(const T& v) { return (v<0)?-v:v; }
#endif

#ifndef MIN
#define MIN(m_a,m_b) (((m_a)<(m_b))?(m_a):(m_b))
#endif

#ifndef MAX
#define MAX(m_a,m_b) (((m_a)>(m_b))?(m_a):(m_b))
#endif

/** Generic swap template */
#ifndef SWAP

#define SWAP(m_x,m_y) __swap_tmpl(m_x,m_y)
template<class T>
inline void __swap_tmpl(T &x, T &y ) {

	T aux=x;
	x=y;
	y=aux;
}

#endif //swap

/** Macros used by tolua for specialized scripting
 * (when the whoel file can't or must not be parsed.
 * Used little, but used in some places.
 * For more info, read the tolua++ docs.
 */

// for scripting
#define SCRIPT_BIND_BEGIN
#define SCRIPT_BIND_END
#define SCRIPT_EXPORT
#define SCRIPT_BIND_CLASS
#define SCRIPT_BIND_BLOCK

/**
 * tolua macro for binding templates.
 * Just write the template inside like
 * TEMPLATE_BIND( std::vector< Burrito >
 * and the template will be usable by lua.
 * For more info, read the tolua++ docs.
 */

#ifndef TEMPLATE_BIND
#define 	TEMPLATE_BIND(x)
#endif

#define tolua_readonly


/** Own Operator New for custom memory placement **/
void *operator new(size_t p_bytes,void *p_alloc_pos,size_t p_alloc_check);


/** Generic color type */
struct Color {

	Uint8 r,g,b,a; ///< ranges 0.0 to 255

	bool operator==(const Color &p_color) const { return (r==p_color.r && g==p_color.g && b==p_color.b && a==p_color.a ); }
	bool operator!=(const Color &p_color) const { return (r!=p_color.r || g!=p_color.g || b!=p_color.b || a!=p_color.a ); }

	Uint32 to_32() const {

		Uint32 c=a;
		c<<=8;
		c|=r;
		c<<=8;
		c|=g;
		c<<=8;
		c|=b;

		return c;
	}

	void from_32(Uint32 p_col) {
	
		b=p_col&0xFF;
		g=(p_col>>8)&0xFF;
		r=(p_col>>16)&0xFF;
		a=(p_col>>24)&0xFF;
	}

	Uint8 gray() const {

		return (Uint8)((Uint16)r + (Uint16)g + (Uint16)b)/3;
	}
	/**
	 * No construct parameters, r=0, g=0, b=0. a=255
	 */
	Color() {
		r=0; g=0; b=0; a=255;
	}
	/**
	 * Gray/Indexed construct parameters (for grayscale/indexed colors)
	 * r=byte g=byte b=byte a=255
	 */
	Color(Uint8 p_byte) {

		r=p_byte; g=p_byte; b=p_byte; a=255;
	}
	/**
	 * Gray+Alpha construct parameters
	 * r=gray g=gray b=gray a=alpha
	 */
	Color(Uint8 p_gray, Uint8 p_alpha) {

		r=p_gray; g=p_gray; b=p_gray; a=p_alpha;
	}

	/**
	 * RGB / RGBA construct parameters. Alpha is optional, but defaults to 255
	 */
	Color(Uint8 p_r,Uint8 p_g,Uint8 p_b,Uint8 p_a=255) { r=p_r; g=p_g; b=p_b; a=p_a; }
};

/** Function to find the nearest (bigger) power of 2 to an integer */

static inline unsigned int nearest_power_of_2(unsigned int p_number) {

	unsigned int i=31;
	
	while(true) {
		if (p_number&(1<<i))
			return ((p_number==(unsigned int)(1<<i)) ? p_number : (1<<(i+1)));
			
		if (i==0)
			return 0;
		else 
			i--;
	}

}

/** Foreach macro, ths is unportable to some compilers, so it's not used anymore... */

#define foreach(m_itname,m_container) \
        for( typeof(m_container.begin()) m_itname=m_container.begin() ; \
        m_itname!=m_container.end() ; \
        m_itname++ )

/** get a shift value from a power of 2 */
static inline int get_shift_from_power_of_2( unsigned int p_pixel ) {
	// return a GL_TEXTURE_SIZE_ENUM


	for (unsigned int i=0;i<32;i++) {

		if (p_pixel==(unsigned int)(1<<i))
			return i;
	}

	return -1;
}

/** Swap 32 bits value for endianness */
static inline Uint32 BSWAP32(Uint32 x) {
	return((x<<24)|((x<<8)&0x00FF0000)|((x>>8)&0x0000FF00)|(x>>24));
}

/** When compiling with RTTI, we can add an "extra"
 * layer of safeness in many operations, so dynamic_cast
 * is used besides casting by enum.
 */



#ifdef NO_SAFE_CAST

#define SAFE_CAST static_cast

#else

#define SAFE_CAST dynamic_cast

#endif

#define MT_SAFE

#ifdef NDS
//#define printf(p...)
#endif

#define __STRX(m_index) #m_index
#define __STR(m_index) __STRX(m_index)


#endif  /* typedefs.h */

