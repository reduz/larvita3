//
// C++ Interface: iapi_base
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef IAPI_BASE_H
#define IAPI_BASE_H


#include "variant.h"
#include "signals/signals.h"
#include "table.h"
#include "list.h"
/**
 * 	@class IAPI
	@author Juan Linietsky <reduzio@gmail.com>
 *
 * 	Introspection API, Base. Objects that inherit from here can implement introspective objects
 *      (objects which can export their properties and methods).
*/

#include "iref.h"
	
#define IAPI_DEFAULT_INSTANCE_FUNC _iapi_default_instance_func
	
#define IAPI_TYPE( m_type , m_parent_type ) IOBJ_TYPE( m_type, m_parent_type )
#define IAPI_INSTANCE_FUNC( m_type )\
	public:\
	static IRef<IAPI> IAPI_DEFAULT_INSTANCE_FUNC(const String& p_type,const  IAPI::CreationParams& p_params) { return memnew( m_type ); } \
	private:
#define IAPI_PERSIST_AS( m_type )\
	public:\
	virtual String persist_as() const { return m_type::get_type_static(); }\
	private:
	

SCRIPT_BIND_BEGIN;	

class IAPI : public IObj {
	IOBJ_TYPE(IAPI,IObj);
	int _objid;
friend class IAPI_DB; // only the database can set IDs
	void set_ID(ObjID p_ID);
public:
		
	/* Properties */
	
	virtual void set(String p_path, const Variant& p_value)=0; /** set a property, by path */
	virtual Variant get(String p_path) const=0; /** get a property, by path */
	
	
	struct PropertyInfo {
		
		enum UsageFlags {
			
			USAGE_GENERAL=1,
			USAGE_DATABASE=2, ///< Will be saved on Database
			USAGE_EDITING=4, ///< Will be displayed on editor
			USAGE_NETWORK=8, ///< Will be syncronized on the network
			USAGE_PERSISTENT=16, ///< Hinted as persistent (for Game usage)
			USAGE_CREATION_PARAM=32,
			USAGE_DEFAULT=USAGE_GENERAL|USAGE_DATABASE|USAGE_EDITING,
			USAGE_SERIALIZE=USAGE_GENERAL|USAGE_DATABASE|USAGE_NETWORK
		};
		
		Variant::Type type;
		String path;
		unsigned int usage_flags;
		
		PropertyInfo( Variant::Type p_type=Variant::NIL, String p_path="",unsigned int p_usage_flags=USAGE_DEFAULT ) { type=p_type; path=p_path; usage_flags=p_usage_flags; }
		
	};
	
	virtual void get_property_list( List<PropertyInfo> *p_list ) const=0; ///< Get a list with all the properies inside this object
	
	struct PropertyHint {
		
		enum Type {
			HINT_NONE, ///< no hints 
   			HINT_RANGE, ///< range hint as "min,max" or "min,max,step" if step exists (only int/real)
			HINT_ENUM, ///< possible values as "Pizza,Beer,Taco" (only for int/real)
			HINT_FLAGS, ///< flags, in ascending bit order values like "special,makegreen,,big" (empty for no flag)
			HINT_FILE, ///< a String value came from a file, hint is a list of extensions - ".jpg;.wav;.doc;.etc"
			HINT_IOBJ_TYPE, ///< String is Type of RID, (usually, as identified by an Asset Manager)
			HINT_SCENE_PATH, ///< a path to a node on the scene
		};
						
		Type type;
		String hint;
		String description;
		Variant default_value;
		PropertyHint(Type p_type=HINT_NONE, String p_hint="",Variant p_default=Variant(), String p_description = "") { type=p_type; hint=p_hint; default_value=p_default; description = p_description; }
		PropertyHint(const PropertyHint& p_hint) { type = p_hint.type; hint = p_hint.hint; default_value = p_hint.default_value; description = p_hint.description; };
	};
	
	virtual PropertyHint get_property_hint(String p_path) const { return PropertyHint(); }; ///< get a property hint
	
	
	
	/**
	 * This is emitted when a property/properties/whole object changes inside.
	 * Some objects may notify exactly which property (or properties) changed.
	 * Others may not be able to provide as much detail, and will simply provide
	 * no information (list pointer will be NULL). When NULL is received as parameter,
	 * it must be assumed that the whole object changed.
	 */
	 
	Signal< Method1< const String& > > property_changed_signal; 
	
	/* Methods */
	
	/**
	 * Structure containing parameters to be passed for call, in a table.
	 */
	
	
	virtual Variant call(String p_method, const List<Variant>& p_params=List<Variant>());
	
	struct MethodInfo {
		
		String name;
		
		struct ParamInfo {
			
			Variant::Type type;
			String name;
			
			PropertyHint hint;
			
			ParamInfo( Variant::Type p_type, String p_name, const PropertyHint& p_hint = PropertyHint() ) { type=p_type; name=p_name; hint = p_hint; }
			ParamInfo() { type=Variant::NIL; }
		};
		
		List< ParamInfo > parameters;
		Variant::Type return_type;

		MethodInfo(String p_name="") { name=p_name; return_type=Variant::NIL; }
	};
	
	virtual void get_method_list( List<MethodInfo> *p_list ) const{}; ///< Get a list with all the properies inside this object
	
	/* Serialize and Deserialize */
	
	typedef Table<String,Variant> CreationParams;
	
	virtual Error serialize( Serializer *p_serializer,unsigned int p_usage=PropertyInfo::USAGE_SERIALIZE) const;
	virtual Error deserialize( Deserializer *p_deserializer );
	
	/* Helpers */
	Error serialize_creation_params( Serializer *p_serializer ) const;
	static Error deserialize_creation_params( Deserializer *p_deserializer, CreationParams& p_params );
	
	virtual void get_dependency_list( List<ObjID>* p_list );
	
	virtual ObjID get_ID() const;
	
	IAPI();
	virtual ~IAPI();

};

typedef IRef<IAPI> IAPIRef;

SCRIPT_BIND_END;

/**
 * method for creating an IAPI of a given type. the first argument is the type
 * to be created, as an instance func should be able to create many types if 
 * needed.
 */
typedef IRef<IAPI>(*IAPI_InstanceFunc)(const String&,const IAPI::CreationParams&);


#endif
