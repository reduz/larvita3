//
// C++ Interface: particle system
//
// Description: classes and helpers for the particle systems
//
//
// Author: Tomas Neme <lacrymology@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef PARTICLE_SYSTEM_NODE_H
#define PARTICLE_SYSTEM_NODE_H

#include "iapi_layer.h"
#include "scene/nodes/node.h"
#include "types/math/vector3.h"
#include "collision/volume.h"
#include "render/renderer.h"
#include "render/spatial_indexer.h"
#include "types/vector.h"
#include "scene/nodes/visual_node.h"
#define MAX_EMITTERS 16

/**
 * The particle system will work as follows:
 * 	A particle system will have a number of ParticleSetting, and a number of
 * 		particles
 **/

struct ParticleSetting
{

	float f_energy_min;		// total lifetime of the particles
	float f_energy_max;		// total lifetime of the particles

	float f_speed_min;		// the minimun speed the particles will start with
	float f_speed_max;		// the maximun speed the particles will start with

	float f_gravity_min;	// this defines the min strength of the gravity on the particles
	float f_gravity_max;	// this defines the max strength of the gravity on the particles

	float f_radial_acc;	// this defines the radial acceleration on the particles
	float f_radial_acc_range;	// this defines the radial acceleration  random range on the particles


	// the tangential acceleration is calculated using the gravity vector
	float f_tang_acc;	// this defines the min tangential acceleration on the particles
	float f_tang_acc_range;	// this defines the max tangential acceleration on the particles

	float f_size_start;		// particles start size
	float f_size_end;		// particles will end at this size
	float f_size_var;		// particles will actually start at some random size
							// between f_start_size and 
							// f_start_size + (f_end_size - f_start_size)*f_size_var

	float f_spin_start;		// particles will start spinning at this angular speed
	float f_spin_end;		// particles will die spinning at this angular speed
	float f_spin_var;		// same as f_size_var

	Color color_start;	// the particle will transition between these colors in it's lifespan
	Color color_end;
	float f_color_var;		// same as f_size_var

	IRef<Texture> r_texture;
	IRef<Material> r_material;
	//Renderer::UV uv_begin, uv_end;

	int emit_when_dead;		// the index in the system node's systems array which will be emmitted
							// when this particle dies

};

struct ParticleSystemDescriptor
{
	float f_life;						// how long will the particle system emit for. 0 is infinite
	float f_spread;						// the initial velocity will have a max angle of f_spread from direction
	ParticleSetting initial_values;		// these are the settings used to calculate the initial
										// and common values of the emitted particles. yay
	float f_emission_rate;
};

struct Particle
{
	int emitter;	// the index in the particle system's emitters
	int descriptor;

	Vector3 v_location;		// particle position in system local coordinates
	Vector3 v_velocity;		// particle velocity

	float f_energy;			// life
	float f_elapsed_time;
	float f_gravity;
	float f_radial_accel;
	float f_tang_accel;
	float f_size;
	float f_size_var;		// this is the variation per second of the particle size
	float f_spin;
	float f_spin_var;		// this is the variation per second of the particle spin speed
	//Color color;

};

struct ParticleSystem
{
	bool alive;
	int live_particles;
	int descriptor;		// the descriptor in the parent node
	Vector3 v_location;					// the location of the system in node-coordinates
	float f_emission_residue;			// "partially emitted" particles. See update_system
	float f_time_remain;				// the system continues to emit until this is <= 0

	ParticleSystem();
	ParticleSystem(int n_descriptor,Vector3 location=Vector3(0,0,0), float lifetime=0.0);
};

class ParticleSystemNode : public VisualNode 
{
	IAPI_TYPE(ParticleSystemNode,VisualNode);
	IAPI_INSTANCE_FUNC(ParticleSystemNode);

public:
	ParticleSystemNode();
	~ParticleSystemNode();
	// iapi handlers and iapi-
	void _get_property_list(List<PropertyInfo>* p_ret_list);
	void _get_property_hint(const String& p_path, PropertyHint& p_ret_value);
	void _set(const String& p_path, const Variant& p_value);
	void _get(const String& p_path, Variant& p_ret);
	void _process_events(EventType p_event);
	AABB get_static_volume();
	virtual void draw(const SpatialRenderData& p_data);	
	void _process_callback(Uint32 p_type, float p_time);

private:
	Vector<Particle> a_particles;// a pointer to the array of particles
	int live_particles;
	ParticleSystem systems[MAX_EMITTERS];
	int live_emitters;
	int smallest_dead;
	Vector<ParticleSystemDescriptor> system_descriptors;
	Matrix4 base;



	Vector3 max_size;
	void generate_static_volume();
	void update(float time);
	void update_system(int sys, float time);

	void create_particles(ParticleSystem& sys,int n_particles);
	void create_system(int descriptor, const Vector3& v_location, float lifetime);
	void remove_system(int system);

	inline int get_descriptors_number() { return this->system_descriptors.size(); }
	inline int get_max_particles() { return this->a_particles.size(); }

	float get_spread_angle(int sys)const;
	float get_life(int sys)const;

	float get_emission_rate(int sys)const;

	float get_energy_min(int sys) const;
	float get_energy_max(int sys) const;

	float get_speed_min(int sys) const;
	float get_speed_max(int sys) const;

	float get_gravity_min(int sys) const;
	float get_gravity_max(int sys) const;

	float get_radial_acc_min(int sys) const;
	float get_radial_acc_max(int sys) const;

	float get_tangential_acc_min(int sys) const;
	float get_tangential_acc_max(int sys) const;

	float get_particle_size_start(int sys) const;
	float get_particle_size_end(int sys) const;
	float get_particle_size_var(int sys) const;

	float get_particle_spin_start(int sys) const;
	float get_particle_spin_end(int sys) const;
	float get_particle_spin_var(int sys) const;

	Color get_particle_color_start(int sys) const;
	Color get_particle_color_end(int sys) const;
	float get_particle_color_var(int sys) const;

	float get_emit_when_dead(int sys) const;

	IRef<Texture> get_texture(int sys) const;

	//get_particle_material(int sys);
	//get_particle_after_emitter(int sys);

	void set_descriptors_number(int);
	void set_max_particles(int);

	void set_life(int sys, const Variant& val);

	void set_spread_angle(int sys, const Variant& val);

	void set_emission_rate(int sys, const Variant& val);

	void set_energy_min(int sys, const Variant& val);
	void set_energy_max(int sys, const Variant& val);

	void set_speed_min(int sys, const Variant& val);
	void set_speed_max(int sys, const Variant& val);

	void set_gravity_min(int sys, const Variant& val);
	void set_gravity_max(int sys, const Variant& val);

	void set_radial_acc_min(int sys, const Variant& val);
	void set_radial_acc_max(int sys, const Variant& val);

	void set_tangential_acc_min(int sys, const Variant& val);
	void set_tangential_acc_max(int sys, const Variant& val);

	void set_particle_size_start(int sys, const Variant& val);
	void set_particle_size_end(int sys, const Variant& val);
	void set_particle_size_var(int sys, const Variant& val);

	void set_particle_spin_start(int sys, const Variant& val);
	void set_particle_spin_end(int sys, const Variant& val);
	void set_particle_spin_var(int sys, const Variant& val);

	void set_particle_color_start(int sys, const Variant& val);
	void set_particle_color_end(int sys, const Variant& val);
	void set_particle_color_var(int sys, const Variant& val);

	void set_emit_when_dead(int sys, const Variant& val);

	void set_texture(int sys, const Variant& val);

	//set_particle_material(int sys);
	//set_particle_after_emitter(int sys);
};

#endif
