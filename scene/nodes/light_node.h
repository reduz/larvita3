#ifndef LIGHT_NODE_H
#define LIGHT_NODE_H

#include "visual_node.h"

#include "render/light.h"

class LightNode : public VisualNode {

	IAPI_TYPE( LightNode, VisualNode );
	IAPI_INSTANCE_FUNC( LightNode );

	/** IAPI proxy */
	void _iapi_set(const String& p_path, const Variant& p_value);
	void _iapi_get(const String& p_path,Variant& p_ret_value);
	void _iapi_get_property_list( List<PropertyInfo>* p_list );
	void _iapi_get_property_hint( const String& p_path, PropertyHint& p_hint );
	
	void _iapi_get_method_list( List<MethodInfo>* p_list );
	void _iapi_call(const String& p_name, const List<Variant>& p_params,Variant& r_ret);

	void _iapi_init();

	IRef<Light> light;
	bool light_is_global;
	
	void event(EventType p_type);

	void draw(const SpatialRenderData& p_data);
	bool debug_draw;
	void set_debug_draw(bool p_global);

	void light_changed(const String& p_prop);
	void mode_changed();

	void visible_callback(const SpatialRenderData& p_data);

	void draw_point();
	void draw_spot(float p_angle, float p_radius);
	
public:

	void set_light(IRef<IAPI> p_light);
	IRef<Light> get_light();

	LightNode();
	~LightNode();
};

#endif
