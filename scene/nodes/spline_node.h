#ifndef SPLINE_NODE_H
#define SPLINE_NODE_H

#include "node.h"

#include "types/math/spline.h"

class SplineNode : public Node {
	
	IAPI_TYPE( SplineNode, Node );
	IAPI_INSTANCE_FUNC( SplineNode );

private:
	Spline spline;
	Color display_color;
	bool display;

	void set_display(bool p_display);

	void _iapi_set(const String& p_path, const Variant& p_value);
	void _iapi_get(const String& p_path,Variant& p_ret_value);
	void _iapi_get_property_list( List<PropertyInfo>* p_list );

	void _draw();
	
public:

	void set_spline(const Spline& p_spline);
	Spline& get_spline();

	SplineNode();
	~SplineNode();
};

#endif


