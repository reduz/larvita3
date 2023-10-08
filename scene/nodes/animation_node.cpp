//
// C++ Implementation: animation_node
//
// Description: 
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "animation_node.h"
#include "scene/main/scene_main_loop.h"
#include "fileio/animation_loader.h"


void AnimationNode::set_animation_set(IRef<AnimationSet> p_anim_set) {

	animation_set = p_anim_set;	
};

IRef<AnimationSet> AnimationNode::get_animation_set() const {

	return animation_set;
};


void AnimationNode::_iapi_set(const String& p_path, const Variant& p_value) {

	if (p_path=="animation_set")
		animation_set=p_value.get_IOBJ();
	if (p_path=="time_scale")
		time_scale=p_value;

}
void AnimationNode::_iapi_get(const String& p_path,Variant& p_ret_value) {

	if (p_path=="animation_set")
		p_ret_value=IObjRef(animation_set);
	if (p_path=="time_scale")
		p_ret_value=time_scale;
	
}
void AnimationNode::_iapi_get_property_list( List<PropertyInfo>* p_list ) {

	p_list->push_back( IAPI::PropertyInfo( Variant::IOBJ, "animation_set" ) ); 
	p_list->push_back( IAPI::PropertyInfo( Variant::REAL, "time_scale" ) ); 
}

void AnimationNode::_iapi_get_property_hint( const String& p_path, PropertyHint& p_hint ) {

	if (p_path=="animation_set") {
	
		p_hint=PropertyHint( PropertyHint::HINT_IOBJ_TYPE, "AnimationSet" );
	}
	if (p_path=="time_scale") {
	
		p_hint=PropertyHint( PropertyHint::HINT_RANGE, "0.01,16,0.01" );
	}
	
}
void AnimationNode::_iapi_call(const String& p_name, const List<Variant>& p_params,Variant& r_ret) {

	if (p_name == "import_animation") {
		
		ERR_FAIL_COND(p_params.size() < 2);
		String path = p_params[1];

		String name;
		if (p_params[0].get_type() == Variant::NIL) {
			name = path;
			int slash = path.find_last("/");
			if (slash != -1) {
				name = path.substr(slash + 1, path.size() - (slash+1));
			};
			int dot = name.find_last(".");
			if (dot != -1) {
				name = name.substr(0, dot);
			};
		} else {
			name = p_params[0];
		};
		IRef<Animation> anim = AnimationLoader::load_animation(path);
		ERR_FAIL_COND( !anim );

		if (!animation_set)
			animation_set = memnew(AnimationSet);

		animation_set->add_animation(name, anim);

	} else if (p_name=="play") {
	
		ERR_FAIL_COND(p_params.size()<3);
		
		queue(p_params[0].get_string(),p_params[1].get_real(),p_params[2].get_int());
		
	} else if (p_name.find("play:")==0) {
			
		play(p_name.right(p_name.find(":")));	
	}

}

void AnimationNode::_iapi_get_method_list( List<MethodInfo>* p_list ) {

	MethodInfo add("import_animation");
	add.parameters.push_back( IAPI::MethodInfo::ParamInfo( Variant::STRING, "name") );
	add.parameters.push_back( IAPI::MethodInfo::ParamInfo( Variant::STRING, "path", IAPI::PropertyHint(IAPI::PropertyHint::HINT_FILE)) );
	p_list->push_back(add);
	
	MethodInfo mi("play");
	
	mi.parameters.push_back( IAPI::MethodInfo::ParamInfo( Variant::STRING, "name") );
	mi.parameters.push_back( IAPI::MethodInfo::ParamInfo( Variant::REAL, "in_time") );
	mi.parameters.push_back( IAPI::MethodInfo::ParamInfo( Variant::INT, "channel") );
	
	p_list->push_back(mi);
	
	if (animation_set.is_null())
		return;

	List<String> anim_names;
	
	animation_set->get_animation_list(&anim_names);
	anim_names.sort();
	
	List<String>::Iterator *I=anim_names.begin();
	
	while(I) {
	
		p_list->push_back( MethodInfo("play:"+I->get()));
		I=I->next();
	}

}

void AnimationNode::process_callback(Uint32 p_callback, float p_time) {
	
	process(p_time);
	
}

void AnimationNode::process_events(QueueItem *qi,float p_time, float p_elapsed) {
	
	Animation *a=qi->animation.ptr();
	ERR_FAIL_COND(!a);

	//if ((a->get_track_count()!=qi->chanlink->tracks.size())) {
	//	redo_animation_link(qi->name);
	//};
	ERR_FAIL_COND(a->get_track_count()!=qi->chanlink->tracks.size());

	for (int i=0; i<a->get_track_count(); i++) {
		
		
		TrackLink *tl=qi->chanlink->tracks[i];
		if (!tl)
			continue;
		if (tl->type==TrackLink::LINK_NONE)
			continue;

		Animation::ValueTrackState tstate;
		Error err = a->get_value_track_state(i, p_time + p_elapsed, p_elapsed, tstate);
		ERR_CONTINUE(err);
		if (!tstate.has_frames)
			continue;
		
		int count = a->get_value_frames_count(i);

		for (int j=tstate.start_frame; j<tstate.end_frame; j++) {

			Animation::Value val = a->get_value_frame(i, j>=0?j:count-j);

			switch(tl->type) {
			
				case TrackLink::LINK_NODE: {
					
					ERR_CONTINUE( tl->node.is_null() );
					((IAPI*)tl->node.ptr())->set( val.name, val.value );
					
				}; break;

				case TrackLink::LINK_PARAMETER: {

					ERR_CONTINUE( tl->node.is_null() );
					((IAPI*)tl->node.ptr())->set( tl->param, val.value );
					
				};

				default:
					ERR_CONTINUE(true);
				
			};
		};
	};
};

void AnimationNode::process_animation(QueueItem *qi,float p_time,float p_interpolate) {
	
	Animation *a=qi->animation.ptr();
	ERR_FAIL_COND(!a);

	ERR_FAIL_COND(a->get_track_count()!=qi->chanlink->tracks.size());
	
	for(int i=0;i<a->get_track_count();i++) {
	
		TrackLink *tl=qi->chanlink->tracks[i];
		if (!tl)
			continue;
		if (tl->type==TrackLink::LINK_NONE)
			continue;
			
		Animation::TrackState tstate;
		Error err = a->get_track_state(i,p_time,tstate);
		ERR_CONTINUE(err);
		
		
		if (!tstate.has_location && !tstate.has_rotation && !tstate.has_scale)
			continue;
		
		
		if (p_interpolate<1.0) {
		
			if (tstate.has_location)
				tl->loc=Vector3::lerp(tl->loc,tstate.location,p_interpolate);			
			if (tstate.has_rotation) {
				tl->rot.slerp(tl->rot,tstate.rotation,p_interpolate);		
				tl->rot.normalize();
			}	
			if (tstate.has_scale)
				tl->scale=Vector3::lerp(tl->scale,tstate.scale,p_interpolate);
			
		} else {
		
			if (tstate.has_location)
				tl->loc=tstate.location;
			if (tstate.has_rotation)
				tl->rot=tstate.rotation;
			if (tstate.has_scale)
				tl->scale=tstate.scale;
		}
		
		
		Matrix4 final;
		if (tstate.has_location) 
			final.set_translation(tl->loc);			
	
		if (tstate.has_rotation)
			final.rotate(tl->rot);
		
		if (tstate.has_scale)
			final.scale(tl->scale);

		if (tl->scale != Vector3(1, 1, 1)) {
			printf("scale for %ls is %ls\n", a->get_track_name(i).c_str(), Variant(tl->scale).get_string().c_str());
		};

		switch(tl->type) {
		
			case TrackLink::LINK_NODE: {
				ERR_CONTINUE( tl->node.is_null() );
//				tl->node->set_pose_matrix( final );
//				tl->node->set_global_matrix( final );
				
				
			} break;
			case TrackLink::LINK_BONE: {
			
				ERR_CONTINUE( tl->skeleton_node.is_null() );
				tl->skeleton_node->set_bone_pose( tl->bone_idx, final );
				
			
			} break;
			case TrackLink::LINK_PARAMETER: {
			
				ERR_CONTINUE( tl->node.is_null() );

				((IAPI*)tl->node.ptr())->set( tl->param, final );
				
			
			} break;
			default: {  }
		
		}
		
	}

}

void AnimationNode::process(float time_elapsed) {
	
	time_elapsed*=time_scale;
	for (int i=0;i<MAX_CHANNELS;i++) {
			
		Channel &c=channels[i];
		
		if (c.queue.empty())
			continue;
		
		QueueItem *qi = &c.queue.begin()->get();
		QueueItem *qi_blend = (c.queue.begin()->next())?(&c.queue.begin()->next()->get()):NULL;
		
		process_animation(qi,c.time,1.0);
		process_events(qi, c.time, time_elapsed);

				
		if (qi_blend) {
				

			///@TODO should do a loop in here to check if more animations passed by in the timeframe
			
			if (qi_blend->in_time>=0) { // check wether another animation is WAITING to kick in and blend
				
				
				qi_blend->in_time-=time_elapsed;
				
				if (qi_blend->in_time<0) { // animation ready to blend
					c.blending_time=qi_blend->blend_time-ABS(qi_blend->in_time); // compute started blending
					qi_blend->in_time=-1;
					

				}
					
			}  
			
			if (qi_blend->in_time<0) {
				
				float blend=0.0; // blend coefficient 0 means original, 1 means new 
				
				if (qi_blend->blend_time>0 && c.blending_time>=0) {
				
					blend=1.0-c.blending_time/qi_blend->blend_time;
					
				}
				
				if (blend>0) {// blend only if necesary
					process_animation(qi_blend,qi_blend->blend_time-c.blending_time,blend);
					process_events(qi_blend,qi_blend->blend_time-c.blending_time, time_elapsed);

				};
				
				c.blending_time-=time_elapsed;
				
	
				if (c.blending_time<0) {
	
					
					c.queue.pop_front();
					c.time=qi_blend->blend_time;
					c.blending_time=-1;
					continue;
				}
			}
		}
		
		c.time+=time_elapsed;

		//*
		if (c.time>=qi->animation->get_length()) {
		
			c.time=0;
		
			if (!qi->animation->has_loop()) {
			
				c.queue.pop_front();
			}
		}
		//	*/
	}
}

bool AnimationNode::is_active(int p_channel) const {
	
	ERR_FAIL_INDEX_V( p_channel, MAX_CHANNELS, false );
	
	return !channels[p_channel].queue.empty();	
}

AnimationNode::TrackLink* AnimationNode::link_up_track_name(String p_name, bool p_force) {

	ERR_FAIL_COND_V(p_name=="",NULL);
	
	if (!p_force && track_link_cache.has(p_name)) {
	
		return track_link_cache.getptr(p_name);
	}

	
	String path;
	String item;
	if (p_name.find(":")!=-1) {
		// sub-something
		item=p_name.get_slice(":",1);
		path=p_name.get_slice(":",0);	
	} else {
		path=p_name;
	}
	
	IRef<Node> node;
	if (!get_parent().is_null()) 
		node=get_parent()->get_node(path);
	
	
	TrackLink tl;
	if (node.is_null()) { // nothing ( broken link )
	
		tl.type=TrackLink::LINK_NONE;
		
	} else if (node->is_type("SkeletonNode") && item!="") { // skeleton
	
		IRef<SkeletonNode> sknode = node;
		
		int bone_idx=-1;
		
		for (int i=0;i<sknode->get_bone_count();i++) {
		
			if (sknode->get_bone_name(i)==item) {
			
				bone_idx=i;
				break;
			}
		}
		
		if (bone_idx==-1) { // bone not found :(
			

			tl.type=TrackLink::LINK_NONE;
		} else {
		
			tl.type=TrackLink::LINK_BONE;
			tl.skeleton_node=sknode;
			tl.bone_idx=bone_idx;
			
		}
	} else {
	
		if (item!="") { // link to a parameter

			tl.type=TrackLink::LINK_PARAMETER;
			tl.node=node;
			tl.param=item;
			
		} else { // link to a node
		
			tl.type=TrackLink::LINK_NODE;
			tl.node=node;
			
		
		}
	}
	
	
	track_link_cache[p_name]=tl;
	
	TrackLink *tlret = track_link_cache.getptr(p_name);
	
	ERR_FAIL_COND_V(!tlret,NULL);
	
	return tlret;
	
}

AnimationNode::ChannelLink* AnimationNode::link_up_animation(String p_name,Animation *p_animation, bool p_force) {


	if (!p_force && channel_link_cache.has(p_name)) {
		return &channel_link_cache[p_name];
	}

	/* animation is not here */

	if (!channel_link_cache.has(p_name)) {
		channel_link_cache[p_name]=ChannelLink();
	};

	ChannelLink &cl=channel_link_cache[p_name];
	
	cl.tracks.resize(p_animation->get_track_count());
	
	for (int i=0;i<p_animation->get_track_count();i++) {
	
		cl.tracks[i] = link_up_track_name(p_animation->get_track_name(i), p_force);
	}
	
	return &cl;
}

void AnimationNode::clear_queue(int p_channel) {

	ERR_FAIL_INDEX( p_channel, MAX_CHANNELS );

	Channel &c=channels[p_channel];
	
	if (c.queue.empty())
		return;
	while(c.queue.begin() != c.queue.back())
		c.queue.erase( c.queue.back() );
		
	c.blending_time=-1;	
	
};

void AnimationNode::play(String p_name,int p_channel) {

	clear_queue();	
	
	queue(p_name,0,p_channel);
	last_animation_name = p_name;
}

bool AnimationNode::is_queued(String p_name, int p_channel) {
	
	ERR_FAIL_INDEX_V( p_channel, MAX_CHANNELS, false );
	ERR_FAIL_COND_V( animation_set.is_null(), false );

	Channel &c=channels[p_channel];

	List<QueueItem>::Iterator* I = c.queue.begin();
	for (;I;I=I->next()) {

		if (I->get().name == p_name) {
			return true;
		};
	};
	
	return false;
};

void AnimationNode::queue(String p_name,float p_time,int p_channel) {

	ERR_FAIL_INDEX( p_channel, MAX_CHANNELS );
	ERR_FAIL_COND( animation_set.is_null() );

	IRef<Animation> anim = animation_set->get_animation(p_name);
	
	ERR_FAIL_COND( anim.is_null() ); // wrong animation name
		
	Channel &c=channels[p_channel];
	
	String from_name;
	if (!c.queue.empty()) {
	
		from_name = c.queue.back()->get().name;
	};
	
	if (p_time<0) {
		// minus one means to queue at playback end
		if (!c.queue.empty()) {
			
			if (c.queue.size()==1)
				p_time=c.queue.back()->get().animation->get_length() - c.time;
			else 
				p_time=c.queue.back()->get().animation->get_length();
			if (p_time<0)
				p_time=0;
		} else {
		
			p_time=0;
			c.blending_time=-1;
			c.time=0;
		}
	}
	
	QueueItem qi;
	qi.animation=anim;	
	qi.in_time=p_time;
	qi.name=p_name;
	qi.blend_time=from_name==""?0:animation_set->get_blend_time(from_name, p_name);

	/* may be the first time playing back this animation, make sure it's linked up */
	qi.chanlink=link_up_animation(p_name,qi.animation.ptr());
	ERR_FAIL_COND(!qi.chanlink);
	c.queue.push_back(qi);
}

void AnimationNode::redo_animation_link(String p_name) {

	IRef<Animation> anim = animation_set->get_animation(p_name);
	ERR_FAIL_COND(!anim);

	link_up_animation(p_name, anim.ptr(), true);
};

void AnimationNode::set_current_animation_time(float p_time, int p_channel) {
	

	ERR_FAIL_INDEX( p_channel, MAX_CHANNELS );

	Channel &c=channels[p_channel];
	if (c.queue.empty())
		return;
	
	QueueItem *qi = &c.queue.begin()->get();
	process_animation(qi,p_time,1.0);
	c.time = p_time;
};


void AnimationNode::force_animation(String p_name, int p_channel) {
	
	ERR_FAIL_INDEX( p_channel, MAX_CHANNELS );

	Channel &c=channels[p_channel];

	IRef<Animation> anim = animation_set->get_animation(p_name);
	ERR_FAIL_COND( anim.is_null() ); // wrong animation name

	clear_queue(p_channel);
	
	QueueItem qi;
	qi.animation=anim;	
	qi.in_time=0;
	qi.name=p_name;
	qi.blend_time=0;
	qi.chanlink=link_up_animation(p_name,qi.animation.ptr());
	ERR_FAIL_COND(!qi.chanlink);

	c.queue.push_front(qi);
};


float AnimationNode::get_animation_pos(int p_channel) const  {
	
	ERR_FAIL_INDEX_V(p_channel,MAX_CHANNELS,-2);
	
	if (channels[p_channel].queue.empty())
		return -1;
	if (channels[p_channel].blending_time >= 0 && channels[p_channel].queue.size() > 1) {
		
		return channels[p_channel].queue[1].blend_time - channels[p_channel].blending_time;
	} else {

		return channels[p_channel].time;
	};
}

String AnimationNode::get_animation_name(int p_channel) const  {
	
	ERR_FAIL_INDEX_V(p_channel,MAX_CHANNELS,"");
	
	if (!channels[p_channel].queue.empty()) {
		if (channels[p_channel].blending_time >= 0) {
	
			ERR_FAIL_COND_V(channels[p_channel].queue.size() < 2, "");
			((AnimationNode*)this)->last_animation_name = channels[p_channel].queue[1].name;
			
		} else {
			((AnimationNode*)this)->last_animation_name = channels[p_channel].queue.begin()->get().name;
		};
	};
	return last_animation_name;
	
}


void AnimationNode::set_time_scale(float p_ratio)  {
	
	time_scale=p_ratio;
}

float AnimationNode::get_time_scale() const {
	
	return time_scale;
}



AnimationNode::AnimationNode() {
	
	set_signal.connect( this, &AnimationNode::_iapi_set );
	get_signal.connect( this, &AnimationNode::_iapi_get );
	get_property_list_signal.connect( this, &AnimationNode::_iapi_get_property_list );
	get_property_hint_signal.connect( this, &AnimationNode::_iapi_get_property_hint );
	get_method_list_signal.connect( this, &AnimationNode::_iapi_get_method_list );
	call_signal.connect( this, &AnimationNode::_iapi_call );
	
	process_signal.connect(this,&AnimationNode::process_callback);
	set_process_flags( MODE_FLAG_EDITOR | MODE_FLAG_GAME | CALLBACK_FLAG_NORMAL  );
	
	time_scale = 1;
}


AnimationNode::~AnimationNode() {
	
}


