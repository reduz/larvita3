//
// C++ Implementation: variant
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "variant.h"
#include "marshalls.h"
#include "types/error_macros.h"
#include "iapi/iapi.h"
#include "fileio/serializer_size_compute.h"
#include "fileio/serializer_buffer.h"
#include "fileio/deserializer_buffer.h"
#include "iapi_persist.h"
#include "stdio.h"

Variant::Type Variant::get_type() const {
	
	return _type;
}

int Variant::size() const {
	
	switch(_type) {
		
		case INT_ARRAY: {
			
			return reinterpret_cast<DVector<int>*>( _t.tother )->size();
		} break;
		case REAL_ARRAY: {
			
			return reinterpret_cast<DVector<double>*>( _t.tother )->size();
		
		} break;
		case STRING_ARRAY: {
			
			return reinterpret_cast<DVector<String>*>( _t.tother )->size();
			
		} break;
		case BYTE_ARRAY: {
			
			return reinterpret_cast<DVector<Uint8>*>( _t.tother )->size();
		} break;
		
		/* Variant */
		case VARIANT_ARRAY: {
			
			return reinterpret_cast<DVector<Variant>*>( _t.tother )->size();
			
		} break;
		default: {
			
			return 0;
		}
	}
	
	return 0;
}
		
/* constructors */

Variant::Variant(bool p_v) {
	
	init();
	set_bool(p_v);
}

Variant::Variant(Sint8 p_v) {
	
	init();
	set_int(p_v);
	
}
Variant::Variant(Uint8 p_v) {
	
	init();
	set_int(p_v);
	
}

Variant::Variant(Sint16 p_v) {
	
	init();
	set_int(p_v);
	
}
Variant::Variant(Uint16 p_v) {
	
	init();
	set_int((Uint32)p_v);
	
}
Variant::Variant(Sint32 p_v) {
	
	init();
	set_int(p_v);
	
}
Variant::Variant(Uint32 p_v) {
	
	init();
	set_int(p_v);
	
}
Variant::Variant(Sint64 p_v) {
	
	init();
	if (p_v>0x7FFFFFFF || p_v < -0x80000000) {
		
		ERR_PRINT("Value too large!");
	}
	
	set_int(p_v);
	
}
Variant::Variant(Uint64 p_v) {
	
	init();
	set_int(p_v);
	if (p_v>0x7FFFFFFF) {
		
		ERR_PRINT("Value too large!");
	}
	
	set_int(p_v);
	
	
}

Variant::Variant(float p_v) {
	
	init();
	set_real(p_v);
	
}
Variant::Variant(double p_v) {
	
	init();
	set_real(p_v);
	
}
Variant::Variant(const char *p_cstr) {
	
	init();
	set_string(p_cstr);	
}

Variant::Variant(String::CharType *p_wcstr) {
		
	init();
	set_string(p_wcstr);	
	
}

Variant::Variant(String p_v) {
	
	init();
	set_string(p_v);
	
}
Variant::Variant(Type p_type,int p_len) {
	
	init();
	create(p_type,p_len);
	
}


void Variant::set_variant(const Variant& p_variant) {


	/* copy a variant */	

	if (this == &p_variant)
		return; /* don't even bother */
	
	clear();
		
	switch(p_variant._type) {
		
		case NIL: {
			
			_type = NIL;
			
		} break;
		
		case BOOL: {
			
			set_bool( p_variant.get_bool() );
		} break;
		case INT: {
			
			set_int( p_variant.get_int() );
			_type=p_variant._type;
		
		} break;
  		case REAL: {
			
			set_real( p_variant.get_real() );
			
		} break;
		case STRING: {
			
			set_string( p_variant.get_string() );
		} break;
		/* Extra types (allocate internally */
		
// macro to make things easier, takes advantage of the copy on write in the arrays
#define COPY_ARR( m_type )\
	DVector<m_type> *new_arr = memnew( DVector<m_type> );\
	*new_arr=*reinterpret_cast<const DVector<m_type>*>( p_variant._t.tother );\
	_t.tother=reinterpret_cast<void*>( new_arr );
		
		case INT_ARRAY: {
			
			COPY_ARR( int );
			_type = INT_ARRAY;

		} break;
		case REAL_ARRAY: {
			
			COPY_ARR( double );
			_type = REAL_ARRAY;
			
		} break;
		case STRING_ARRAY: {
			
			COPY_ARR( String );
			_type = STRING_ARRAY;
			
		} break;
		case BYTE_ARRAY: {
			
			COPY_ARR( Uint8 );
			_type = BYTE_ARRAY;
			
		} break;
		case VARIANT_ARRAY: {
			
			COPY_ARR( Variant );
			_type = VARIANT_ARRAY;
			
		} break;
		/* Math */
		case VECTOR3: {
			
			set_vector3( p_variant.get_vector3() );
			
		} break;
		case MATRIX4: {
			
			set_matrix4( p_variant.get_matrix4() );
			
		} break;
		case PLANE: {
			
			set_plane( p_variant.get_plane() );
			
		} break;
		case RECT3: {
			
			set_rect3( p_variant.get_rect3() );
			
		} break;
		case QUAT: {
			
			set_quat( p_variant.get_quat() );
			
		} break;
		case COLOR: {
			
			set_color( p_variant.get_color() );
		} break;
		case IMAGE: {
			
			set_image( p_variant.get_image() );
		} break;
		case IOBJ: {
			
			set_IOBJ( p_variant.get_IOBJ() );
		} break;
		
		default: {
			ERR_PRINT("BUG: Forgot something?"); clear();
		}
	}
	
}


Variant::Variant(const Variant& p_v) {
	
	init();
	set_variant(p_v);
	
}


Variant::Variant(const Vector3& p_v) {
	
	init();
	set_vector3(p_v);
	
}
Variant::Variant(const Matrix4& p_v) {
	
	init();
	set_matrix4(p_v);
	
}
Variant::Variant(const Plane& p_v) {
	
	init();
	set_plane(p_v);
	
}
Variant::Variant(const Rect3& p_v) {
	
	init();
	set_rect3(p_v);
	
}
Variant::Variant(const Quat& p_v) {
	
	init();
	set_quat(p_v);
	
}

Variant::Variant(const Color& p_v) {
	
	init();
	set_color(p_v);
	
}

Variant::Variant(const Image& p_v) {
	
	init();
	set_image(p_v);
	
}

Variant::Variant(const IObjRef& p_v) {
	
	init();
	set_IOBJ(p_v);
	
}

/* operator = */


void Variant::operator=(const Variant& p_v)  {
	
	set_variant(p_v);
	
}

/* logic/math operators */
/* 

bool Variant::operator!=(const Variant& p_variant) const {
	
}
Variant Variant::operator+(const Variant& p_variant) const {
	
}
Variant Variant::operator-(const Variant& p_variant) const {
	
}
Variant Variant::operator*(const Variant& p_variant) const {
	
}
Variant Variant::operator/(const Variant& p_variant) const {
	
}

void Variant::operator+=(const Variant& p_variant) {
	
}
void Variant::operator-=(const Variant& p_variant) {
	
}
void Variant::operator*=(const Variant& p_variant) {
	
}
void Variant::operator/=(const Variant& p_variant) {
	
}
*/
/* gets, conversion will take place if possible, otherwise not. if index is passed, array is assumed. if a type not of array has an index, variant array is assumed. */

bool Variant::get_bool() const {
	
	switch(_type) {

		case NIL: {
			return false;
		} break;
		
		case BOOL: {
			
			return _t.tbool;
		} break;
		case INT: {
			
			return _t.tint!=0;
		} break;
		case REAL: {
			
			return _t.treal!=0.0;			
		} break;
		default: {}
	}
		
	ERR_PRINT("Incompatible Types");
	return false;

}

Sint32 Variant::get_int() const {
	
	switch(_type) {
		
		case NIL: {
			
			return 0;
		} break;
		case BOOL: {
			
			return _t.tbool?1:0;
		} break;
		case INT: {
			
			return _t.tint;
		} break;
		case REAL: {
			
			return (int)_t.treal;
		} break;
		default: {}
	}
		
	ERR_PRINT("Incompatible Types");
	return 0;

}


double Variant::get_real() const {
	
	switch(_type) {
		
		case BOOL: {
			
			return _t.tbool?1.0:0.0;
		} break;
		case INT: {
			
			return (double)_t.tint;
		} break;
		case REAL: {
			
			return _t.treal;
		} break;
		default: {}
	}
		
	ERR_PRINT("Incompatible Types");
	return 0;
	
}
String Variant::get_string() const {
	

	switch(_type) {
	
		case NIL: {
				return "<nil>";
		} break;
		
		case BOOL: {
			
			return _t.tbool?"true":"false";
		} break;
		case INT: {
			
			return String::num(_t.tint,0);
		} break;
		case REAL: {
			
			return String::num(_t.treal);
		} break;
		case STRING: {
			
			return _tstr;
		} break;
		case INT_ARRAY: {
			
			String intarray="(";
			for (int i=0;i<size();i++) {
				
				if (i!=0)
					intarray+=", ";
				intarray+=String::num( array_get_int(i) );
			}
			return intarray+")";
		} break;
		case REAL_ARRAY: {
			
			String realarray="(";
			for (int i=0;i<size();i++) {
				
				if (i!=0)
					realarray+=", ";
				realarray+=String::num( array_get_real(i) );
			}
			return realarray+")";
		} break;
		case BYTE_ARRAY: {
			
			String bytearray="(";
			for (int i=0;i<size();i++) {
				
				if (i!=0)
					bytearray+=", ";
				bytearray+=String::num( array_get_byte(i) );
			}
			return bytearray+")";
		} break;
		case STRING_ARRAY: {
		
			String strarray = "{";
			for (int i=0; i<size(); i++) {
				if (i)
					strarray+=", ";
				strarray+=array_get_string(i);
			};
			return strarray+")";
		} break;
		case VARIANT_ARRAY: {
			
			String variantarray="(";
			for (int i=0;i<size();i++) {
				
				if (i!=0)
					variantarray+=", ";
				variantarray+=array_get(i).get_string();
			}
			return variantarray+")";
		} break;
		case VECTOR3: {
			
			Vector3 v=get_vector3();
			
			return String::num(v.get_x()) + ", " + String::num(v.get_y())+", "+String::num(v.get_z());
		} break;
		case MATRIX4: {
			
			Matrix4 m=get_matrix4();
			
			String r;
			
			for (int i=0;i<16;i++) {
				
				if (i!=0)
					r+=", ";
				
				r+=String::num( m.get( i % 4, i / 4 ) );
				
			}
			
			return r;
			
		} break;
		case PLANE: {
			
			Plane p = get_plane();
			return String::num(p.normal.x) + ", " + String::num(p.normal.y)+", "+String::num(p.normal.z)+", "+String::num(p.d);
			
			
		} break;
		case RECT3: {
			
			Rect3 r = get_rect3();
			return String::num(r.pos.x) + ", " + String::num(r.pos.y) + ", " + String::num(r.pos.z) + ":" + String::num(r.size.x) + ", " + String::num(r.size.y) + ", " + String::num(r.size.z);
		} break;
		case QUAT: {
			
			Quat q = get_quat();
			return String::num(q.get_x()) + ", " + String::num(q.get_y())+", "+String::num(q.get_z())+", "+String::num(q.get_w());
			
		} break;
		
		case COLOR: {
			
			Color c = get_color();
			return String::num(c.r) + ", " + String::num(c.g)+", "+String::num(c.b)+", "+String::num(c.a);
			
		} break;
		
		/* Data */
		
		case IOBJ: {
			
			//RID rid = get_RID();
			
			//return String::num(rid.get_asset_ID());
			//return "IOBJ";
			IObjRef *io = reinterpret_cast<IObjRef*>( _t.tother );
			if (!(*io)) return "<null IObj>";
			return (*io)->get_type()+":"+String::num((*io)->get_ID());

			
		} break;
		
		case IMAGE: {
			Image* img = reinterpret_cast<Image*>( _t.tother );
			return String("Image: ")+String::num(img->get_width())+"x"+String::num(img->get_height())+"x"+String::num(img->get_format());
		} break;
		
		default: {}
	}
	
	ERR_PRINT("Incompatible Types");
	return "";
	
}


DVector<Uint8> Variant::get_byte_array() const {

	ERR_FAIL_COND_V(_type!=BYTE_ARRAY,DVector<Uint8>());
	return *reinterpret_cast<DVector<Uint8>*>( _t.tother );	
}
DVector<String> Variant::get_string_array() const {

	ERR_FAIL_COND_V(_type!=STRING_ARRAY,DVector<String>());
	return *reinterpret_cast<DVector<String>*>( _t.tother );	

}
DVector<int> Variant::get_int_array() const {

	ERR_FAIL_COND_V(_type!=INT_ARRAY,DVector<int>());
	return *reinterpret_cast<DVector<int>*>( _t.tother );	

}
DVector<double> Variant::get_real_array() const {

	ERR_FAIL_COND_V(_type!=REAL_ARRAY,DVector<double>());
	return *reinterpret_cast<DVector<double>*>( _t.tother );	

}
DVector<Variant> Variant::get_variant_array() const {

	ERR_FAIL_COND_V(_type!=VARIANT_ARRAY,DVector<Variant>());
	return *reinterpret_cast<DVector<Variant>*>( _t.tother );	

}


Vector3 Variant::get_vector3() const {
	
	ERR_FAIL_COND_V( _type!=VECTOR3, Vector3() );	
	
	Vector3 *v = reinterpret_cast<Vector3*>( _t.tother );
	return *v;
}
Matrix4 Variant::get_matrix4() const {
	
	ERR_FAIL_COND_V( _type!=MATRIX4, Matrix4() );	
	
	Matrix4 *m = reinterpret_cast<Matrix4*>( _t.tother );
	return *m;
	
}
Plane Variant::get_plane() const {
	
	ERR_FAIL_COND_V( _type!=PLANE, Plane() );	
	
	Plane *p = reinterpret_cast<Plane*>( _t.tother );
	return *p;
	
}
Rect3 Variant::get_rect3() const {
	
	ERR_FAIL_COND_V( _type!=RECT3, Rect3() );	
	
	Rect3 *r = reinterpret_cast<Rect3*>( _t.tother );
	return *r;
}

Quat Variant::get_quat() const {
	
	ERR_FAIL_COND_V( _type!=QUAT, Quat() );	
	
	Quat *q = reinterpret_cast<Quat*>( _t.tother );
	return *q;
	
}

Color Variant::get_color() const {
	
	ERR_FAIL_COND_V( _type!=COLOR, Color() );	
	
	Color *c = reinterpret_cast<Color*>( _t.tother );
	return *c;
	
}

Image Variant::get_image() const {
	
	ERR_FAIL_COND_V( _type!=IMAGE, Image() );	
	
	return *reinterpret_cast<Image*>( _t.tother );
}

IObjRef Variant::get_IOBJ() const {
	
	ERR_FAIL_COND_V( _type!=IOBJ, IObjRef() );	
	
	IObjRef *io = reinterpret_cast<IObjRef*>( _t.tother );
	return *io;
}


/** Internal, set */

void Variant::set_bool(bool p_v) {
	
	clear();
	_type=BOOL;
	_t.tbool=p_v;
	
}
void Variant::set_int(int p_v) {
	

	clear();
	_type=INT;
	_t.tint=p_v;
	
} 
void Variant::set_real(double p_v) {
	

	clear();
	_type=REAL;
	_t.treal=p_v;
	
}
void Variant::set_string(String p_v) {
	
	clear();
	_type=STRING;
	_tstr=p_v;
	
}

// will likely crash anyway, but checking is always good
#define _VARIANT_CHECK_OOM\
	if (!_t.tother) {\
		_type=NIL;\
		ERR_FAIL_COND( !_t.tother );\
	}

void Variant::set_vector3(const Vector3& p_v) {
	
	clear();
	_type=VECTOR3;
	
	
	_t.tother=reinterpret_cast<void*>( memnew( Vector3(p_v) ) );
	_VARIANT_CHECK_OOM
}
void Variant::set_matrix4(const Matrix4& p_v) {
	
	clear();
	_type=MATRIX4;
	
	_t.tother=reinterpret_cast<void*>( memnew( Matrix4(p_v) ) );
	_VARIANT_CHECK_OOM
	
}
void Variant::set_plane(const Plane& p_v) {
	
	clear();
	_type=PLANE;
	
	_t.tother=reinterpret_cast<void*>( memnew( Plane(p_v) ) );
	_VARIANT_CHECK_OOM
	
}
void Variant::set_rect3(const Rect3& p_v) {
	
	clear();
	_type=RECT3;
	
	_t.tother=reinterpret_cast<void*>( memnew( Rect3(p_v) ) );
	_VARIANT_CHECK_OOM
	
}
void Variant::set_quat(const Quat& p_v) {
	
	clear();
	_type=QUAT;
	
	_t.tother=reinterpret_cast<void*>( memnew( Quat(p_v) ) );
	_VARIANT_CHECK_OOM
	
}
void Variant::set_color(const Color& p_v) {
	
	clear();
	_type=COLOR;
	
	_t.tother=reinterpret_cast<void*>( memnew( Color(p_v) ) );
	_VARIANT_CHECK_OOM
	
}

void Variant::set_image(const Image& p_v) {
	
	clear();
	_type=IMAGE;
	
	_t.tother=reinterpret_cast<void*>( memnew( Image(p_v) ) );
	_VARIANT_CHECK_OOM
	
}

void Variant::set_IOBJ(const IObjRef& p_v) {
	
	clear();
	_type=IOBJ;
	
	_t.tother=reinterpret_cast<void*>( memnew( IObjRef(p_v) ) );
	_VARIANT_CHECK_OOM
	
}

/* specialized array set/get */
void Variant::array_set_int(int p_idx, int p_value) {
	
	ERR_FAIL_COND( _type != INT_ARRAY );
	
	reinterpret_cast<DVector<int>*>( _t.tother )->set(p_idx,p_value);
	
}

int Variant::array_get_int(int p_idx) const {
	
	ERR_FAIL_COND_V( _type != INT_ARRAY, 0 );
	
	return reinterpret_cast<DVector<int>*>( _t.tother )->get(p_idx);
	
}
void Variant::array_set_real(int p_idx, double p_value) {
	
	ERR_FAIL_COND( _type != REAL_ARRAY );
	
	reinterpret_cast<DVector<double>*>( _t.tother )->set(p_idx,p_value);
	
}
double Variant::array_get_real(int p_idx) const {
	
	ERR_FAIL_COND_V( _type != REAL_ARRAY, 0 );
	
	return reinterpret_cast<DVector<double>*>( _t.tother )->get(p_idx);
	
}
void Variant::array_set_string(int p_idx, String p_value) {
	
	ERR_FAIL_COND( _type != STRING_ARRAY );
	
	reinterpret_cast<DVector<String>*>( _t.tother )->set(p_idx,p_value);
	
}
String Variant::array_get_string(int p_idx) const {
	
	ERR_FAIL_COND_V( _type != STRING_ARRAY, "" );
	
	return reinterpret_cast<DVector<String>*>( _t.tother )->get(p_idx);
	
}


void Variant::set_byte_array(const DVector<Uint8>& p_array) {

	create(BYTE_ARRAY, p_array.size());
	
	reinterpret_cast<DVector<Uint8>*>( _t.tother )->operator=(p_array);
};

void Variant::set_string_array(const DVector<String>& p_array) {

	create(STRING_ARRAY, p_array.size());
	
	reinterpret_cast<DVector<String>*>( _t.tother )->operator=(p_array);
};

void Variant::set_int_array(const DVector<int>& p_array) {

	create(INT_ARRAY, p_array.size());
	
	reinterpret_cast<DVector<int>*>( _t.tother )->operator=(p_array);
};

void Variant::set_real_array(const DVector<double>& p_array) {

	create(REAL_ARRAY, p_array.size());
	
	reinterpret_cast<DVector<double>*>( _t.tother )->operator=(p_array);
};

void Variant::set_variant_array(const DVector<Variant>& p_array) {

	create(VARIANT_ARRAY, p_array.size());
	
	reinterpret_cast<DVector<Variant>*>( _t.tother )->operator=(p_array);
};

void Variant::array_set_byte(int p_idx, Uint8 p_value) {
	
	ERR_FAIL_COND( _type != BYTE_ARRAY );
	
	reinterpret_cast<DVector<Uint8>*>( _t.tother )->set(p_idx,p_value);
	
}
Uint8 Variant::array_get_byte(int p_idx) const {
	
	ERR_FAIL_COND_V( _type != BYTE_ARRAY, 0 );
	
	return reinterpret_cast<DVector<Uint8>*>( _t.tother )->get(p_idx);
	
}


/* general array set/get, for variants and supported types */
void Variant::array_set(int p_idx, const Variant& p_variant) {
	
	ERR_FAIL_COND( _type != VARIANT_ARRAY && _type != INT_ARRAY && _type != BYTE_ARRAY && _type != REAL_ARRAY  && _type != STRING_ARRAY );
	
	switch( _type ) {
		
		case INT_ARRAY: {
			
			array_set_int( p_idx, (int)p_variant );
		} break;
		case REAL_ARRAY: {
			
			array_set_real( p_idx, (double)p_variant );
			
		} break;
		case STRING_ARRAY: {
			
			array_set_string( p_idx, p_variant.get_string() );
			
		} break;
		case BYTE_ARRAY: {
			
			array_set_byte( p_idx, (int)p_variant );
			
		} break;
		case VARIANT_ARRAY: {
			
	
			reinterpret_cast<DVector<Variant>*>( _t.tother )->set(p_idx,p_variant);
		} break;
		default: { ERR_PRINT("Buggor"); }
	}

}
Variant Variant::array_get(int p_idx) const {
	
	ERR_FAIL_COND_V( _type != VARIANT_ARRAY && _type != INT_ARRAY && _type != BYTE_ARRAY && _type != REAL_ARRAY && _type != STRING_ARRAY, Variant() );
		
	switch( _type ) {
		
		case INT_ARRAY: {
			
			return array_get_int( p_idx );
		} break;
		case REAL_ARRAY: {
			
			return array_get_real( p_idx );
			
		} break;
		case STRING_ARRAY: {
			
			return array_get_string( p_idx );
			
		} break;
		case BYTE_ARRAY: {
			
			return array_get_byte( p_idx );
			
		} break;
		case VARIANT_ARRAY: {
			
			return reinterpret_cast<DVector<Variant>*>( _t.tother )->get(p_idx);
		} break;
		default: { ERR_PRINT("Buggor"); }
	}
	
	return Variant();	
}

Variant& Variant::operator[](int p_idx) {

	static Variant _static_variant;

	ERR_FAIL_COND_V(_type != VARIANT_ARRAY, _static_variant);
	
	Variant * varr = reinterpret_cast<Variant*>( _t.tother );
	ERR_FAIL_INDEX_V( p_idx, (int)memarr_len( varr ), _static_variant );

	return varr[p_idx];
};

Variant Variant::operator[](int p_idx) const {
	
	return array_get(p_idx);
};


void Variant::create(Type p_type,int p_len) {
	
	clear();
	
	switch (p_type) {
		case NIL: {
			
			_type=NIL;
			/* nothing to do */
		} break;
		
		/* Basic, built in types */
		case BOOL: {
			
			set_bool(false); /* false by default */
		} break;
		case INT: {
			
			set_int(0); /* zero by default */
		} break;
		case REAL: {
			
			set_real(0); /* zero by default */
			
		} break;
		case STRING: {
			
			set_string(""); /* empty by default */
		} break;
		
		/* Extra types (allocate internally */
		case INT_ARRAY: {
			
			
			ERR_FAIL_COND( p_len <= 0 );
			
			_type = INT_ARRAY;
			
			DVector<int> * arr = memnew( DVector<int> );
			arr->resize(p_len);
			for (int i=0;i<p_len;i++)
				arr->set(i,0);
			_t.tother = reinterpret_cast<void*> ( arr );

			
		} break;
		case REAL_ARRAY: {
			
			ERR_FAIL_COND( p_len <= 0 );
			
			_type = REAL_ARRAY;
			
			DVector<double> * arr = memnew( DVector<double> );
			arr->resize(p_len);
			for (int i=0;i<p_len;i++)
				arr->set(i,0);
			_t.tother = reinterpret_cast<void*> ( arr );
			
		} break;
		case STRING_ARRAY: {
			
			ERR_FAIL_COND( p_len <= 0 );
			_type = STRING_ARRAY;
			DVector<String> * arr = memnew( DVector<String> );
			arr->resize(p_len);
			_t.tother = reinterpret_cast<void*> ( arr );
			
		} break;
		case BYTE_ARRAY: {
			
			
			ERR_FAIL_COND( p_len <= 0 );
			_type = BYTE_ARRAY;
			DVector<Uint8> * arr = memnew( DVector<Uint8> );
			arr->resize(p_len);
			for (int i=0;i<p_len;i++)
				arr->set(i,0);
			_t.tother = reinterpret_cast<void*> ( arr );
			
		} break;
		case VARIANT_ARRAY: {
			
			ERR_FAIL_COND( p_len <= 0 );
			_type = VARIANT_ARRAY;
			DVector<Variant> * arr = memnew( DVector<Variant> );
			arr->resize(p_len);
			_t.tother = reinterpret_cast<void*> ( arr );
			
		} break;
		
		/* Math */
		
		case VECTOR3: {
			
			set_vector3( Vector3(0,0,0) );
		} break;
		case MATRIX4: {
			
			set_matrix4( Matrix4() );
			
		} break;
		case PLANE: {
			
			set_plane( Plane() );
			
		} break;
		case RECT3: {
			
			set_rect3( Rect3() );
			
		} break;
		case QUAT: {
			
			set_quat( Quat() );
			
		} break;
		case COLOR: {
			
			set_color( Color() );
			
		} break;
		/* Data */
		
		case IMAGE: {
			
			set_image( Image() );
			
		} break;
		case IOBJ: {
			
			set_IOBJ( IObjRef() );
			
		} break;
		default: { ERR_PRINT("Forgot something?"); clear(); }
	}	
}

void Variant::serialize(Serializer *p_serializer) const {
	
	p_serializer->add_uint8( get_type() );
	
	switch (get_type()) {
	
		case NIL: {
		
			return ;
		} break;
		
		/* Basic, built in types */
		case BOOL: {
		
			p_serializer->add_uint8( get_bool() );
		} break;
		case INT: {
		
			p_serializer->add_uint32( get_int() );

		} break;
  		case REAL: {
  		
			p_serializer->add_double( get_real() );
  		
  		} break;
		case STRING: {
		
			p_serializer->add_string( get_string() );
			
		} break;
		/* Extra types (allocate internally */
		// arrays , share size() //
		case INT_ARRAY: {
		
			p_serializer->add_uint32( size() );
			
			for (int i=0;i<size();i++) {
			
				p_serializer->add_uint32( array_get_int(i) );
			}
						
		} break;
		case REAL_ARRAY: {
		
			p_serializer->add_uint32( size() );
			
			for (int i=0;i<size();i++) {
			
				p_serializer->add_double( array_get_real(i) );
			}
		
		} break;
		case STRING_ARRAY: {
		
			p_serializer->add_uint32( size() );
			
			for (int i=0;i<size();i++) {
			
				p_serializer->add_string( array_get_string(i) );
			}
		
		} break;
		case BYTE_ARRAY: {
		
			p_serializer->add_uint32( size() );
			
			for (int i=0;i<size();i++) {
			
				p_serializer->add_uint8( array_get_byte(i) );
			}
		
		} break;
		case VARIANT_ARRAY: {
		
			p_serializer->add_uint32( size() );
			
			for (int i=0;i<size();i++) {
			
				p_serializer->add_variant( array_get(i) );
			}
		
		} break;
		
		/* Math */
		
		case VECTOR3: {
		
			p_serializer->add_vector3( get_vector3() );
		
		} break;
		case MATRIX4: {
		
			p_serializer->add_matrix4( get_matrix4() );
		
		} break;
		case PLANE: {
		
			p_serializer->add_plane( get_plane() );
			
		} break;
		case RECT3: {
		
			p_serializer->add_rect3( get_rect3() );
		
		} break;
		case QUAT: {
		
			p_serializer->add_quat( get_quat() );
		
		} break;
		
		case COLOR: {
		
			p_serializer->add_color( get_color() );		
		} break;
		/* Data */
		
		case IMAGE: {
		
			Image *img = reinterpret_cast<Image*>( _t.tother );
			
			switch(img->get_format()) {
				
				case Image::FORMAT_GRAYSCALE: {
					p_serializer->add_string("grayscale");				
				} break;
				case Image::FORMAT_GRAYSCALE_ALPHA: {
					p_serializer->add_string("grayscale_alpha");						
				} break;
				case Image::FORMAT_RGB: {
					p_serializer->add_string("rgb");					
				} break;
				case Image::FORMAT_RGBA: {
					p_serializer->add_string("rgba");						
				} break;
				case Image::FORMAT_INDEXED: {
					p_serializer->add_string("indexed");						
				} break;
				case Image::FORMAT_INDEXED_ALPHA: {
					
					p_serializer->add_string("indexed_alpha");						
				} break;
			}
			
			p_serializer->add_uint32(img->get_width());
			p_serializer->add_uint32(img->get_height());
			
			DVector<Uint8> imgdata = img->get_data();
			imgdata.read_lock();
			const Uint8 *ptr = imgdata.read();
			int len=imgdata.size();
			p_serializer->add_uint32(len);
			for (int i=0;i<len;i++) {
				
				p_serializer->add_uint8(ptr[i]);
			}
			imgdata.read_unlock();
			
		} break;
		case IOBJ: {
		
			if (get_IOBJ().is_null()) {
				
				printf("serialize empty iobj\n");
				// 0 == empty iobj
				p_serializer->add_uint8(0);
				
			} else if (get_IOBJ()->get_ID()!=OBJ_INVALID_ID){
				
				printf("serialize ref iobj\n");
				p_serializer->add_uint8(1); // 1 == reference iobj
				p_serializer->add_uint32( get_IOBJ()->get_ID() );		
			} else {
				printf("serialize bundled iobj\n");
				
				IAPIRef iapi=get_IOBJ();
				if (iapi.is_null()) {
					
					ERR_PRINT("INVALID IOBJ");
					p_serializer->add_uint8(0); // 0 == empty iobj
					break;
				}
					
				p_serializer->add_uint8(2); // 2 == embed iobj
				
				p_serializer->add_string( iapi->persist_as() );
				iapi->serialize_creation_params( p_serializer );
				iapi->serialize( p_serializer, IAPI::PropertyInfo::USAGE_SERIALIZE );
								
			}

		} break;
		default: {}
	
	}	
}

Error Variant::deserialize(Deserializer *p_deserializer) {
	
	Uint8 type=p_deserializer->get_uint8();
	
	switch(type) {
	
		case NIL: {
		
			/* nonoe */
		} break;
		/* Basic, built in types */
		case BOOL: {
		
			set_bool( p_deserializer->get_uint8() );
		} break;
		case INT: {
		
			set_int( p_deserializer->get_uint32() );

		} break;
  		case REAL: {
  		
			set_real( p_deserializer->get_double() );
  		
  		} break;
		case STRING: {
		
			set_string( p_deserializer->get_string() );
		
		} break;
		/* Extra types (allocate internally */
		// arrays , share size() //
		case INT_ARRAY: {
		
			int count=p_deserializer->get_uint32();
			create( INT_ARRAY, count );
			
			for (int i=0;i<count;i++) {
				
				array_set_int( i, p_deserializer->get_uint32() );
			}
									
		} break;
		case REAL_ARRAY: {
		
			int count=p_deserializer->get_uint32();
			create( REAL_ARRAY, count );
			
			for (int i=0;i<count;i++) {
				
				array_set_real( i, p_deserializer->get_double() );
			}
		
		} break;
		case STRING_ARRAY: {

			int count=p_deserializer->get_uint32();
			create( STRING_ARRAY, count );
			
			for (int i=0;i<count;i++) {
				
				array_set_string( i, p_deserializer->get_string() );
			}
		
		} break;
		case BYTE_ARRAY: {
		
			int count=p_deserializer->get_uint32();
			create( BYTE_ARRAY, count );
			
			for (int i=0;i<count;i++) {
				
				array_set_byte( i, p_deserializer->get_uint8() );
			}
		
		} break;
		case VARIANT_ARRAY: {
		
		
			int count=p_deserializer->get_uint32();
			create( VARIANT_ARRAY, count );
			
			for (int i=0;i<count;i++) {
				
				Variant v;
				p_deserializer->get_variant(v);
				array_set( i, v );
			}
		
		} break;
		
		/* Math */
		
		case VECTOR3: {
		
			set_vector3( p_deserializer->get_vector3() );
		
		} break;
		case MATRIX4: {
		
			set_matrix4( p_deserializer->get_matrix4() );
		
		} break;
		case PLANE: {
		
			set_plane( p_deserializer->get_plane() );
		} break;
		case RECT3: {
		
			set_rect3( p_deserializer->get_rect3() );
		
		} break;
		case QUAT: {
		
			set_quat( p_deserializer->get_quat() );
		
		} break;
		
		case COLOR: {
		
			set_color( p_deserializer->get_color() );
		
		} break;
		/* Data */
		case IMAGE: {
		
			DVector<Uint8> data;
			
			String str = p_deserializer->get_string();
			Image::Format fmt;
			if (str=="grayscale")
				fmt=Image::FORMAT_GRAYSCALE;
			else if (str=="grayscale_alpha")
				fmt=Image::FORMAT_GRAYSCALE_ALPHA;
			else if (str=="rgb")
				fmt=Image::FORMAT_RGB;
			else if (str=="rgba")
				fmt=Image::FORMAT_RGBA;
			else if (str=="indexed")
				fmt=Image::FORMAT_INDEXED;
			else if (str=="indexed_alpha")
				fmt=Image::FORMAT_INDEXED_ALPHA;
			else {
				ERR_PRINT("Invalid Format");
				return ERR_INVALID_DATA;
			}
			int width=p_deserializer->get_uint32();
			int height=p_deserializer->get_uint32();
			data.resize( p_deserializer->get_uint32() );
			int len=data.size();
			data.write_lock();
			Uint8* buf = data.write();
			for (int i=0;i<len;i++) {
				
				buf[i]=p_deserializer->get_uint8();	
			}
			
			data.write_unlock();
			
			Image img(width,height,fmt,data);
			set_image(img);
		} break;
		
		case IOBJ: {
		
			switch(p_deserializer->get_uint8()) {
				
				case 0: { //empty iobj
					//printf("IOBJ empty\n");
					set_IOBJ( IObjRef() );
				} break;
				case 1: { //reference iobj
					//printf("IOBJ reference\n");
					
					ObjID objID=p_deserializer->get_uint32();
					
					IAPIRef obj = IAPI_Persist::get_singleton()->load(objID);
					set_IOBJ( obj );
				} break;
				case 2: { //embed iobj
					
					String type=p_deserializer->get_string();
					//printf("IOBJ embed %ls\n", type.c_str());
					
					IAPI::CreationParams mp;
					IAPI::deserialize_creation_params(p_deserializer,mp);

					IRef<IAPI> iapi = IAPI_Persist::get_singleton()->instance_type(type,mp);
					ERR_FAIL_COND_V(iapi.is_null(),ERR_BUG);
				
					iapi->deserialize(p_deserializer);
					set_IOBJ( iapi );
				} break; 
			}
		
		} break;
		default: {
		
			ERR_PRINT("Invalid Variant Type");
		} break;
	
	}
	
	return OK;	
}



String Variant::textualize() const {
	
	return "";
}
void Variant::parse(String p_str) {
	
	
}

void Variant::init() {
	
	_type=NIL;
	_t.tint=0;
	
}

void Variant::clear() {
	
	switch(_type) {
		
		case INT_ARRAY: {
			
			memdelete( reinterpret_cast<DVector<int>*>( _t.tother ) );
		} break;
		case REAL_ARRAY: {
			
			memdelete( reinterpret_cast<DVector<double>*>( _t.tother ) );
		
		} break;
		case STRING_ARRAY: {
			
			memdelete( reinterpret_cast<DVector<String>*>( _t.tother ) );
			
		} break;
		case BYTE_ARRAY: {
			
			memdelete( reinterpret_cast<DVector<Uint8>*>( _t.tother ) );			
		} break;
		
		/* Variant */
		case VARIANT_ARRAY: {
			
			memdelete( reinterpret_cast<DVector<Variant>*>( _t.tother ) );			
			
		} break;
		
		/* Math */
		
		case VECTOR3: {
			
			memdelete( reinterpret_cast<Vector3*>( _t.tother ) );			
			
		} break;
		case MATRIX4: {
			
			memdelete( reinterpret_cast<Matrix4*>( _t.tother ) );			
			
		} break;
		case PLANE: {
			
			memdelete( reinterpret_cast<Plane*>( _t.tother ) );			
			
		} break;
		case RECT3: {
			
			memdelete( reinterpret_cast<Rect3*>( _t.tother ) );			
			
		} break;
		case QUAT: {
			
			memdelete( reinterpret_cast<Quat*>( _t.tother ) );			
			
		} break;
		case COLOR: {
			
			memdelete( reinterpret_cast<Color*>( _t.tother ) );			
			
		} break;
		/* Etc. */
		case IMAGE: {
			
			memdelete( reinterpret_cast<Image*>( _t.tother ) );			
			
		} break;
		case IOBJ: {
			
			memdelete( reinterpret_cast<IObjRef*>( _t.tother ) );			
			
		} break;
		default: {}
		
	}
	
	_type=NIL;
	_t.tint=0;
	_tstr="";
	
}

String Variant::get_type_string(Variant::Type p_type) {


	switch(p_type) {
		case NIL: return "void";
		case BOOL: return "bool";
		case INT: return "int";
		case REAL: return "real_t";
		case STRING: return "String";
		case INT_ARRAY: return "int[]";
		case REAL_ARRAY: return "real_t[]";
		case STRING_ARRAY: return "String[]";
		case BYTE_ARRAY: return "byte[]";
		case VARIANT_ARRAY: return "Variant[]";
		case VECTOR3: return "Vector3";
		case MATRIX4: return "Matrix4";
		case PLANE: return "Plane";
		case RECT3: return "Rect3";
		case QUAT: return "Quat";
		case COLOR: return "Color";
		case IMAGE: return "Image";
		case IOBJ: return "IObj";
	}
	
	return "";

}

Variant::Variant() {
	
	_type=NIL;
	_t.tint=0;
	
}
Variant::~Variant() {
	
	clear();
}
