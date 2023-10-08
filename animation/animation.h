#ifndef ANIMATION_H
#define ANIMATION_H

#include "vector3.h"
#include "quat.h"
#include "dvector.h"
#include "rstring.h"
#include "iapi.h"
#include "vector.h"

class Animation : public IAPI {

	IAPI_TYPE( Animation, IAPI );

public:
	
	struct Value {
		
		String name;
		Variant value;
	};

private:
	enum {
		
		ERR_INVALID_IDX=-2
	};
	
	template<class T>
	struct Key {
				
		float offset;
		T data;
		
		Key() { offset=0; }
	};
	
	typedef Key<Vector3> LocationKey;
	typedef Key<Quat> RotationKey;
	typedef Key<Vector3> ScaleKey;
	typedef Key<Value> ValueKey;
	
	struct Track {
	
		String name;
	
		Vector< LocationKey > location_keys;
		Vector< RotationKey > rotation_keys;
		Vector< ScaleKey > scale_keys;

		Vector< ValueKey > value_keys;
	};
	
	bool loop;
	float length;
	
	Vector<Track*> tracks;	
	
	template<class T>
	int get_key_idx( const Vector<Key<T> >& p_track, float p_time, bool* r_exact=NULL ) const;
	
	template<class T>
	inline int insert_key( Vector< Key<T> >& p_track, float p_time, const T& p_data, bool p_overwrite = true );
		
public:

	static IRef<IAPI> IAPI_DEFAULT_INSTANCE_FUNC(const String& p_type,const  IAPI::CreationParams& p_params);
	
	/* Properties */
	
	virtual void set(String p_path, const Variant& p_value); 
	virtual Variant get(String p_path) const;
	virtual void get_property_list( List<PropertyInfo> *p_list ) const;
	
	virtual PropertyHint get_property_hint(String p_path) const;
	virtual Variant call(String p_method, const List<Variant>& p_params=List<Variant>());	
	virtual void get_method_list( List<MethodInfo> *p_list ) const; ///< Get a list with all the properies inside this object
	
	void clear();
	
	int add_track(String p_name);
	
	void add_location_key(int p_track,float p_offset,const Vector3& p_location);
	void add_rotation_key(int p_track,float p_offset,const Quat& p_rotation);
	void add_scale_key(int p_track,float p_offset,const Vector3& p_scale);

	struct ValueTrackState {

		bool has_frames;
		int start_frame;
		int end_frame;

		ValueTrackState() { has_frames = false; };
	};
	
	int add_value_key(int p_track, float p_offset, String p_name, Variant p_value, bool p_overwrite = false);
	Error get_value_track_state(int p_track, float p_time, float p_elapsed, ValueTrackState& r_state);

	Value get_value_frame(int p_track, int p_frame);
	float get_value_offset(int p_track, int p_frame);
	int get_value_frames_count(int p_track) const;
	
	void remove_value_key(int p_track, int p_key);
	
	struct TrackState {
		
		Vector3 location;
		Quat rotation;
		Vector3 scale;
		bool has_location;
		bool has_rotation;
		bool has_scale;				
		TrackState() { has_location=has_rotation=has_scale=false; }
	};
	
	Error get_track_state(int p_track,float p_time,TrackState& r_state);
	
	bool has_track(String p_name) const;
	int get_track_count() const;
	String get_track_name(int p_track) const;
	int get_track_index(String p_name) const;

	void set_loop(bool p_enabled);
	bool has_loop() const;
	
	void set_length(float p_length);
	float get_length() const;

	Animation();
	~Animation();

};


#endif
