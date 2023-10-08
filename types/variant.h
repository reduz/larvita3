
#ifndef VARIANT_H
#define VARIANT_H

#include "typedefs.h"
#include "rstring.h"
#include "vector3.h"
#include "matrix4.h"
#include "plane.h"
#include "aabb.h"
#include "quat.h"
#include "iref.h"
#include "image.h"
#include "dvector.h"

/**
 * @class Variant
 * @author Juan Linietsky
 * class that can contain several many types inside.
 * it's only 16 bytes in size when using the basic types, and allocates
 * the needed memory for the extra ones.
 */

SCRIPT_BIND_BEGIN;
class Variant {
public:
	enum Type {
	
		NIL=0, ///< empty, unused
		
		/* Basic, built in types */
		BOOL=1, ///< True/False
		INT=2, ///< signed 32 bits integer 
  		REAL=3, ///< double precision floating point
		STRING=4, ///< String (real)
		
		/* Extra types (allocate internally */
		// arrays , share size() //
		INT_ARRAY=20, ///< Array of integers, 32 bits signed
		REAL_ARRAY=21, ///< Array of double precision floating point
		STRING_ARRAY=22,
		BYTE_ARRAY=23, ///< custom data / buffer
		VARIANT_ARRAY=24, ///< array of variants
		
		/* Math */
		
		VECTOR3=40, ///< Vector3
		MATRIX4=41, ///< 4x4 Matrix
		PLANE=42, ///< Plane
		RECT3=43, ///< Rect
		AABB=RECT3, ///< Axis Aligned Bounding Box (same as Rect3)
		QUAT=44, ///< Quaternion
		
		COLOR=50,
		/* Data */
  		
		IMAGE=70, // Image type
		IOBJ=100, ///< IObjRef

	};
		
private:
	
	Type _type;

	union _T {
	
		bool tbool;
		Sint32 tint;
		double treal;
		void *tother; // wathever else type is used, this holds a mem pointer
	};
	_T _t;
	
	String _tstr; /* string type is here, to avoid messing with mem too much, at the cost of a pointer, also enums use this */
	
	void init();
	void clear();
	
	/* Internal set functions */
	
	void set_bool(bool p_v);
	void set_int(int p_v); 
	void set_real(double p_v);
	void set_string(String p_v);
	void set_variant(const Variant& p_variant);
	void set_vector3(const Vector3& p_v);
	void set_matrix4(const Matrix4& p_v);
	void set_plane(const Plane& p_v);
	void set_rect3(const Rect3& p_v);
	void set_quat(const Quat& p_v);
	void set_color(const Color& p_v);
	void set_image(const Image& p_v);
	void set_IOBJ(const IObjRef& p_v);
	
public:		
		
		
	Type get_type() const; ///< Query the variant type */
	int size() const; ///< get size, for arrray types, otherwise returns 0
			
	/* constructors */

	Variant(bool p_v);
	/* these are used for compiler disambiguation (best match), even though all of them are set_int */
	Variant(Uint8 p_v); 
	Variant(Uint16 p_v); 
	Variant(Uint32 p_v); 
	Variant(Uint64 p_v); 
	Variant(Sint8 p_v); 
	Variant(Sint16 p_v); 
	Variant(Sint32 p_v); 
	Variant(Sint64 p_v); 
	
	/* these are used for compiler disambiguation (best match), even though both are set_real*/
	Variant(float p_v); ///< goes as 32
	Variant(double p_v); ///< goes as 32
	
	Variant(const char *p_cstr);
	Variant(String::CharType *p_wcstr);
	Variant(String p_v);
	Variant(Type p_type,int p_len=0); ///< create specifically for a type, and optionally define a length for arrays
	Variant(const Variant& p_v); ///< regular copy constructor
	
	Variant(const Vector3& p_v);
	Variant(const Matrix4& p_v);
	Variant(const Plane& p_v);
	Variant(const Rect3& p_v);
	Variant(const Quat& p_v);
	Variant(const Color& p_v);
	Variant(const Image& p_v);
	Variant(const IObjRef& p_v);
	
	/* only this one needs to exist and it will work for all cases*/
	void operator=(const Variant& p_v); ///< regular copy constructor
	
	/* logic/math operators */
	
	//bool operator==(const Variant& p_variant) const;
	//bool operator!=(const Variant& p_variant) const;
	
	/* these would rock, but it's too much work
	Variant operator+(const Variant& p_variant) const;
	Variant operator-(const Variant& p_variant) const;
	Variant operator*(const Variant& p_variant) const;
	Variant operator/(const Variant& p_variant) const;
	
	void operator+=(const Variant& p_variant);
	void operator-=(const Variant& p_variant);
	void operator*=(const Variant& p_variant);
	void operator/=(const Variant& p_variant);
	*/
	/* gets, conversion will take place if possible, otherwise not. if index is passed, array is assumed. if a type not of array has an index, variant array is assumed. */
	
	bool get_bool() const;
	int get_int() const;
	double get_real() const;
	String get_string() const;
	
	DVector<Uint8> get_byte_array() const; // get a copy 
	DVector<String> get_string_array() const; // get a copy 
	DVector<int> get_int_array() const; // get a copy 
	DVector<double> get_real_array() const; // get a copy 
	DVector<Variant> get_variant_array() const; // get a copy 
	
	Vector3 get_vector3() const;
	Matrix4 get_matrix4() const;
	Plane get_plane() const;
	Rect3 get_rect3() const;
	Quat get_quat() const;
	Color get_color() const;
	Image get_image() const;
	IObjRef get_IOBJ() const;
	
	/* assignment operators */
	inline operator bool() const { return get_bool(); }
	inline operator Uint8() const { return get_int(); }
	inline operator Sint8() const { return get_int(); }
	inline operator Uint16() const { return get_int(); }
	inline operator Sint16() const { return get_int(); }
	inline operator Uint32() const { return get_int(); }
	inline operator Sint32() const { return get_int(); }
	inline operator Uint64() const { return get_int(); }
	inline operator Sint64() const { return get_int(); }
	inline operator float() const { return get_real(); }
	inline operator double() const { return get_real(); }
	inline operator String() const { return get_string(); }
	inline operator Vector3() const { return get_vector3(); }
	inline operator Matrix4() const { return get_matrix4(); }
	inline operator Plane() const { return get_plane(); }
	inline operator Rect3() const { return get_rect3(); }
	inline operator Quat() const { return get_quat(); }
	inline operator Color() const { return get_color(); }
	inline operator Image() const { return get_image(); }
	inline operator IObjRef() const { return get_IOBJ(); }

	void set_byte_array(const DVector<Uint8>& p_array);
	void set_string_array(const DVector<String>& p_array); 
	void set_int_array(const DVector<int>& p_array); 
	void set_real_array(const DVector<double>& p_array); 
	void set_variant_array(const DVector<Variant>& p_array); 
	
	/* specialized array set/get */
	void array_set_int(int p_idx, int p_value);
	int array_get_int(int p_idx) const;
	void array_set_real(int p_idx, double p_value);
	double array_get_real(int p_idx) const;
	void array_set_string(int p_idx, String p_value);
	String array_get_string(int p_idx) const;
	void array_set_byte(int p_idx, Uint8 p_value);
	Uint8 array_get_byte(int p_idx) const;
	
	
	/* general array set/get, for variants and other supported array types */
	void array_set(int p_idx, const Variant& p_variant);
	Variant array_get(int p_idx) const;
	
	Variant& operator[](int p_idx);
	Variant operator[](int p_idx) const;
	
	void create(Type p_type,int p_len=0); ///< create specifically for a type, and optionally define a length for arrays
	
	void serialize(Serializer *p_serializer) const; 
	Error deserialize(Deserializer *p_deserializer);
		
	String textualize() const; ///< return the string representation of this
	void parse(String p_str); ///< parse a string representation into this

	static String get_type_string(Variant::Type p_type);

	Variant(); ///< variants are of type NIL by default */
	~Variant();


};

SCRIPT_BIND_END;
#endif
