#include "animation.h"
#include "fileio/animation_loader.h"

IRef<IAPI> Animation::IAPI_DEFAULT_INSTANCE_FUNC(const String& p_type,const  IAPI::CreationParams& p_params) {
	
	if (p_params.has("track_names")) {
		
		Variant v=p_params.get("track_names");
		ERR_FAIL_COND_V( v.get_type()!=Variant::STRING_ARRAY, IRef<IAPI>() );
		
		Animation * a = memnew( Animation );
		bool err=false;
		for (int i=0;i<v.size();i++) {
			
			if (a->add_track( v.array_get_string(i))<0) {
				
				err=true;
				break;
			}
		}
		
		if (err) {
			
			memdelete(a);
			ERR_FAIL_COND_V(err, IRef<IAPI>() );
		}
		
		return a;
	} else {
		
		return memnew( Animation );
	}
}

template<class T>
inline int Animation::get_key_idx( const Vector< Key<T> >& p_track, float p_time,bool* r_exact ) const {
	
	if (r_exact)
		*r_exact=false;
	
	if (p_track.size()==0)
		return ERR_INVALID_IDX;
	
	int low = 0;
	int high = p_track.size() -1;
	int middle;
	
	const Key<T> *a=&p_track[0];

	while( low <= high ) {
		
		middle = ( low  + high ) / 2;

		if( p_time == a[  middle ].offset ) { //match
			if (r_exact)
				*r_exact=true;
			return middle;
		} else if( p_time < a[middle].offset )
			high = middle - 1; //search low end of array
		else
			low = middle + 1; //search high end of array
	}

	if (a[middle].offset>p_time)
		middle--;
	
	return middle;
}
	
template<class T>
inline int Animation::insert_key( Vector< Key<T> >& p_track, float p_time, const T& p_data, bool p_overwrite ) {
	
	Key<T> new_v;
	new_v.offset=p_time;
	new_v.data=p_data;
	
	bool exact;
	int pos=get_key_idx<T>(p_track,p_time,&exact);
	
	if (pos==ERR_INVALID_IDX) { //it's empty!
		pos=p_track.size();
		p_track.push_back(new_v);
		
	} else if (!exact) { /* no exact position found, make room */
		p_track.insert(pos+1,new_v);
	} else {

		if (p_overwrite) {
			p_track[pos]=new_v; /* Overwrite, sine exact position */

		} else {
			p_time += 0.00001;
			return insert_key(p_track, p_time, p_data, false);
		};
		
	}
	
	return pos;		
}


void Animation::set(String p_path, const Variant& p_value) {
	
	if (p_path=="length")
		set_length(p_value);
	
	if (p_path=="loops")
		set_loop(p_value);
	
	if (p_path.find("tracks/")==0) {
		
		int idx=p_path.get_slice("/",1).to_int();
		ERR_FAIL_INDEX(idx,tracks.size());
		
		String var=p_path.get_slice("/",2);
		
		if (var=="name") {
			
			tracks[idx]->name = p_value;
			return;
		};
		
		if (var=="loc_keys") {
			
			ERR_FAIL_COND(p_value.get_type()!=Variant::REAL_ARRAY);
			
			tracks[idx]->location_keys.resize(p_value.size()/4);
			
			DVector<double> dvf=p_value.get_real_array();
			
			dvf.read_lock();
			
			const double *data=dvf.read();
			
			for(int i=0;i<tracks[idx]->location_keys.size();i++) {
			
				LocationKey lk;
				lk.offset=data[i*4+0];
				lk.data.x=data[i*4+1];
				lk.data.y=data[i*4+2];
				lk.data.z=data[i*4+3];
				
				tracks[idx]->location_keys[i]=lk;
			}
			dvf.read_unlock();
					
		}
		
		if (var=="rot_keys") {
			
			ERR_FAIL_COND(p_value.get_type()!=Variant::REAL_ARRAY);
			
			tracks[idx]->rotation_keys.resize(p_value.size()/5);
			
			DVector<double> dvf=p_value.get_real_array();
			
			dvf.read_lock();
			
			const double *data=dvf.read();
			
			for(int i=0;i<tracks[idx]->rotation_keys.size();i++) {
			
				RotationKey rk;
				rk.offset=data[i*5+0];
				rk.data.x=data[i*5+1];
				rk.data.y=data[i*5+2];
				rk.data.z=data[i*5+3];
				rk.data.w=data[i*5+4];
				
				tracks[idx]->rotation_keys[i]=rk;
			}
			dvf.read_unlock();
		}
		
		if (var=="scale_keys") {
			
			ERR_FAIL_COND(p_value.get_type()!=Variant::REAL_ARRAY);
			
			tracks[idx]->scale_keys.resize(p_value.size()/4);
			
			DVector<double> dvf=p_value.get_real_array();
			
			dvf.read_lock();
			
			const double *data=dvf.read();
			
			
			for(int i=0;i<tracks[idx]->scale_keys.size();i++) {
			
				ScaleKey sk;
				sk.offset=data[i*4+0];
				sk.data.x=data[i*4+1];
				sk.data.y=data[i*4+2];
				sk.data.z=data[i*4+3];
				
				tracks[idx]->scale_keys[i]=sk;
			}
			dvf.read_unlock();
		}
		
		if (var == "value_keys") {
			
			ERR_FAIL_COND(p_value.get_type()!=Variant::VARIANT_ARRAY);
			tracks[idx]->value_keys.resize(p_value.size()/3);
			
			for (int i=0; i<tracks[idx]->value_keys.size(); i++) {
				
				ValueKey vk;
				vk.offset = p_value.array_get(i*3+0).get_real(); // get real!
				vk.data.name = p_value.array_get(i*3+1).get_string();
				vk.data.value = p_value.array_get(i*3+2);
				
				tracks[idx]->value_keys[i] = vk;
			};
		};
		
	}
	
	
}

Variant Animation::get(String p_path) const {
	
	if (p_path=="track_names") {
		Variant v(Variant::STRING_ARRAY,tracks.size());
		for(int i=0;i<tracks.size();i++)
			v.array_set_string(i,tracks[i]->name);
		
		return v;
	}
				
	if (p_path=="length")
		return length;
	
	if (p_path=="loops")
		return loop;
	
	if (p_path.find("tracks/")==0) {
		
		int idx=p_path.get_slice("/",1).to_int();
		ERR_FAIL_INDEX_V(idx,tracks.size(),Variant());
		
		String var=p_path.get_slice("/",2);
		
		if (var=="name")
			return tracks[idx]->name;
		
		if (var=="loc_keys") {
			
			Variant keys(Variant::REAL_ARRAY,tracks[idx]->location_keys.size()*4);
			
			for(int i=0;i<tracks[idx]->location_keys.size();i++) {
				const LocationKey &lk=tracks[idx]->location_keys[i];
				keys.array_set_real(i*4+0,lk.offset);
				keys.array_set_real(i*4+1,lk.data.x);
				keys.array_set_real(i*4+2,lk.data.y);
				keys.array_set_real(i*4+3,lk.data.z);
			}
			return keys;
		}
		
		if (var=="rot_keys") {
			
			Variant keys(Variant::REAL_ARRAY,tracks[idx]->rotation_keys.size()*5);
			
			for(int i=0;i<tracks[idx]->rotation_keys.size();i++) {
				const RotationKey &lk=tracks[idx]->rotation_keys[i];
				keys.array_set_real(i*5+0,lk.offset);
				keys.array_set_real(i*5+1,lk.data.x);
				keys.array_set_real(i*5+2,lk.data.y);
				keys.array_set_real(i*5+3,lk.data.z);
				keys.array_set_real(i*5+4,lk.data.w);
			}
			return keys;
		}
		
		if (var=="scale_keys") {
			
			Variant keys(Variant::REAL_ARRAY,tracks[idx]->scale_keys.size()*4);
			
			for(int i=0;i<tracks[idx]->scale_keys.size();i++) {
				const ScaleKey &lk=tracks[idx]->scale_keys[i];
				keys.array_set_real(i*4+0,lk.offset);
				keys.array_set_real(i*4+1,lk.data.x);
				keys.array_set_real(i*4+2,lk.data.y);
				keys.array_set_real(i*4+3,lk.data.z);
			}
			return keys;
		}
		
		if (var == "value_keys") {
			
			Variant keys(Variant::VARIANT_ARRAY, tracks[idx]->value_keys.size() * 3);
			
			for(int i=0;i<tracks[idx]->value_keys.size();i++) {
				
				const ValueKey& vk = tracks[idx]->value_keys[i];
				keys.array_set(i*3+0, vk.offset);
				keys.array_set(i*3+1, vk.data.name);
				keys.array_set(i*3+2, vk.data.value);
			};
			
			return keys;
		};
		
	}
			
	return Variant();
}

IAPI::PropertyHint Animation::get_property_hint(String p_path) const {
	
	if( p_path=="length" ) {
		
		return IAPI::PropertyHint( PropertyHint::HINT_RANGE, "0,36000,0.001","Total time of animtion");
	}
	
	return PropertyHint();
}

Variant Animation::call(String p_method, const List<Variant>& p_params) {
		
	if (p_method=="clear")
		clear();
	else if (p_method=="add_track") {
		
		ERR_FAIL_COND_V(p_params.size()<1,Variant());
		
		add_track(p_params[0].get_string());
		property_changed_signal.call("");

	} else if (p_method=="import") {
		
		ERR_FAIL_COND_V(p_params.size()<1,Variant());

		IRef<Animation> anim = AnimationLoader::load_animation(p_params[0].get_string());
		ERR_FAIL_COND_V(anim.is_null(),Variant());
		Animation *src=anim.ptr();
		ERR_FAIL_COND_V(!src,Variant());
		
		clear(); // important step, clear the animation
				
		loop=src->loop;
		length=src->length;
		tracks.resize(src->tracks.size());
		for (int i=0;i<tracks.size();i++) {
		
			tracks[i]=memnew(Track);
			*tracks[i]=*src->tracks[i];
		}
		
		property_changed_signal.call("");

	}
	
	
	return Variant();
}
void Animation::get_method_list( List<MethodInfo> *p_list ) const {
	
	MethodInfo mi("add_track");
	mi.parameters.push_back( MethodInfo::ParamInfo( Variant::STRING, "name" ) );
	p_list->push_back( mi );
	
	mi.name="import";
	mi.parameters.clear();
	mi.parameters.push_back( MethodInfo::ParamInfo( Variant::STRING, "path", PropertyHint( PropertyHint::HINT_FILE,"*.dae") ) );
	p_list->push_back( mi );
	
	p_list->push_back( MethodInfo("clear") );
	
}


void Animation::get_property_list( List<PropertyInfo> *p_list ) const {
	
	if (tracks.size())
		p_list->push_back( PropertyInfo( Variant::STRING_ARRAY, "track_names",PropertyInfo::USAGE_CREATION_PARAM ) );
	
	p_list->push_back( PropertyInfo( Variant::REAL, "length" ) );
	p_list->push_back( PropertyInfo( Variant::BOOL, "loops" ) );
	
	for (int i=0;i<tracks.size();i++) {
		
		String path="tracks/"+String::num(i)+"/";
		
		p_list->push_back( PropertyInfo( Variant::STRING, path+"name" ) );
		if (tracks[i]->location_keys.size())
			p_list->push_back( PropertyInfo( Variant::REAL_ARRAY, path+"loc_keys" ) );
		if (tracks[i]->rotation_keys.size())
			p_list->push_back( PropertyInfo( Variant::REAL_ARRAY, path+"rot_keys" ) );
		if (tracks[i]->scale_keys.size())
			p_list->push_back( PropertyInfo( Variant::REAL_ARRAY, path+"scale_keys" ) );						
		if (tracks[i]->value_keys.size())
			p_list->push_back( PropertyInfo( Variant::REAL_ARRAY, path+"value_keys" ) );						
	}
	
	
}


void Animation::clear() {
	
	for (int i=0;i<tracks.size();i++)
		memdelete( tracks[i] );
	
	tracks.clear();
	loop=false;
	length=0;
	
	
}

int Animation::add_track(String p_name) {
	
	ERR_FAIL_COND_V(p_name=="",-1);
			
	for (int i=0;i<tracks.size();i++) {
		ERR_FAIL_COND_V((tracks[i]->name==p_name),-1); // track exists
	}
	
	tracks.push_back( memnew( Track ) );
	tracks[ tracks.size() -1 ]->name=p_name;
		
	return tracks.size()-1;
}

void Animation::remove_value_key(int p_track, int p_key) {
	
	ERR_FAIL_INDEX(p_track, tracks.size());

	Track &t = *tracks[p_track];
	ERR_FAIL_INDEX(p_key, t.value_keys.size());
	
	t.value_keys.remove(p_key);
};


int Animation::add_value_key(int p_track, float p_offset, String p_name, Variant p_value, bool p_overwrite) {

	ERR_FAIL_COND_V(p_offset < 0, -1);
	ERR_FAIL_INDEX_V(p_track, tracks.size(), -1);
	
	Track &t = *tracks[p_track];
	
	Value val;
	val.name = p_name;
	val.value = p_value;
	
	return insert_key<Value>( t.value_keys, p_offset, val, p_overwrite );
};

void Animation::add_location_key(int p_track,float p_offset,const Vector3& p_location) {
	
	ERR_FAIL_COND(p_offset<0);
	ERR_FAIL_INDEX(p_track,tracks.size());

	Track &t=*tracks[p_track];
	insert_key<Vector3>( t.location_keys, p_offset, p_location);
	
}
void Animation::add_rotation_key(int p_track,float p_offset,const Quat& p_rotation) {
	ERR_FAIL_COND(p_offset<0);	
	ERR_FAIL_INDEX(p_track,tracks.size());
	
	Track &t=*tracks[p_track];
	insert_key<Quat>(t.rotation_keys, p_offset, p_rotation );
	
}
void Animation::add_scale_key(int p_track,float p_offset,const Vector3& p_scale) {
	
	ERR_FAIL_COND(p_offset<0);
	ERR_FAIL_INDEX(p_track,tracks.size());
	
	Track &t=*tracks[p_track];
	insert_key<Vector3>(t.scale_keys, p_offset, p_scale );
}

int Animation::get_value_frames_count(int p_track) const {

	ERR_FAIL_INDEX_V(p_track,tracks.size(), -1);
	return tracks[p_track]->value_keys.size();
};


Error Animation::get_value_track_state(int p_track, float p_time, float p_elapsed, ValueTrackState& r_state) {
	
	ERR_FAIL_COND_V(p_time<0, ERR_INVALID_PARAMETER);
	ERR_FAIL_INDEX_V(p_track,tracks.size(), ERR_INVALID_PARAMETER);

	Track &t=*tracks[p_track];

	r_state.has_frames = false;
	
	if (t.value_keys.size() == 0) {
		return OK;
	};

	float start_time = p_time - p_elapsed;
	p_time -= 0.000001;
	int start_frame;
	if (start_time < 0) {

		start_frame = get_key_idx(t.value_keys, length - start_time);
		//start_frame -= t.value_keys.size();
	} else {
		
		start_frame = get_key_idx(t.value_keys, start_time);
	};

	int end_frame = get_key_idx(t.value_keys, p_time);
	if (start_frame == -1) {
		start_frame = 0;
	};
	if (end_frame == -1) {
		end_frame = 0;
	};

	// check if event is between the times
	if (start_frame == end_frame) {
		float ofs = t.value_keys[start_frame].offset;
		//printf("times %f, %f, %f\n", start_time, ofs, p_time);
		if (start_time > ofs || p_time < ofs) {
			return OK;
		};
		++end_frame;
	};

	if (start_time > end_frame) {
		start_frame -= t.value_keys.size();
	};

	r_state.has_frames = true;
	r_state.start_frame = start_frame;
	r_state.end_frame = end_frame;
	
	//printf("returning count %i for %i, %i\n", r_state.count, start_frame, end_frame);
	return OK;
};

Animation::Value Animation::get_value_frame(int p_track, int p_frame) {

	ERR_FAIL_INDEX_V(p_track,tracks.size(), Value());

	Track &t=*tracks[p_track];
	ERR_FAIL_INDEX_V(p_frame, t.value_keys.size(), Value());

	return t.value_keys[p_frame].data;
};

float Animation::get_value_offset(int p_track, int p_frame) {

	ERR_FAIL_INDEX_V(p_track,tracks.size(), -1);

	Track &t=*tracks[p_track];
	ERR_FAIL_INDEX_V(p_track, t.value_keys.size(), -1);

	return t.value_keys[p_frame].offset;
};

Error Animation::get_track_state(int p_track,float p_time,TrackState& r_state) {
	
	ERR_FAIL_COND_V(p_time<0, ERR_INVALID_PARAMETER);
	ERR_FAIL_INDEX_V(p_track,tracks.size(), ERR_INVALID_PARAMETER);
		
	Track &t=*tracks[p_track];
	
	// handle location 
	{
		
		int idx = get_key_idx(t.location_keys,p_time);
		if (idx!=ERR_INVALID_IDX) {
			
			if (idx==-1)
				r_state.location=t.location_keys[0].data;
			else if (idx<(t.location_keys.size()-1)) {
				
				const LocationKey &a=t.location_keys[idx];
				const LocationKey &b=t.location_keys[idx+1];
				ERR_FAIL_COND_V(a.offset==b.offset,ERR_BUG);
				float c=(p_time-a.offset)/(b.offset-a.offset);
				
				r_state.location=Vector3::lerp(a.data,b.data,c);
				
			} else {
				
				r_state.location=t.location_keys[idx].data;
			}
			r_state.has_location=true;
			
		} else {
			
			r_state.has_location=false;

		}
	}
	
	// handle rotation 
	{
		
		int idx = get_key_idx(t.rotation_keys,p_time);
		if (idx!=ERR_INVALID_IDX) {
			
			if (idx==-1)
				r_state.rotation=t.rotation_keys[0].data;
			else if (idx<(t.rotation_keys.size()-1)) {
				
				const RotationKey &a=t.rotation_keys[idx];
				const RotationKey &b=t.rotation_keys[idx+1];
				ERR_FAIL_COND_V(a.offset==b.offset,ERR_BUG);
				float c=(p_time-a.offset)/(b.offset-a.offset);
				
				r_state.rotation.slerp(a.data,b.data,c);

				
			} else {
				
				r_state.rotation=t.rotation_keys[idx].data;
			}
			r_state.has_rotation=true;
			
		} else {
			
			r_state.has_rotation=false;

		}
	}
	
	// handle scale 
	{
		
		int idx = get_key_idx(t.scale_keys,p_time);
		if (idx!=ERR_INVALID_IDX) {
			
			if (idx==-1)
				r_state.scale=t.scale_keys[0].data;
			else if (idx<(t.scale_keys.size()-1)) {
				
				const ScaleKey &a=t.scale_keys[idx];
				const ScaleKey &b=t.scale_keys[idx+1];
				ERR_FAIL_COND_V(a.offset==b.offset,ERR_BUG);
				float c=(p_time-a.offset)/(b.offset-a.offset);
				
				r_state.scale=Vector3::lerp(a.data,b.data,c);
				
			} else {
				
				r_state.scale=t.scale_keys[idx].data;
			}
			r_state.has_scale=true;
			
		} else {
			
			r_state.has_scale=false;

		}
	}
			
	return OK;
}

bool Animation::has_track(String p_name) const {
	
	for (int i=0;i<tracks.size();i++) {
		if (tracks[i]->name==p_name)
			return true;
	}
	
	return false;
}

int Animation::get_track_count() const {
	
	return tracks.size();
	
}

String Animation::get_track_name(int p_track) const {
	
	ERR_FAIL_INDEX_V(p_track,tracks.size(),"");
	return tracks[p_track]->name;
	
}
int Animation::get_track_index(String p_name) const {
	
	for (int i=0;i<tracks.size();i++) {
		if (tracks[i]->name==p_name)
			return i;
	}
	
	return -1;
}

void Animation::set_loop(bool p_enabled) {
	
	loop=p_enabled;
}
bool Animation::has_loop() const {
	
	return loop;	
}

void Animation::set_length(float p_length) {
	
	ERR_FAIL_COND(p_length<0);
	
	length=p_length;
}
float Animation::get_length() const {
	
	return length;	
}

Animation::Animation() {
	
	loop=false;
	length=0;
	
}
Animation::~Animation() {
	
	clear();
}
