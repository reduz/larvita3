#include "light_node.h"

#include "render/renderer.h"
#include "render/spatial_indexer.h"
#include "scene/main/scene_main_loop.h"

void LightNode::visible_callback(const SpatialRenderData& p_data) {
	
	if (light_is_global || !light) {
		return;
	};

	SpatialIndexer* i = Renderer::get_singleton()->get_spatial_indexer();
	if (use_post_matrix) {
		light->set_matrix(get_global_matrix() * post_matrix);
	} else {
		light->set_matrix(get_global_matrix());
	};
	i->add_light(light);
};

void LightNode::draw_spot(float p_angle, float p_radius) {
	
	Renderer* r = Renderer::get_singleton();
	r->render_primitive_set_material(r->get_indicator_material());
	r->render_set_matrix(Renderer::MATRIX_WORLD, get_global_matrix());
	Matrix4 mat;
	Color c[2];
	c[0] = c[1] = Color(0, 255, 0, 255);
	Vector3 last;
	Vector3 first;
	Vector3 v[2];

	for (int i=0; i<4; i++) {

		Matrix4 mtmp(mat);
		mtmp.rotate_x(p_angle);
		mtmp.translate(0, 0, -p_radius);
		
		v[0] = Vector3();
		v[1] = mtmp.get_translation();
		r->render_primitive(2, v, NULL, NULL, c, NULL);

		v[0] = last;

		if (i) {
			r->render_primitive(2, v, NULL, NULL, c, NULL);
		} else {
			first = v[1];
		};
		last = v[1];
		
		mat.rotate_z(Math_PI / 2);
	};
	v[0] = last;
	v[1] = first;
	r->render_primitive(2, v, NULL, NULL, c, NULL);
	
};

void LightNode::draw_point() {

	Renderer* r = Renderer::get_singleton();
	r->render_set_matrix(Renderer::MATRIX_WORLD, get_global_matrix());

	Vector3 v[2];
	Color c[2] = { Color(0, 255, 0), Color(0, 255, 0) };
	
	float size = light->get_radius();
	
	v[0] = Vector3(-size, 0, 0);
	v[1] = Vector3(size, 0, 0);
	r->render_primitive(2, v, NULL, NULL, c, NULL); 

	v[0] = Vector3(0, -size, 0);
	v[1] = Vector3(0, size, 0);
	r->render_primitive(2, v, NULL, NULL, c, NULL); 

	v[0] = Vector3(0, 0, -size);
	v[1] = Vector3(0, 0, size);
	r->render_primitive(2, v, NULL, NULL, c, NULL); 
};

void LightNode::draw(const SpatialRenderData& p_data) {

	if (debug_draw) {
		return;
		switch (light->get_mode()) {
			
		case Light::MODE_DIRECTIONAL:
			draw_spot(Math_PI / 6, 2);
			break;
		case Light::MODE_POINT:
			draw_point();
			break;
		case Light::MODE_SPOT:
			draw_spot(light->get_spot_angle() / 180.0 * Math_PI, light->get_radius());
			break;
		default: break;
		};
	};
};

void LightNode::set_debug_draw(bool p_draw) {
	
	debug_draw = p_draw;
	if (!light) return;
	if (light->get_mode() == Light::MODE_DIRECTIONAL) {
		
		if (p_draw) {
			set_AABB( Rect3(Vector3(-1, -1, -1), Vector3(2, 2, 2)) );
		} else {
			set_AABB( Rect3() );
		};
	};
};

IRef<Light> LightNode::get_light() {
	return light;
};

void LightNode::mode_changed() {
	SpatialIndexer* i = Renderer::get_singleton()->get_spatial_indexer();
	if (light_is_global && light->get_mode() != Light::MODE_DIRECTIONAL) {
		i->remove_global_light(light);
		event_signal.disconnect(this,&LightNode::event);
	};

	if (light->get_mode() == Light::MODE_DIRECTIONAL) {
		
		light_is_global = true;
		i->add_global_light(light);
		event_signal.connect(this,&LightNode::event);
	} else {
		light_is_global = false;
		set_AABB( light->get_AABB() );
	};		
};

void LightNode::set_light(IRef<IAPI> p_light) {
	
	if (light == p_light) {
		return;
	};
	
	SpatialIndexer* i = Renderer::get_singleton()->get_spatial_indexer();
	
	if (light) {
	
		light->property_changed_signal.disconnect(this, &LightNode::light_changed);

		if (light_is_global) {
			i->remove_global_light(light);
			event_signal.disconnect(this,&LightNode::event);
		};
	};
	
	light = p_light;
	
	if (!light) {
		return;
	};

	light_is_global = false;
	light->property_changed_signal.connect(this, &LightNode::light_changed);
	mode_changed();
};

void LightNode::light_changed(const String& p_prop) {
	
	if (p_prop == "mode" || p_prop == "radius") {
		mode_changed();
	};
};

void LightNode::event(EventType p_type) {

	switch(p_type) {
		case EVENT_MATRIX_CHANGED: 
			if (light && light_is_global) {
				light->set_matrix(get_global_matrix());
			};
			break;
		default:
			break;
	};
};

void LightNode::_iapi_set(const String& p_path, const Variant& p_value) {
	if (p_path == "light") {
		set_light(p_value.get_IOBJ());
	};
	if (p_path == "debug_draw") {
		set_debug_draw(p_value);
	};
};

void LightNode::_iapi_get(const String& p_path,Variant& p_ret_value) {
	if (p_path == "light") {
		p_ret_value = IObjRef(get_light());
	};
	if (p_path == "debug_draw") {
		p_ret_value = debug_draw;
	};
};

void LightNode::_iapi_get_property_list( List<PropertyInfo>* p_list ) {

	p_list->push_back( IAPI::PropertyInfo( Variant::IOBJ, "light" ) );
	p_list->push_back( IAPI::PropertyInfo( Variant::BOOL, "debug_draw", PropertyInfo::USAGE_EDITING ) );
};

void LightNode::_iapi_get_property_hint( const String& p_path, PropertyHint& p_hint ) {

	if (p_path == "light") {
		p_hint=PropertyHint( PropertyHint::HINT_IOBJ_TYPE, "Light" );
	};
};

void LightNode::_iapi_get_method_list( List<MethodInfo>* p_list ) {
	
};

void LightNode::_iapi_call(const String& p_name, const List<Variant>& p_params,Variant& r_ret) {

};

void LightNode::_iapi_init() {
	
	SpatialObject* so = get_spatial_object();
	ERR_FAIL_COND(!so);
	so->set_visible_callback( Method1<const SpatialRenderData&>( this, &LightNode::visible_callback ) );
};

LightNode::LightNode() {

	set_signal.connect( this, &LightNode::_iapi_set );
	get_signal.connect( this, &LightNode::_iapi_get );
	get_property_list_signal.connect( this, &LightNode::_iapi_get_property_list );
	get_property_hint_signal.connect( this, &LightNode::_iapi_get_property_hint );
	
	get_method_list_signal.connect( this, &LightNode::_iapi_get_method_list );
	call_signal.connect( this, &LightNode::_iapi_call );

	init_signal.connect( this, &LightNode::_iapi_init );

	light_is_global = false;
	
	set_debug_draw(SceneMainLoop::get_singleton()->get_process_flags() & MODE_FLAG_EDITOR?true:false);
};

LightNode::~LightNode() {
	
	if (light && light_is_global) {
		SpatialIndexer* i = Renderer::get_singleton()->get_spatial_indexer();
		i->remove_global_light(light);
	};
};


