//
// C++ Interface: iapi_layer
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef IAPI_LAYER_H
#define IAPI_LAYER_H

#include "iapi.h"


/**
 * 	@class IAPI_Layer
	@author Juan Linietsky <reduzio@gmail.com>
 *	
 *	An IAPI Implementation layer. This is a higher level of IAPI that handles a lot of things,
 * 	such as multi-level inheritance access to IAPI, abstracting for thread-safety, 
 * 	Inheriting the IAPI functions directly is no good (they are private here), instead, you have 
 * 	to supply Methods to be called back to the protected functions
 * 	
*/


class IAPI_Layer : public IAPI {
	
	IAPI_TYPE( IAPI_Layer, IAPI );
	
	/* Overriding these is no longer possible from this layer, connecting to signals must be done instead */
	
	virtual void set(String p_path, const Variant& p_value);
	virtual Variant get(String p_path) const; 
	virtual void get_property_list( List<PropertyInfo> *p_list ) const;
	virtual PropertyHint get_property_hint(String p_path) const;
	
	virtual Variant call(String p_method, const List<Variant>& p_params=List<Variant>());
	
	virtual void get_method_list( List<MethodInfo> *p_list ) const;
		
	
protected:	
	/* These signals must be connected by any level of inheritance in order to have access to the IAPI.
	   This way, many levels can have transparent access to IAPI */
	
	/* properties */
	
	Signal< Method2< const String&,const Variant& > > set_signal; ///< Called when a property is set
	Signal< Method2< const String&, Variant& > > get_signal; ///< Called when a property is requested, if handled, the result must be returned in the variant.
	Signal< Method1< List<PropertyInfo>* > > get_property_list_signal; ///< Signal to obtain the list of properties.
	
	Signal< Method2< const String&, PropertyHint& > > get_property_hint_signal; ///< Signal to obtain a hint from a property
	
	/* methods */
		
	Signal< Method3< const String&, const List<Variant>&, Variant& > > call_signal; ///< Signal used for receiving a call. If the object/inheritance layer needs it, can connect
	
	Signal< Method1< List<MethodInfo> * > > get_method_list_signal; ///< If the object/inheritance layer receives calls, it can specify which ones by filling them into this list
	
	Signal<> init_signal;
	Signal<> predelete_signal;

	bool __debug_empty;
	
public:
	
	virtual void predelete();
	virtual void init();	
	IAPI_Layer();
	~IAPI_Layer();

};

#endif
