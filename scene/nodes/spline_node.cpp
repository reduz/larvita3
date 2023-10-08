#include "spline_node.h"

#include "render/renderer.h"
#include "render/spatial_indexer.h"
#include "scene/main/scene_main_loop.h"

void SplineNode::_draw() {
	
	Renderer* r = Renderer::get_singleton();

	r->render_set_matrix(Renderer::MATRIX_WORLD, get_global_matrix());
	
	int points = spline.get_point_count();

	Color c[2] = { display_color, display_color };
	for (int i=0; i<points - 1; i++) {
	
		Vector3 p1 = spline.get_point(i);
		Vector3 p2 = spline.get_point(i+1);
		
		Vector3 l[2] = { p1, p2 };
		
		r->render_primitive(2, l, 0, 0, c, 0);
	};
};

void SplineNode::set_display(bool p_display) {
	
	if (p_display) {

		Renderer::get_singleton()->get_spatial_indexer()->render_scene_signal.connect(this, &SplineNode::_draw);
		
	} else {
		
		Renderer::get_singleton()->get_spatial_indexer()->render_scene_signal.disconnect(this, &SplineNode::_draw);
	};
	
	display = p_display;
};

void SplineNode::_iapi_set(const String& p_path, const Variant& p_value) {

	if (p_path == "display") {
		
		set_display(p_value);
		
	} else if (p_path == "display_color") {
		
		display_color = p_value;
		
	} else if (p_path == "spline_points") {

		ERR_FAIL_COND( p_value.get_type() != Variant::REAL_ARRAY );
		spline.set_point_count(p_value.size() / 3);
		for (int i=0; i<spline.get_point_count(); i++) {
			
			Vector3 p;
			p.x = p_value.array_get_real(i*3+0);
			p.y = p_value.array_get_real(i*3+1);
			p.z = p_value.array_get_real(i*3+2);
			
			spline.set_point(i, p);
		};
			
	} else if (p_path == "spline_pulls") {

		// todo
		
	};
};

void SplineNode::_iapi_get(const String& p_path,Variant& p_ret_value) {
	
	if (p_path == "display") {

		p_ret_value = Variant(display);
		
	} else if (p_path == "display_color") {
		
		p_ret_value = display_color;
		
	} else if (p_path == "spline_points") {

		Variant ret(Variant::REAL_ARRAY, spline.get_point_count() * 3);
		for (int i=0; i<spline.get_point_count(); i++) {
			
			Vector3 p = spline.get_point(i);
			ret.array_set_real(i * 3 + 0, p.x);
			ret.array_set_real(i * 3 + 1, p.y);
			ret.array_set_real(i * 3 + 2, p.z);
		};
		
		p_ret_value = ret;
		
	} else if (p_path == "spline_pulls") {

		p_ret_value = Variant();
	};
};

void SplineNode::_iapi_get_property_list( List<PropertyInfo>* p_list ) {
	
	p_list->push_back( IAPI::PropertyInfo( Variant::BOOL, "display", IAPI::PropertyInfo::USAGE_EDITING ) );
	p_list->push_back( IAPI::PropertyInfo( Variant::COLOR, "display_color", IAPI::PropertyInfo::USAGE_EDITING ) );

	p_list->push_back( IAPI::PropertyInfo( Variant::REAL_ARRAY, "spline_points" ) );
	p_list->push_back( IAPI::PropertyInfo( Variant::REAL_ARRAY, "spline_pulls" ) );
};

void SplineNode::set_spline(const Spline& p_spline) {
	
	spline = p_spline;
};

Spline& SplineNode::get_spline() {

	return spline;	
};

SplineNode::SplineNode() {

	display = false;
	display_color.b = 255;
	
	set_signal.connect( this, &SplineNode::_iapi_set );
	get_signal.connect( this, &SplineNode::_iapi_get );
	get_property_list_signal.connect( this, &SplineNode::_iapi_get_property_list );
};

SplineNode::~SplineNode() {
	
};


