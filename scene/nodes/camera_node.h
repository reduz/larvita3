#ifndef SCENE_CAMERA_H
#define SCENE_CAMERA_H

#include "scene/nodes/node.h"

class CameraNode : public Node {

	IAPI_TYPE( CameraNode, Node );
	IAPI_INSTANCE_FUNC( CameraNode );

	Matrix4 projection_matrix;

	/* basic handlers for iapi */
	void _set(const String& p_path, const Variant& p_value);
	void _get(const String& p_path,Variant& p_ret_value);
	void _get_property_list( List<PropertyInfo>* p_list );

	void _call(const String& p_method, const List<Variant>& p_params,Variant& p_ret);
	void _get_method_list(List<MethodInfo> *p_method_list);

public:

	Matrix4 get_projection_matrix();
	void set_projection_matrix(const Matrix4& p_mat);
	
	void set_as_current(); /**< Makes camera the current camera (rendering will happen from here) */
	
	void render();


	CameraNode();
	~CameraNode();
};


#endif


