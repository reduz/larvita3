//
// C++ Interface: animation_node
//
// Description: 
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//  
//
#ifndef ANIMATION_NODE_H
#define ANIMATION_NODE_H

#include "scene/nodes/node.h"
#include "animation/animation_set.h"
#include "scene/nodes/skeleton_node.h"

/**
	@author ,,, <red@lunatea>
*/
class AnimationNode : public Node {

	IAPI_TYPE(AnimationNode,Node);
	IAPI_INSTANCE_FUNC( AnimationNode );
	
	IRef<AnimationSet> animation_set;
	
	enum {
	
		MAX_CHANNELS=4
	};
	
	
	struct TrackLink {
	
		enum Type {
		
			LINK_NONE, // couldn't link
			LINK_NODE, // link to node (alter pose matrix)
			LINK_BONE, // link to a bone
			LINK_PARAMETER // link to a parameter (set a parameter as a matrix)
		};	
				
		Vector3 loc;
		Quat rot;
		Vector3 scale;
				
		Type type;
		WeakRef<Node> node;
		WeakRef<SkeletonNode> skeleton_node;
		String param;
		int bone_idx;
	};
	
	Table<String,TrackLink> track_link_cache; // link animation tracks by name found in animation
	
	struct ChannelLink {
	
		Vector<TrackLink*> tracks;
		
	};
	
	Table<String,ChannelLink> channel_link_cache; // link animation tracks to track numbers
	
	
	struct QueueItem {
	
		ChannelLink *chanlink;
		String name;
		IRef<Animation> animation;
		float in_time;
		float blend_time;
		QueueItem() { in_time=0; blend_time=0; chanlink=0; }
	};
	
	struct Channel {
		
		List<QueueItem> queue; // queue of animations
		float time; // time of thec current animation
		float blending_time; // blending time, if active (>=0), blending is happening with the first 2 animations in the queue
		Channel() { blending_time = -1; time = 0; };
	};
	
	Channel channels[MAX_CHANNELS];
	
	float time_scale;
	
	String last_animation_name;

	void process_callback(Uint32 p_callback, float p_time);
	void process_animation(QueueItem *qi,float p_time,float p_interpolate);
	void process_events(QueueItem *qi,float p_time, float p_elapsed);
	void process(float p_time);
	
	
	TrackLink* link_up_track_name(String p_name, bool p_force = false);
	ChannelLink* link_up_animation(String p_name,Animation *p_animation, bool p_force = false);
	
	/** IAPI proxy */
	void _iapi_set(const String& p_path, const Variant& p_value);
	void _iapi_get(const String& p_path,Variant& p_ret_value);
	void _iapi_get_property_list( List<PropertyInfo>* p_list );
	void _iapi_get_property_hint( const String& p_path, PropertyHint& p_hint );
	
	void _iapi_get_method_list( List<MethodInfo>* p_list );
	void _iapi_call(const String& p_name, const List<Variant>& p_params,Variant& r_ret);
	
	
public:

	void set_animation_set(IRef<AnimationSet> p_anim_set);
	IRef<AnimationSet> get_animation_set() const;

	void play(String p_name,int p_channel=0);
	void queue(String p_name,float p_time=-1,int p_channel=0);

	void clear_queue(int p_channel=0);
	bool is_queued(String p_name, int p_channel=0);
	
	bool is_active(int p_channel=0) const;
	
	float get_animation_pos(int p_channel=0) const;
	String get_animation_name(int p_channel=0) const;
	
	void set_time_scale(float p_ratio);
	float get_time_scale() const;

	void set_current_animation_time(float p_time, int p_channel = 0);
	void force_animation(String p_name, int p_channel = 0);
	void redo_animation_link(String p_name);

	AnimationNode();
	~AnimationNode();

};

#endif
