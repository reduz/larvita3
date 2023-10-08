//
// C++ Implementation: character_body
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "character_body.h"

#include "rigid_body.h"
#include "os/memory.h"
#include "physics_world_settings.h" 
 
Uint32 CharacterBody::get_flags() const {

  if (influence) {

		return FLAG_INFLUENCE;
	} else {
		return FLAG_DYNAMIC;
	};
}
 
 
bool CharacterBody::is_collidable() const {
	
	return collidable;
}

void CharacterBody::set_collidable(bool p_active) {
	
	collidable=p_active;
}

void CharacterBody::set_influence(bool p_inf) {
	
	influence = p_inf;
};

bool CharacterBody::is_influence() {
	
	return influence;
};

 
float CharacterBody::get_friction() const {

	return 0.0;
}
float CharacterBody::get_bounce() const {

	return 0;

}
float CharacterBody::get_inv_mass() const {

	return inv_mass;
}
Matrix4 CharacterBody::get_inv_inertia_tensor() const {

	Matrix4 z;
	z.load_zero();
	return z;
	return inv_inertia_tensor;
}

Matrix4 CharacterBody::get_transform() const {

	return transform;
}

void CharacterBody::set_transform(const Matrix4& p_transform)  {

	pos=p_transform.get_translation();
	transform.set_translation(pos);
}
	
IRef<Volume> CharacterBody::get_volume() const {

	return volume;	
}

AABB CharacterBody::get_aabb() const {

	return volume->compute_AABB( transform );
}

void CharacterBody::notify_friction_request(bool p_request) {
	
	friction_request=p_request;
}

Vector3 CharacterBody::get_linear_velocity() const {

	return linear_velocity;
}
Vector3 CharacterBody::get_angular_velocity() const {

	return Vector3();
}

float CharacterBody::get_walk_velocity() const {
	
	return walk_speed;
}

void CharacterBody::apply_impulse(const Vector3& p_pos, const Vector3& p_dir) {

	
	linear_velocity+=p_dir*inv_mass;
	//printf("impulse %f,%f,%f\n",p_dir.x,p_dir.y,p_dir.z);	
	if (inv_mass==0)
		return;
/*			
	Vector3 dir=p_dir;
	dir.normalize();
	
	if (dir.dot(-gn)>0.5) {
		on_floor=true;
	}
*/	
}

/**
* For objects that don't do any physics, only serve for colliding against it
*/

bool CharacterBody::is_pure_collision() const {

	return false;
}
/************************************************
 ************************************************/
 


void CharacterBody::recreate_inertia_tensor() {


	Matrix4 n = transform;
	n.clear_translation();
	inv_inertia_tensor=n;
	inv_inertia_tensor.scale(inv_inertia_local);
	n.transpose();
	inv_inertia_tensor = n * inv_inertia_tensor;
		
}

void CharacterBody::set_walk_accel(float p_walk_accel) {
	
	walk_accel=p_walk_accel;
}
void CharacterBody::set_walk_damp(float p_walk_damp) {
	
	walk_damp=p_walk_damp;
}
void CharacterBody::set_turn_speed(float p_turn_speed) {
	
	turn_speed=p_turn_speed;
}
void CharacterBody::set_jump_impulse(float p_jump_impulse) {
	
	jump_impulse=p_jump_impulse;
}

void CharacterBody::set_jump_accel_dump(float p_jump_accel_damp) {

	jump_accel_damp = p_jump_accel_damp;
};


void CharacterBody::set_jump_damp(float p_jump_damp) {
	
	jump_damp=p_jump_damp;
}



float CharacterBody::get_max_walk_velocity() const {
	
	return max_walk_speed;
}
float CharacterBody::get_walk_accel() const {
	
	return walk_accel;
}
float CharacterBody::get_walk_damp() const {
	
	return walk_damp;
}
float CharacterBody::get_turn_speed() const {
	
	return turn_speed;
}
float CharacterBody::get_jump_impulse() const {
	
	return jump_impulse;
}


void CharacterBody::set_facing_vector(const Vector3& p_dir) {
	
	facing_dir=p_dir;
	transform.set_lookat( transform.get_translation(), transform.get_translation()+p_dir, -gn );
}

Vector3 CharacterBody::get_facing_vector() const {
	
	return facing_dir;
}

float CharacterBody::get_jump_damp() const {
	
	return jump_damp;
}

void CharacterBody::fly(float p_time) {

	float vertical_speed = -this->get_linear_velocity().dot(this->gn);
	const Vector3 vertical_velocity = this->gn * vertical_speed;
	Vector3 g = get_world_settings()->get_gravity();

	const float max_vel = 4;

	if(vertical_speed < max_vel + jump_impulse) {

		if(vertical_speed < -max_vel) {

			linear_velocity += gn*vertical_speed*0.4;
		} else {

			linear_velocity += gn*(-1)*
				(jump_impulse*2 + (max_vel - vertical_speed))*p_time*inv_mass - g*p_time;
		}
	}
}

void CharacterBody::process_jump(float p_time) {
	
	if(hover&&jump) {

		fly(p_time);
		return;
	}
		
	if (!jumping)
		return;

	if ((is_on_floor() && !jump) || gn.dot(linear_velocity)>=0) {
	
		jumping=false;
					
	} else if(!jump) {
		// if the jump has ended, we force it downwards faster
		Vector3 g=get_world_settings()->get_gravity();
		linear_velocity += g*3*p_time;
	}
}


void CharacterBody::set_jump(bool p_jump) {
		
	if(hover) {

		jump = p_jump;
		return;
	}
	if (p_jump==jump)
		return;
	
	if (!get_world_settings()) {
		return;
	
	} else { 
	
		
		if (!jump && !jumping && is_on_floor()) {
			
			//Vector3 gn = get_world_settings()->get_gravity().get_normalized();
			
			linear_velocity+=gn * -jump_impulse * inv_mass;
			jumping=true;
			jump=true;
		
		} else {
			
			jump=false;
		}
	}
}

bool CharacterBody::is_jump() const {
	
	return jump;
};

void CharacterBody::set_hover(bool p_hover) {

	if (p_hover == hover)
		return;

	hover=p_hover;
}

void CharacterBody::set_gravity_factor(float p_factor) {
	
	gravity_factor = p_factor;
}

float CharacterBody::get_gravity_factor() {

	return gravity_factor;
}

bool CharacterBody::is_going_up() const {
	
	if (!get_world_settings())
		return false;
	return get_linear_velocity().dot(get_world_settings()->get_gravity()) < 0;
}
	
bool CharacterBody::is_on_floor() const {
	
	return last_on_floor_time < airborne_min;
}

void CharacterBody::process_floor(float p_time) {
	
	if (on_floor) {
		last_on_floor_time=0;
		on_floor_frames++;
		if (max_walk_speed==0 && on_floor_frames>20)
			gn/=20;
		on_floor=false;
	} else {
		on_floor_frames=0;
		last_on_floor_time+=p_time;		
	}
		
}


void CharacterBody::set_target_dir(const Vector3& p_normal) {

	target_normal=p_normal;
}

Vector3 CharacterBody::get_target_dir() const {
	
	return target_normal;
}

void CharacterBody::adjust_turn_velocity(Vector3 &p_h_linear_vel,const Vector3 &p_target_dir,float p_time,bool p_deaccel_sudden_turns) {

		
	if (p_h_linear_vel==Vector3() || max_walk_speed<CMP_EPSILON) {
		
		return;
	}
	
	Vector3 n = p_target_dir; // normal
	Vector3 t = Vector3::cross(n,get_world_settings()->get_gravity()).get_normalized();
	
	float x = n.dot(p_h_linear_vel);
	float y = t.dot(p_h_linear_vel);
	     	
	float ang = Math::atan2(y,x);
		
	if (ABS(ang)<1e-3) // too small
		return;
	if (p_deaccel_sudden_turns) {
		
		if (is_on_floor() && ABS(ang)>Math_PI*0.8) {
			p_h_linear_vel*=0.8*p_time;
			
		}
	}
		
	float turn_ang = ABS(ang) * (!is_on_floor() || jumping?jump_accel_damp:1.0)*turn_speed * p_time;
	
	if (turn_ang > ABS(ang) ) {
		
		ang=0;
	} else if (ang < 0 ) {
		
		ang+=turn_ang;
	} else if (ang > 0 ) {
		
		ang-=turn_ang;
	}
		
	Vector3 res= (n * Math::cos(ang)) + (t * Math::sin(ang));
	p_h_linear_vel=res * p_h_linear_vel.length();
	
}
		
		
void CharacterBody::notify_contact(const PhysicsBody& p_body, const Vector3& p_contact, const Vector3& p_depth) {
	
	/* compute floor velocity */
	
	if (p_body.get_flags()&FLAG_INFLUENCE)
		return;
	/* must be a floor and must not have mass*/
	
	Vector3 dir=p_depth.get_normalized();
	
	if (dir.dot( gn ) < 0.5)
		return;
	
	on_floor=true;
	
	
	if ( p_body.get_inv_mass() > CMP_EPSILON )
		return;
	
	Vector3 B_vel = p_body.get_velocity_at_point(p_contact);
	Vector3 B_floor_vel=B_vel - gn * gn.dot(B_vel);
	
	if (B_floor_vel.squared_length() > floor_velocity_max.squared_length())
		floor_velocity_max=B_floor_vel;
	
	
	
}


/*
bool CharacterBody::handles_own_friction() const {

	return true;
}
void CharacterBody::handle_own_friction(const PhysicsBody& p_body_B,const Vector3& p_B_contact,const Vector3& p_depth) {

	Vector3 B_vel = p_body_B.get_linear_velocity() + Vector3::cross( p_body_B.get_angular_velocity(), p_B_contact );
	Vector3 B_floor_vel=B_vel - gn * gn.dot(B_vel);
	
	if (B_floor_vel.squared_length() > floor_velocity_max.squared_length())
		floor_velocity_max=B_floor_vel;
		

}*/
				
		
void CharacterBody::integrate_forces(float p_time) {

	if (!get_world_settings())
		return;
	
	if (influence)
		return;
	
	
	Vector3 g=get_world_settings()->get_gravity();
	

	process_floor(p_time);
	
	
	float glen=g.length();
	gn= (ABS(glen)<CMP_EPSILON)?Vector3():g/glen;	

	
	if (0) { 
		
		linear_velocity+=g*p_time;
		return;
	}
		
	floor_velocity=floor_velocity_max;
	
	linear_velocity-=floor_velocity_old;
	Vector3 target_dir=Vector3::cross(g,Vector3::cross(target_normal,g)).get_normalized();	
	
	Vector3 linear_h_velocity = linear_velocity - gn * linear_velocity.dot( gn );
	
	walk_speed=linear_h_velocity.length();
	
	adjust_turn_velocity(facing_dir,target_dir,p_time,false);
	facing_dir.normalize();
	
//	if (ABS(walk_speed)>0.001) {
//		facing_dir=linear_h_velocity;
	//}
	
	linear_velocity-=linear_h_velocity;
		
	adjust_turn_velocity(linear_h_velocity,target_dir,p_time,true);
	float speed = linear_h_velocity.length();
	float speed_adj = ((speed > max_walk_speed) ? -walk_damp : walk_accel)*p_time;
	if (!is_on_floor() || jumping)
		speed_adj*=jump_accel_damp;
	speed = (ABS(speed_adj)>ABS(speed-max_walk_speed))?max_walk_speed:(speed+speed_adj);
	if (linear_h_velocity.length()<CMP_EPSILON && max_walk_speed) {
		
		linear_h_velocity=target_dir*speed_adj;
	} else {
		linear_h_velocity=linear_h_velocity.get_normalized()*speed;
	}
		
	linear_velocity+=linear_h_velocity;
	linear_velocity+=floor_velocity;
	
	
	floor_velocity_old=floor_velocity;
	floor_velocity_max=Vector3();
	

	
	/*
	
	Vector3 target_linear_velocity=target_dir*max_walk_speed;
	
	Vector3 linear_h_velocity = linear_velocity - gn * linear_velocity.dot( gn );
	Vector3 rel = (target_linear_velocity - linear_h_velocity);
	float rel_len = rel.length();
	if (ABS(rel_len)>CMP_EPSILON) {
		float incr_len = MIN( rel_len, walk_accel*p_time );
		Vector3 incr = (rel/rel_len)*incr_len;
		linear_velocity+=incr;
	}
	*/		
	//if(!hover)
	linear_velocity+=g*p_time*gravity_factor;

}
		
void CharacterBody::integrate_velocities(float p_time) {
	
	if (influence)
		return;
	//printf("linvel %f,%f,%f\n",linear_velocity.x,linear_velocity.y,linear_velocity.z);
	//printf("pos %f,%f,%f - time %f\n",pos.x,pos.y,pos.z,p_time);
	pos+=linear_velocity*p_time;
	
	transform.load_translate(pos);
	
	transform.set_lookat(pos, pos+facing_dir, -gn );
	
	
	recreate_inertia_tensor();
	
	
	if (0)
		return;

	
	process_jump(p_time);
	
	// set terminal velocity
	const float terminal_velocity = 34;
	if(this->linear_velocity.squared_length() > terminal_velocity*terminal_velocity) {

		this->linear_velocity /= this->linear_velocity.length();
		this->linear_velocity *= terminal_velocity;
	}
	
	transform_changed.call();
}

void CharacterBody::set_walk_max_velocity(float p_max_vel) {
	
	max_walk_speed=p_max_vel;
}

void CharacterBody::spin(float p_angle) {

	//printf("rotating %f,%ls\n",p_angle,Variant(target_normal).get_string().c_str());
	target_normal.rotate(gn,p_angle);
}

void CharacterBody::reset_velocity() {

	this->linear_velocity = Vector3();
}

void CharacterBody::set_mass(float p_mass) {

	
	if (p_mass<0) {
	
		mass=1e7;
		inv_mass=0;
	} else {
	
		if (p_mass==0.0)
			p_mass=0.000001;
		inv_mass=1.0/p_mass;
		if (inv_mass<0.0000001)
			inv_mass=0;
		mass=p_mass;
		
	}

	inv_inertia_local=Vector3(
		volume->get_local_inertia(p_mass).x ? 1.0 / volume->get_local_inertia(p_mass).x : 0,
		volume->get_local_inertia(p_mass).y ? 1.0 / volume->get_local_inertia(p_mass).y : 0,
		volume->get_local_inertia(p_mass).z ? 1.0 / volume->get_local_inertia(p_mass).z : 0 );
		
	recreate_inertia_tensor();

}

IRef<IAPI> CharacterBody::get_owner() const {
	
	return owner;
}

void CharacterBody::set_owner(IRef<IAPI> p_iapi) {
 	
	owner=p_iapi;
}


CharacterBody::CharacterBody(IRef<Volume> p_volume,float p_mass) {

	
	if (p_mass<0) { //infinite mass
	
		mass=1e7;
		inv_mass=0;
	} else {
	
		if (p_mass==0.0)
			p_mass=0.000001;
			
		inv_mass=1.0/p_mass;
		if (inv_mass<0.0000001)
			inv_mass=0;
		
		mass=p_mass;
		
	}
		
	
	facing_dir=Vector3(0,0,-1);
	jump_accel_damp=0.1;
	walk_accel=9; 
	walk_damp=14;
	max_walk_speed=0;
	walk_speed=0;
	turn_speed=10;
	jump_impulse=10.0;
	jump_damp=5;
	jump=0;
	jumping=0;
	hover=0;
	gravity_factor = 1;
//	vertical_speed=0;
	airborne_min=0.1;
	last_on_floor_time=1e10;
	on_floor=false;
		
	target_normal=Vector3(0,0,-1); // simple default looking towards -z
//	current_dir=Vector3(0,0,-1); // simple default looking towards -z
//	target_dir=Vector3(0,0,-1); // simple default looking towards -z
	volume=p_volume;
	volume_aabb=volume->compute_AABB(Matrix4());
	
	
	friction_request=false;
	
	linear_damping=0.0001;
	inv_inertia_local=Vector3(
		p_volume->get_local_inertia(p_mass).x ? 1.0 / p_volume->get_local_inertia(p_mass).x : 0,
		p_volume->get_local_inertia(p_mass).y ? 1.0 / p_volume->get_local_inertia(p_mass).y : 0,
		p_volume->get_local_inertia(p_mass).z ? 1.0 / p_volume->get_local_inertia(p_mass).z : 0 );
		
	recreate_inertia_tensor();
	
	collidable=true;

	influence = false;	
	
	on_floor_frames=0;
	
	
}


CharacterBody::~CharacterBody() {

}


