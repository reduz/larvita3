#include "scene/main/scene_main_loop.h"
#include "scene/nodes/particle_system_node.h"
#include "types/math/math_funcs.h"
#include "types/math/aabb.h"
#include "render/renderer.h"
#include "main/main.h"
//#include "scene/main/scene_main.h"

ParticleSystemNode::ParticleSystemNode()
{
	

	event_signal.connect(this, &ParticleSystemNode::_process_events);
	process_signal.connect( this, &ParticleSystemNode::_process_callback );

	for(int i = 0 ; i < MAX_EMITTERS ; i++) {

		this->systems[i].descriptor = -1;
	}
	this->system_descriptors.resize(0);

	this->a_particles.resize(0);
	this->live_particles = 0;
	this->set_max_particles(1024);

	this->live_emitters = 0;
	this->smallest_dead = 0;



	this->generate_static_volume();

	this->set_descriptors_number(1);

	set_signal.connect( this, &ParticleSystemNode::_set );
	get_signal.connect( this, &ParticleSystemNode::_get );
	get_property_list_signal.connect( this, &ParticleSystemNode::_get_property_list );
	get_property_hint_signal.connect( this, &ParticleSystemNode::_get_property_hint );

	set_process_flags( MODE_FLAG_EDITOR | MODE_FLAG_PAUSE| MODE_FLAG_GAME | CALLBACK_FLAG_NORMAL );

}

void ParticleSystemNode::_process_callback(Uint32 p_type, float p_time) {
	
	this->update(p_time);
}

void ParticleSystemNode::_process_events(EventType p_event)
{
	switch(p_event)
	{
		case EVENT_MATRIX_CHANGED: {
			this->base = this->get_global_matrix();
			max_size=Vector3();
			generate_static_volume();
			break; }
		default:
			break;
	}
}

ParticleSystemNode::~ParticleSystemNode()
{
	if(this->system_descriptors.size() > 0)
	{

		this->system_descriptors.resize(0);
	}
}

static inline double vel_sens_function(double p_velocity,double p_velocity_sens) { 

	double x; 
	x=Math::pow(4,p_velocity_sens); 
	if ( (p_velocity_sens>0.99) && (p_velocity>0.99) ) 
		return 1.0; 
	else 
		return Math::pow(p_velocity,x); 
} 

void ParticleSystemNode::draw(const SpatialRenderData& render_data)
{
	

	
	Renderer* render = Renderer::get_singleton();
	render->render_set_matrix(Renderer::MATRIX_WORLD,Matrix4());
	Matrix4 camera_matrix = SceneMainLoop::get_singleton()->get_current_camera()->get_global_matrix();
	//Matrix4 camera_matrix = render->render_get_matrix(Renderer::MATRIX_CAMERA);

	Vector3 to_cam(0,0,-1);
	camera_matrix.transform_no_translation(to_cam);
	Vector3 v_up(0,1,0);
	camera_matrix.transform_no_translation(v_up);
	Vector3 v_side(-1,0,0);
	camera_matrix.transform_no_translation(v_side);
	Vector3 v_front(0,0,1);
	camera_matrix.transform_no_translation(v_front);

	const Particle* part = &this->a_particles[0];
	
	//IRef<Material> material = render->get_indicator_material();
	//render->render_primitive_set_material( material ); 
				
	for(int i = 0 ; i < this->live_particles ; i++,part++)
	{
		if(part->descriptor >= 0)
		{
			const ParticleSystemDescriptor& descriptor = 
				this->system_descriptors[part->descriptor];

			Vector3 quad[4] = {part->v_location + (v_up - v_side)*part->f_size,
				part->v_location + (-v_up - v_side)*part->f_size,
				part->v_location + (-v_up + v_side)*part->f_size,
				part->v_location + ( v_up + v_side)*part->f_size
			};
			Vector3 normals[4] = {v_front,v_front,v_front,v_front};

			float int_factor = part->f_elapsed_time / (part->f_elapsed_time + part->f_energy);
			int_factor = vel_sens_function(int_factor,descriptor.initial_values.f_color_var);
			Color color;
			color.r = (int)(descriptor.initial_values.color_end.r * int_factor + 
				descriptor.initial_values.color_start.r*(1 - int_factor));
			color.g = (int)(descriptor.initial_values.color_end.g * int_factor + 
				descriptor.initial_values.color_start.g*(1 - int_factor));
			color.b = (int)(descriptor.initial_values.color_end.b * int_factor + 
				descriptor.initial_values.color_start.b*(1 - int_factor));
			color.a = (int)(descriptor.initial_values.color_end.a * int_factor + 
				descriptor.initial_values.color_start.a*(1 - int_factor));

			Color colors[4] = {color,color,color,color};
			float uv[8] = {0,0,
					0,1,
					1,1,
					1,0};
			if (descriptor.initial_values.r_material)
				render->render_primitive_set_material( descriptor.initial_values.r_material );
			else
				render->render_primitive_set_material( render->get_indicator_material() );

			if(descriptor.initial_values.r_texture)
				render->render_primitive_change_texture(descriptor.initial_values.r_texture);
;
			render->render_primitive(4,quad,normals,NULL,colors,uv);
		}
	}
}

void ParticleSystemNode::generate_static_volume() {

	Vector3 center=get_global_matrix().get_translation();

	Vector3 max=max_size;
	
	for( int i = 0 ; i < this->live_particles ; i++) {
	
		Vector3 p = this->a_particles[i].v_location-center;
		ERR_CONTINUE( i >= a_particles.size() );
		p=p.abs();
		
		if (p.x>max.x)
			max.x=p.x;
		if (p.y>max.y)
			max.y=p.y;
		if (p.z>max.z)
			max.z=p.z;
	}
		
	if (max==Vector3()) {
		Vector3 from = Vector3(-1, -1, -1);
		Vector3 size = Vector3(2, 2, 2);
		set_AABB(AABB(from,size));
	} else {
				
		max.x=1<<nearest_power_of_2( (int)(max.x+1.0) );
		max.y=1<<nearest_power_of_2( (int)(max.y+1.0) );
		max.z=1<<nearest_power_of_2( (int)(max.z+1.0) );
		
		if (max!=max_size) {
		
			return;
		}
		
		max_size=max;
		
		AABB aabb;
		aabb.size=max_size*2;
		aabb.pos=-max_size;
		set_AABB(aabb);
	}
}

AABB ParticleSystemNode::get_static_volume()
{
	return AABB(); //return this->aabb;
}

void ParticleSystemNode::update(float time) {

	ParticleSystem* sys = this->systems;
	for(int i = 0 ; i < MAX_EMITTERS ; i++, sys++) {

		if(this->systems[i].alive) {

			this->update_system(i,time);
			if( sys->f_time_remain < 0 && sys->live_particles <= 0 ) {

				this->remove_system(i);
				if(this->live_emitters <= 0 ) {

					// kill node
					//this->get_owner()->remove_child(this);
				}
			}
		}
	}

	Particle* part = &this->a_particles[0];

	for( int i = 0 ; i < this->live_particles ; i++, part++) {

		ParticleSystemDescriptor* descriptor = &this->system_descriptors[part->descriptor];
		if(!descriptor) {

			ERR_CONTINUE("WTF descriptor is NULL!\n");
		}
		ParticleSetting& setting = descriptor->initial_values;
		// update particle
		{
			if(!(setting.f_energy_min == 0 && setting.f_energy_max == 0)) {

				part->f_energy -= time;
				if(part->f_energy == 0)
					part->f_energy = -1;
			}
			part->f_elapsed_time += time;

			if(part->f_energy < 0) {

				// kill particle
				this->live_particles--;
				this->systems[part->emitter].live_particles--;

				Vector3 location = part->v_location;
				*part = this->a_particles[this->live_particles];
				if(setting.emit_when_dead >= 0 && 
				   setting.emit_when_dead <= this->get_descriptors_number()) {

					float lifetime = this->system_descriptors[setting.emit_when_dead].f_life;
					this->create_system(setting.emit_when_dead,location - 
										this->base.get_translation(),lifetime);
				}
				part--;
				i--;
			}
			else {//(part->f_energy > 0)
				if(!this->is_visible()) continue;

				Vector3 gravity(0,-9.8,0);
				Vector3 partial_accel, accel;

				accel = gravity * part->f_gravity;

				// calculate radial acceleration
				partial_accel = part->v_location - this->base.get_translation();
				partial_accel.normalize();
				partial_accel *= part->f_radial_accel;
				accel += partial_accel;

				//calculate tangential acceleration
				partial_accel.set_as_cross_of(part->v_velocity,gravity);
				partial_accel.normalize();
				partial_accel *= part->f_tang_accel;
				accel += partial_accel;

				part->v_velocity += accel * time;
				part->v_location += part->v_velocity * time;

				part->f_size += part->f_size_var*time;
				part->f_spin += part->f_spin_var*time;

/*				float int_factor = part->f_elapsed_time / (part->f_elapsed_time + part->f_energy);
				part->color.r = part->descriptor->color_end.r * int_factor + 
					part->descriptor->color_start.r*(1 - int_factor);
				part->color.g = part->descriptor->color_end.g * int_factor + 
					part->descriptor->color_start.g*(1 - int_factor);
				part->color.b = part->descriptor->color_end.b * int_factor + 
					part->descriptor->color_start.b*(1 - int_factor);
*/
			}
		}

	}
	
	generate_static_volume();
}

ParticleSystem::ParticleSystem() {

	this->alive = false;
	this->descriptor = -1;
	this->live_particles = 0;
	this->v_location = Vector3(0,0,0);
	this->f_emission_residue = 0;
	this->f_time_remain = -1.0;

}

ParticleSystem::ParticleSystem(int n_descriptor,
			Vector3 location,
		   	float lifetime) {

	this->alive = true;
	this->descriptor = n_descriptor;
	this->live_particles = 0;
	this->v_location = location;
	this->f_emission_residue = 0;
	this->f_time_remain = lifetime;
}

void ParticleSystemNode::create_system(int descriptor,const Vector3& location, float lifetime) {

	int i;
	for(i = this->smallest_dead ; i < MAX_EMITTERS ; i++) {

		if(!this->systems[i].alive) break;
	}
	this->smallest_dead = i;
	if(i < MAX_EMITTERS) {

		this->systems[i] = ParticleSystem(descriptor,location,lifetime);
		this->live_emitters++;
	}
}

void ParticleSystemNode::remove_system(int system) {

	if(this->systems[system].alive) {

		this->systems[system].alive = false;
		//this->systems[system].descriptor = -1;
		this->live_emitters--;
		this->smallest_dead = MIN(this->smallest_dead,system);
	}
}

void ParticleSystemNode::update_system(int n_system, float time) {

	ParticleSystem* system = this->systems + n_system;
	if(system->alive) {

		if( system->f_time_remain > 0) {

			system->f_time_remain -= time;
			if(system->f_time_remain <= 0) system->f_time_remain = -1;
		}
		if(!this->is_visible()) return;

		const Vector3 x_axis(1,0,0);
		const Vector3 y_axis(0,1,0);
		const Vector3 z_axis(0,0,1);

		if(system->f_time_remain >= 0) {

			// emit new particles
			ParticleSystemDescriptor& descriptor = this->system_descriptors[system->descriptor];
			float f_needed_particles = descriptor.f_emission_rate*time + system->f_emission_residue;
			int n_new_particles = (int)(f_needed_particles);
			system->f_emission_residue = f_needed_particles - n_new_particles;
			if(this->live_particles < a_particles.size()) {

				Particle* part = &this->a_particles[this->live_particles];
				const ParticleSetting& values = descriptor.initial_values;
				Vector3 v_x = this->base.get_x(),
						v_y = this->base.get_y(),
						v_z = this->base.get_z();

				for( int i = 0 ; i < n_new_particles && this->live_particles < this->a_particles.size() ; 
						i++, part++, this->live_particles++) {

					part->emitter = n_system;
					part->descriptor = system->descriptor;
					// set the position
					part->v_location = this->base.get_translation() + system->v_location;
					
					// and the velocity.
					float x1,y1,x2,y2;
					float angle = Math::random(-descriptor.f_spread,descriptor.f_spread);
					x1 = Math::sin(angle);
					y1 = Math::cos(angle);
					angle = Math::random(-descriptor.f_spread,descriptor.f_spread);
					x2 = Math::sin(angle);
					y2 = Math::cos(angle);

					part->v_velocity = v_y*y1 + v_x*x1;
					part->v_velocity = part->v_velocity*y2 + v_z*x2;

					part->v_velocity *= Math::random(values.f_speed_min,values.f_speed_max);

					// now the scalar values
					part->f_energy = Math::random(values.f_energy_min,values.f_energy_max);
					part->f_elapsed_time = 0;
					part->f_gravity = Math::random(values.f_gravity_min,values.f_gravity_max);
					part->f_radial_accel = Math::random(values.f_radial_acc - values.f_radial_acc_range, values.f_radial_acc + values.f_radial_acc_range);
					part->f_tang_accel = Math::random(values.f_tang_acc - values.f_tang_acc_range, values.f_tang_acc + values.f_tang_acc_range);

					// and the scalars with variation
					part->f_size = Math::random(values.f_size_start,		// a random number between the start
							values.f_size_start + 		// and the start plus the difference
								(values.f_size_end - values.f_size_start) * values.f_size_var); // times the variation

					part->f_spin = Math::random(values.f_spin_start,		// a random number between the start
							values.f_spin_start + 		// and the start plus the difference
								(values.f_spin_end - values.f_spin_start) * values.f_spin_var); // times the variation

					if(part->f_energy != 0) {

						part->f_size_var = (values.f_size_end - part->f_size) / part->f_energy;
						part->f_spin_var = (values.f_spin_end - part->f_spin) / part->f_energy;
					} else {

						part->f_size_var = 0;
						part->f_spin_var = 0;
					}
				}
			}
		}
	}
}

// iapi handlers
void ParticleSystemNode::_get_property_list(List<PropertyInfo>* p_ret_list) {

	p_ret_list->push_back( PropertyInfo(Variant::INT, "max_particles"));
	p_ret_list->push_back( PropertyInfo(Variant::INT, "descriptors_number"));

	String s_prop;
	for(int i = 0 ; i < this->get_descriptors_number(); i++) {

		String s_name = "systems/" + String::num(i) + "/";

		s_prop = s_name + "life";
		p_ret_list->push_back( PropertyInfo(Variant::REAL, s_prop));

		s_prop = s_name + "spread";
		p_ret_list->push_back( PropertyInfo(Variant::REAL, s_prop ));


		s_prop = s_name + "emission_rate";
		p_ret_list->push_back( PropertyInfo(Variant::REAL, s_prop ));

		s_prop = s_name + "energy_min";
		p_ret_list->push_back( PropertyInfo(Variant::REAL, s_prop ));
		s_prop = s_name + "energy_max";
		p_ret_list->push_back( PropertyInfo(Variant::REAL, s_prop ));

		s_prop = s_name + "speed_min";
		p_ret_list->push_back( PropertyInfo(Variant::REAL, s_prop ));
		s_prop = s_name + "speed_max";
		p_ret_list->push_back( PropertyInfo(Variant::REAL, s_prop ));

		s_prop = s_name + "gravity_min";
		p_ret_list->push_back( PropertyInfo(Variant::REAL, s_prop ));
		s_prop = s_name + "gravity_max";
		p_ret_list->push_back( PropertyInfo(Variant::REAL, s_prop ));

		s_prop = s_name + "radial_acc";
		p_ret_list->push_back( PropertyInfo(Variant::REAL, s_prop ));
		s_prop = s_name + "radial_acc_range";
		p_ret_list->push_back( PropertyInfo(Variant::REAL, s_prop ));

		s_prop = s_name + "tang_acc";
		p_ret_list->push_back( PropertyInfo(Variant::REAL, s_prop ));
		s_prop = s_name + "tang_acc_range";
		p_ret_list->push_back( PropertyInfo(Variant::REAL, s_prop ));

		s_prop = s_name + "size_start";
		p_ret_list->push_back( PropertyInfo(Variant::REAL, s_prop ));
		s_prop = s_name + "size_end";
		p_ret_list->push_back( PropertyInfo(Variant::REAL, s_prop ));
		s_prop = s_name + "size_var";
		p_ret_list->push_back( PropertyInfo(Variant::REAL, s_prop ));

		/*s_prop = s_name + "spin_start";
		p_ret_list->push_back( PropertyInfo(Variant::REAL, s_prop ));
		s_prop = s_name + "spin_end";
		p_ret_list->push_back( PropertyInfo(Variant::REAL, s_prop ));
		s_prop = s_name + "spin_var";
		p_ret_list->push_back( PropertyInfo(Variant::REAL, s_prop ));
*/
		s_prop = s_name + "texture";
		p_ret_list->push_back( PropertyInfo(Variant::IOBJ, s_prop ));

		s_prop = s_name + "color_start";
		p_ret_list->push_back( PropertyInfo(Variant::COLOR, s_prop ));
		s_prop = s_name + "color_end";
		p_ret_list->push_back( PropertyInfo(Variant::COLOR, s_prop ));
		s_prop = s_name + "color_var";
		p_ret_list->push_back( PropertyInfo(Variant::REAL, s_prop ));

		s_prop = s_name + "emit_when_dead";
		p_ret_list->push_back( PropertyInfo(Variant::INT, s_prop ));

		s_prop = s_name + "material";
		p_ret_list->push_back( PropertyInfo(Variant::IOBJ, s_prop ));
	}
}

void ParticleSystemNode::_get_property_hint(const String& p_path, PropertyHint& p_ret_value) {

	if(p_path == "descriptors_number") {

		p_ret_value = PropertyHint(IAPI::PropertyHint::HINT_RANGE, "0,16,1");
	}
	else if(p_path.find("/texture") == p_path.size() - ( (int)sizeof("/texture") - 1)) {

		p_ret_value = PropertyHint(IAPI::PropertyHint::HINT_IOBJ_TYPE, "Texture");
	}
	else if(p_path.find("/emit_when_dead") == p_path.size() - (sizeof("texture") - 1)) {

		p_ret_value = PropertyHint(IAPI::PropertyHint::HINT_RANGE, "-1,15,1");
	}
	else if (p_path.find("/radial_acc_range") != -1) {
		p_ret_value = PropertyHint(IAPI::PropertyHint::HINT_RANGE, "0,16,0.001");
	}
	else if (p_path.find("/radial_acc") != -1) {
		p_ret_value = PropertyHint(IAPI::PropertyHint::HINT_RANGE, "-16,16,0.001");
	}
	else if (p_path.find("/tang_acc_range") != -1) {
		p_ret_value = PropertyHint(IAPI::PropertyHint::HINT_RANGE, "0,16,0.001");
	}
	else if (p_path.find("/tang_acc") != -1) {
		p_ret_value = PropertyHint(IAPI::PropertyHint::HINT_RANGE, "-16,16,0.001");
	}
	else if (p_path.find("/material") != -1) {
		p_ret_value = PropertyHint(IAPI::PropertyHint::HINT_IOBJ_TYPE, "Material");
	};
}

void ParticleSystemNode::set_max_particles(int val) {

	if(val < 0) val = 0;

	this->a_particles.resize(val);
	this->live_particles = MIN(val,this->live_particles);
}

void ParticleSystemNode::set_descriptors_number(int val) {

	int old_size = this->system_descriptors.size();

	this->system_descriptors.resize(val);
	
	if(val > 0) {


		for( int i = old_size; i < val ; i++) {

			ParticleSystemDescriptor* descriptor = &this->system_descriptors[i];
			
			descriptor->f_life = 0;
			descriptor->f_spread = 0;
			descriptor->f_emission_rate = 10;
			descriptor->initial_values.f_energy_min = 1.3;
			descriptor->initial_values.f_energy_max = 1.4;
			descriptor->initial_values.f_speed_min = 3.0;
			descriptor->initial_values.f_speed_max = 3.5;
			descriptor->initial_values.f_gravity_min = 0;
			descriptor->initial_values.f_gravity_max = 0;
			descriptor->initial_values.f_radial_acc = -0.5;
			descriptor->initial_values.f_radial_acc_range = 0;
			descriptor->initial_values.f_tang_acc = 0;
			descriptor->initial_values.f_tang_acc_range = 0;
			descriptor->initial_values.f_size_start = 0.25;
			descriptor->initial_values.f_size_end = 0.14;
			descriptor->initial_values.f_size_var = 0;
			descriptor->initial_values.f_spin_start = 0;
			descriptor->initial_values.f_spin_end = 0;
			descriptor->initial_values.f_spin_var = 0;
			descriptor->initial_values.color_start = Color(255,255,255,255);
			descriptor->initial_values.color_end = Color(255,255,255,255);
			descriptor->initial_values.f_color_var = 0;
			descriptor->initial_values.r_texture = IRef<Texture>();
			descriptor->initial_values.emit_when_dead = -1;
		}
		if(this->live_emitters <= 0) {

			this->create_system(0,Vector3(0,0,0),0.0);
		}
		Particle* part = &this->a_particles[0];
		for(int i = 0 ; i < this->live_particles ; i++, part++) {

			if(part->descriptor >= val) {

				*part = this->a_particles[this->live_particles--];
				i--;
				part--;
			}
		}
	} else {

		for(int i = 0 ; i < MAX_EMITTERS ; i++)
			this->remove_system(i);
	}

	this->property_changed_signal.call("");
}

void ParticleSystemNode::_set(const String& p_path, const Variant& p_value) {

	if (p_path == "descriptors_number") { this->set_descriptors_number(p_value); }
	else if(p_path == "max_particles") { this->set_max_particles(p_value); }

	else if(p_path.find("systems/") == 0) {

		int sys = p_path.get_slice("/",1).to_int();
		ERR_FAIL_INDEX(sys,this->get_descriptors_number());

		String path = p_path.get_slice("/",2);
		// the third "word" should be the system's index
		if(path =="life") { this->set_life(sys,p_value); }
		else if(path == "spread") { this->set_spread_angle(sys,p_value); }

		else if(path == "emission_rate") { this->set_emission_rate(sys,p_value); }

		else if(path == "energy_min") { this->set_energy_min(sys,p_value); }
		else if(path == "energy_max") { this->set_energy_max(sys,p_value); }

		else if(path == "speed_min") { this->set_speed_min(sys,p_value); }
		else if(path == "speed_max") { this->set_speed_max(sys,p_value); }

		else if(path == "gravity_min") { this->set_gravity_min(sys,p_value); }
		else if(path == "gravity_max") { this->set_gravity_max(sys,p_value); }

		else if(path == "radial_acc") { this->system_descriptors[sys].initial_values.f_radial_acc = p_value; }
		else if(path == "radial_acc_range") { this->system_descriptors[sys].initial_values.f_radial_acc_range = p_value; }

		else if(path == "tang_acc") { this->system_descriptors[sys].initial_values.f_tang_acc = p_value; }
		else if(path == "tang_acc_range") { this->system_descriptors[sys].initial_values.f_tang_acc_range = p_value; }

		else if(path == "size_start") { this->set_particle_size_start(sys,p_value); }
		else if(path == "size_end") { this->set_particle_size_end(sys,p_value); }
		else if(path == "size_var") { this->set_particle_size_var(sys,p_value); }

		else if(path == "spin_start") { this->set_particle_spin_start(sys,p_value); }
		else if(path == "spin_end") { this->set_particle_spin_end(sys,p_value); }
		else if(path == "spin_var") { this->set_particle_spin_var(sys,p_value); }

		else if(path == "color_start") { this->set_particle_color_start(sys,p_value); }
		else if(path == "color_end") { this->set_particle_color_end(sys,p_value); }
		else if(path == "color_var") { this->set_particle_color_var(sys,p_value); }

		else if(path == "texture") { this->set_texture(sys,p_value); }
		else if(path == "emit_when_dead") { this->set_emit_when_dead(sys,p_value); }
		else if (path == "material") { 	this->system_descriptors[sys].initial_values.r_material = p_value.get_IOBJ(); }

		//else if(path == "material") { this->set_particle_material(sys); }
		//else if(path == "emit_when_dead") { this->set_particle_after_emitter(sys); }

		else { ERR_PRINT(path.ascii().get_data()); return; }

	}
}

void ParticleSystemNode::_get(const String& p_path, Variant& p_ret) {

	if (p_path == "descriptors_number") { p_ret = this->get_descriptors_number(); }
	else if (p_path == "max_particles") { p_ret = this->get_max_particles(); }

	else if(p_path.find("systems/")==0) {

		int sys = p_path.get_slice("/",1).to_int();
		ERR_FAIL_INDEX(sys,this->get_descriptors_number());

		String path = p_path.get_slice("/",2);
		// the second "word" should be the system's index
		if(path == "life") { 

			p_ret = this->get_life(sys); 
		}
		else if(path == "spread") { p_ret = this->get_spread_angle(sys); }

		else if(path == "emission_rate") { p_ret = this->get_emission_rate(sys); }

		else if(path == "energy_min") { p_ret = this->get_energy_min(sys); }
		else if(path == "energy_max") { p_ret = this->get_energy_max(sys); }

		else if(path == "speed_min") { p_ret = this->get_speed_min(sys); }
		else if(path == "speed_max") { p_ret = this->get_speed_max(sys); }

		else if(path == "gravity_min") { p_ret = this->get_gravity_min(sys); }
		else if(path == "gravity_max") { p_ret = this->get_gravity_max(sys); }

		else if(path == "radial_acc") { p_ret = this->system_descriptors[sys].initial_values.f_radial_acc; }
		else if(path == "radial_acc_range") { p_ret = this->system_descriptors[sys].initial_values.f_radial_acc_range; }

		else if(path == "tang_acc") { p_ret = this->system_descriptors[sys].initial_values.f_tang_acc; }
		else if(path == "tang_acc_range") { p_ret = this->system_descriptors[sys].initial_values.f_tang_acc_range; }

		else if(path == "size_start") { p_ret = this->get_particle_size_start(sys); }
		else if(path == "size_end") { p_ret = this->get_particle_size_end(sys); }
		else if(path == "size_var") { p_ret = this->get_particle_size_var(sys); }

		else if(path == "spin_start") { p_ret = this->get_particle_spin_start(sys); }
		else if(path == "spin_end") { p_ret = this->get_particle_spin_end(sys); }
		else if(path == "spin_var") { p_ret = this->get_particle_spin_var(sys); }

		else if(path == "color_start") { p_ret = this->get_particle_color_start(sys); }
		else if(path == "color_end") { p_ret = this->get_particle_color_end(sys); }
		else if(path == "color_var") { p_ret = this->get_particle_color_var(sys); }

		else if(path == "texture") { p_ret = (IObjRef)this->get_texture(sys); }
		else if(path == "material") { p_ret = (IObjRef)this->system_descriptors[sys].initial_values.r_material; }
		else if(path == "emit_when_dead") { p_ret = this->get_emit_when_dead(sys); }

		//else if(path == "material") { p_ret = this->get_particle_material(sys); }
		//else if(path == "emit_when_dead") { p_ret = this->get_particle_after_emitter(sys); }

		//else { ERR_PRINT(path.ascii().get_data()); return; }

	}
}

float ParticleSystemNode::get_spread_angle(int sys) const {

	return this->system_descriptors[sys].f_spread;
}

float ParticleSystemNode::get_emission_rate(int sys) const {

	return this->system_descriptors[sys].f_emission_rate;
}

float ParticleSystemNode::get_energy_min(int sys) const {

	return this->system_descriptors[sys].initial_values.f_energy_min;
}

float ParticleSystemNode::get_energy_max(int sys) const {

	return this->system_descriptors[sys].initial_values.f_energy_max;
}

float ParticleSystemNode::get_speed_min(int sys) const {

	return this->system_descriptors[sys].initial_values.f_speed_min;
}

float ParticleSystemNode::get_speed_max(int sys) const {

	return this->system_descriptors[sys].initial_values.f_speed_max;
}

float ParticleSystemNode::get_gravity_min(int sys) const {

	return this->system_descriptors[sys].initial_values.f_gravity_min;
}

float ParticleSystemNode::get_gravity_max(int sys) const {

	return this->system_descriptors[sys].initial_values.f_gravity_max;
}

float ParticleSystemNode::get_particle_size_start(int sys) const {

	return this->system_descriptors[sys].initial_values.f_size_start;
}

float ParticleSystemNode::get_particle_size_end(int sys) const {

	return this->system_descriptors[sys].initial_values.f_size_end;
}

float ParticleSystemNode::get_particle_size_var(int sys) const {

	return this->system_descriptors[sys].initial_values.f_size_var;
}

float ParticleSystemNode::get_particle_spin_start(int sys) const {

	return this->system_descriptors[sys].initial_values.f_spin_start;
}

float ParticleSystemNode::get_particle_spin_end(int sys) const {

	return this->system_descriptors[sys].initial_values.f_spin_end;
}

float ParticleSystemNode::get_particle_spin_var(int sys) const {

	return this->system_descriptors[sys].initial_values.f_spin_var;
}

Color ParticleSystemNode::get_particle_color_start(int sys) const {

	return this->system_descriptors[sys].initial_values.color_start;
}

Color ParticleSystemNode::get_particle_color_end(int sys) const {

	return this->system_descriptors[sys].initial_values.color_end;
}

float ParticleSystemNode::get_particle_color_var(int sys) const {

	return this->system_descriptors[sys].initial_values.f_color_var;
}

IRef<Texture> ParticleSystemNode::get_texture(int sys) const {

	return this->system_descriptors[sys].initial_values.r_texture;
}

float ParticleSystemNode::get_life(int sys) const {

	return this->system_descriptors[sys].f_life;
}

float ParticleSystemNode::get_emit_when_dead(int sys) const {

	return this->system_descriptors[sys].initial_values.emit_when_dead;
}


/*float ParticleSystemNode::get_particle_material(int sys) const
{
}

float ParticleSystemNode::get_particle_after_emitter(int sys) const
{
}*/

void ParticleSystemNode::set_spread_angle(int sys, const Variant& val) {

	this->system_descriptors[sys].f_spread = val;
}

void ParticleSystemNode::set_emission_rate(int sys, const Variant& val) {

	this->system_descriptors[sys].f_emission_rate = val;
}

void ParticleSystemNode::set_energy_min(int sys, const Variant& val) {

	this->system_descriptors[sys].initial_values.f_energy_min = val;
}

void ParticleSystemNode::set_energy_max(int sys, const Variant& val) {

	this->system_descriptors[sys].initial_values.f_energy_max = val;
}

void ParticleSystemNode::set_speed_min(int sys, const Variant& val) {

	this->system_descriptors[sys].initial_values.f_speed_min = val;
}

void ParticleSystemNode::set_speed_max(int sys, const Variant& val) {

	this->system_descriptors[sys].initial_values.f_speed_max = val;
}

void ParticleSystemNode::set_gravity_min(int sys, const Variant& val) {

	this->system_descriptors[sys].initial_values.f_gravity_min = val;
}

void ParticleSystemNode::set_gravity_max(int sys, const Variant& val) {

	this->system_descriptors[sys].initial_values.f_gravity_max = val;
}

void ParticleSystemNode::set_particle_size_start(int sys, const Variant& val) {

	this->system_descriptors[sys].initial_values.f_size_start = val;
}

void ParticleSystemNode::set_particle_size_end(int sys, const Variant& val) {

	this->system_descriptors[sys].initial_values.f_size_end = val;
}

void ParticleSystemNode::set_particle_size_var(int sys, const Variant& val) {

	this->system_descriptors[sys].initial_values.f_size_var = val;
}

void ParticleSystemNode::set_particle_spin_start(int sys, const Variant& val) {

	this->system_descriptors[sys].initial_values.f_spin_start = val;
}

void ParticleSystemNode::set_particle_spin_end(int sys, const Variant& val) {

	this->system_descriptors[sys].initial_values.f_spin_end = val;
}

void ParticleSystemNode::set_particle_spin_var(int sys, const Variant& val) {

	this->system_descriptors[sys].initial_values.f_spin_var = val;
}

void ParticleSystemNode::set_particle_color_start(int sys, const Variant& val) {

	this->system_descriptors[sys].initial_values.color_start = val.get_color();
}

void ParticleSystemNode::set_particle_color_end(int sys, const Variant& val) {

	this->system_descriptors[sys].initial_values.color_end = val.get_color();
}

void ParticleSystemNode::set_particle_color_var(int sys, const Variant& val) {

	this->system_descriptors[sys].initial_values.f_color_var = val;
}

void ParticleSystemNode::set_texture(int sys, const Variant& val)
{
	this->system_descriptors[sys].initial_values.r_texture = val.get_IOBJ();
}

void ParticleSystemNode::set_life(int sys, const Variant& val) {

	this->system_descriptors[sys].f_life = val.get_real();
	this->systems[0] = ParticleSystem(0);
	for(int i = 0 ; i < MAX_EMITTERS ; i++) {

		if(this->systems[i].descriptor == sys) {

			this->systems[i].f_time_remain = val.get_real();
			this->systems[i].alive = true;
		}
	}
}

void ParticleSystemNode::set_emit_when_dead(int sys, const Variant& val) {

	this->system_descriptors[sys].initial_values.emit_when_dead = val;
}

