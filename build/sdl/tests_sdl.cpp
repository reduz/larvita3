

#include "bin/tests/test_main.h"


#include "build/sdl/setup_sdl.h"

int main(int argc,char* argv[]) {
	
	
	init_sdl( 1,argv );
	{
		
		if (argc<2) {
				
			printf("usage: test <testname>\n");
			List<String> tlist;
			test_get_list(&tlist);
			List<String>::Iterator* I = tlist.begin();
			while (I) {
				printf("\t%ls\n", I->get().c_str());
				I = I->next();
			};
			finish_sdl();
			return 255;
		}
			
		String test_str=argv[1];
		
		List<String> extra_args;
		for (int i=2;i<argc;i++) {
		
			extra_args.push_back( argv[i] );
		}
		
		test_main(test_str,extra_args);
	}		
	
	finish_sdl();

	return 0;
}


