#include "global_vars.h"
#include "fileio/config_file.h"

GlobalVars* GlobalVars::instance = NULL;


void GlobalVars::set(String p_path, const Variant& p_value) {

	vars[p_path] = p_value;
	
};

Variant GlobalVars::get(String p_path) const {
	
	if (!vars.has(p_path))
		return Variant(); // bleh, nobody wants to fix the errors
	ERR_FAIL_COND_V( !vars.has(p_path), Variant() );
	return vars[p_path];
};

GlobalVars* GlobalVars::get_singleton() {

	return instance;	
};


void GlobalVars::get_property_list( List<PropertyInfo> *p_list ) const {

	const String *s=NULL;
	while ( (s=vars.next(s)) ) {
	
		p_list->push_back( PropertyInfo( vars[*s].get_type(), *s ) );
	
	}
	

}


void GlobalVars::load_option(String p_section,String p_variable,String p_value) {


	if (p_section != "")
		p_variable=p_section+"/"+p_variable;
	
	printf("reading %s : %s\n",p_variable.ascii().get_data(),p_value.ascii().get_data());
	
	if (p_value.find("\"")!=-1 || p_value.find("'")!=-1) { // string
		
		p_value.replace("\\\"","\\042");
		p_value.replace("\\'","\\039");
		
		p_value.replace("\"","");
		p_value.replace("'","");
		p_value.strip_edges();		
			
		p_value.replace("\\042","\"");
		p_value.replace("\\039","'");
		
		printf("setting var %s to %s\n",p_variable.ascii().get_data(),p_value.ascii().get_data());
		vars[p_variable]=p_value;
		
	} else if (p_variable.find(".")!=-1) {
				
		vars[p_variable]=p_value.to_double();
		
	} else if (p_value.find("true")!=-1 || p_value.find("yes")!=-1)   {
		
		vars[p_variable]=true;
	} else if (p_value.find("false")!=-1 || p_value.find("no")!=-1)   {
		
		vars[p_variable]=false;
	} else {
	
		vars[p_variable]=(int)p_value.to_int();		
	}
	
	
}

Error GlobalVars::load_from_file(String p_file) {
	
	FileAccess *fa = FileAccess::create();
	ConfigFile config_file( fa  );
	config_file.set_path(p_file);
	config_file.read_entry_signal.connect( this, &GlobalVars::load_option );
	Error fail=config_file.load();
	memdelete( fa );
	return fail;
}

String GlobalVars::translate_path(String p_path) {
	p_path.replace("tex://",get("path/resources").get_string() + "/" + get("path/textures").get_string()+"/");
	p_path.replace("script://",get("path/resources").get_string() + "/" + get("path/scripts").get_string()+"/");
	p_path.replace("db://",get("path/resources").get_string() + "/" + get("path/db").get_string()+"/");
	p_path.replace("res://",get("path/resources").get_string()+"/");
	return p_path;
}

GlobalVars::GlobalVars() {
	
	instance = this;
	
	vars["path/resources"]=".";
	vars["path/textures"]="textures";
	vars["path/db"]="db_sqlite";
	vars["path/scripts"]="scripts";
	vars["iapi_db_type"]="sqlite";
	vars["sqlite_db_file"]="sqlite.db";
	vars["path/scripts"]="./scripts";
	vars["http_resource_host"]="localhost";
	vars["http_resource_port"]=80;
	vars["audio/sample_ram"]=5*1024*1024; // 2 mb
	vars["audio/sw_mixing_rate"]=44100; 
	vars["audio/sw_bits"]=16; 
	vars["audio/sw_stereo"]=true; 
	vars["audio/sw_buffer_size"]=1024; 
	vars["audio/sw_gain"]=0.7; 
	vars["render/render_list_size_kb"]=2048;
	vars["render/render_list_elements"]=32768;
};
