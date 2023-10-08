#include "build/sdl/setup_sdl.h"

#include "lua/lua_manager.h"
#include "fileio/iapi_data.h"

#include "types/global_vars.h"
#include "main/main.h"

#include <string.h>

#ifdef OSX_ENABLED
#include <unistd.h>
#endif

static int rebuild_arg(int& argc, char**& argv) {

	printf("reading from cmdline %s\n", getenv("PWD"));


	#ifdef OSX_ENABLED
	printf("reading from cmdline %s\n", getenv("PWD"));

	int len = strlen(argv[0]);

	while (len--){
		if (argv[0][len] == '/') break;
	}

	if (len>=0) {
		char *path = (char *)malloc(len+1);
		memcpy(path, argv[0], len);
		path[len]=0;
		printf("Path: %s\n", path);
		chdir(path);
	}
	#endif

	FILE* f = fopen("cmdline", "rb");
	if (!f) {
		return 1;
	};
	printf("opened\n");
	int size = 8;
	char* prgname = argv[0];
	argv = (char**)malloc(sizeof(char*) * size);
	argv[0] = prgname;

	int i=1;
	char line[256];

	do {

		if (i >= size) {
			size += 8;
			argv = (char**)realloc(argv, sizeof(char*) * size);
		};

		if (fgets(line, sizeof(line), f) && *line && *line != '\n' && *line != '\r') {

			int len = strlen(line)+1;
			argv[i] = (char*)malloc(len);
			strcpy(argv[i], line);
			if (argv[i][len-3] == '\r') {

				argv[i][len-3] = 0;
			} else {
				argv[i][len-2] = 0;
			};
			printf("arg %i is %s\n", i, argv[i]);
		} else {

			argv[i] = NULL;

			break;
		};
		++i;
	} while (true);

	argc = i;

	fclose(f);

	return 0;
};

int main(int argc,char* argv[]) {

	if (argc < 2)
		if (rebuild_arg(argc, argv))
			return 1;

	init_sdl(argc, argv);

	IAPI_REGISTER_TYPE(IAPIData);

	String script_name = argv[1];
	{
		String filepar = argv[1];
		if (filepar.find(".lua") != filepar.size() - 4) {

			script_name = "res://main.lua";
		};
	};

	LuaManager lm;
	String mode = GlobalVars::get_singleton()->get("script_load_mode");
	if (mode == "database") {
		lm.set_load_mode(LuaManager::MODE_DATABASE);
	} else {
		lm.set_load_mode(LuaManager::MODE_FILESYSTEM);
	};
	lm.load_args(argc, argv);
	
	String err;

	if (lm.require(script_name, err) != OK) {
		printf("error loading script: %s\n", (char*)err.ascii());
	};

	return 0;
	
	lua_close(lm.get_lua_state());

	finish_sdl();

	return 0;
}

