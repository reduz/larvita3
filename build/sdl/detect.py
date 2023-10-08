
import os
import sys	

def can_build():
	print("detecting..")
	return True
  
def get_opts():
	print("getting opts")
	return [ ]
  
def get_flags():

	return [ ("sdl_testflag","Testing Flag (true|false)","true"),
			]


def configure(env):

	if 'msvc' in env['TOOLS']:
		env.Append(CPPPATH=['/mingw/pgsql/include'])
		env.Append(LIBPATH=['/mingw/pgsql/lib'])
		env.Append(CPPFLAGS=['/DMSVC', '/GR', ])
		env.Append(CXXFLAGS=['/TP'])
		env.Append(CCFLAGS=['/MT','/Gd'])
		env['ENV']['INCLUDE'] = "C:\\Program Files\\Microsoft Visual Studio 9.0\\VC\\include;C:\\Program Files\\Microsoft Visual Studio 9.0\\VC\\atlmfc\\include;C:\\Program Files\\Microsoft SDKs\\Windows\\v6.0A\\include;C:\\Program Files\\Microsoft SDKs\\Windows\\v6.0A\\include\\SDL"
		env['ENV']['PATH'] = env['ENV']['PATH'] + ";C:\\Program Files\\Microsoft Visual Studio 9.0\\VC\\bin;C:\\Program Files\\Microsoft Visual Studio 9.0\\Common7\\IDE"
		env['ENV']['LIB'] = "C:\\Program Files\\Microsoft Visual Studio 9.0\\VC\\lib;C:\\Program Files\\Microsoft SDKs\\Windows\\v6.0A\\Lib;"
		#print("env is "+str(env['ENV']))

		if (env["profile"]):

			pass
		elif (env["optimize"]):

			env.Append(CCFLAGS=['/O2', '/EHsc', '/GL'])
			#env.Append(LINKFLAGS=['/LTCG'])
			

		elif (env["debug"]):

			env.Append(CCFLAGS=['/Zi'])
			env.Append(CPPFLAGS=['-DDEBUG_MEMORY_ALLOC'])

	else:

		if (sys.platform == 'darwin'):

			env.Append(CPPPATH=['/Library/Frameworks/SDL.framework/Headers'])
			env.Append(CCFLAGS=['-D_THREAD_SAFE'])
			env.Append(LIBPATH=['/Library/Frameworks/SDL.framework/lib'])
			env.Append(LINKFLAGS=['-framework', 'SDL', '-framework', 'Cocoa', '-framework', 'Carbon', '-framework', 'OpenGL'])
		else:
			env.ParseConfig("sdl-config --cflags --libs")

		if (env["profile"]):
	
			env.Append(CCFLAGS=['-g','-pg'])
			env.Append(LINKFLAGS=['-pg'])		
			env.Append(CPPFLAGS=['-DLUA_USE_APICHECK'])
		elif (env["optimize"]):

			env.Append(CCFLAGS=['-O3','-ffast-math','-fomit-frame-pointer'])
			env.Append(LINKFLAGS=['-O3','-ffast-math','-fomit-frame-pointer'])
			

		elif (env["debug"]):
			env.Append(CCFLAGS=['-g3', '-Wall'])
			env.Append(CPPFLAGS=['-DLUA_USE_APICHECK'])
			env.Append(CPPFLAGS=['-DDEBUG_MEMORY_ALLOC'])

	env.Append(CPPFLAGS=['-DSDL_ENABLED', '-DOPENGL_ENABLED', '-DOPENGL2_ENABLED']); # must enable SDL 
	
	if (os.name=="nt"):
	
		env['enable_ssl'] = 0
		env.Append(CPPFLAGS=['-DWINDOWS_ENABLED', '-DWIN32_ENABLED', '-DWIN32']);
		env['enable_glew'] = 1
		if env['enable_postgres']:
			env.Append(LIBS=['pq'])
		if env['enable_ssl']:
			env.Append(libs=['ssl'])
		if 'msvc' in env['TOOLS']:

			env.Append(LINKFLAGS=['/NODEFAULTLIB:msvcrt', '/DEBUG'])
			env.Append(LIBS=['sdl', 'sdlmain'])

		env.Append(LIBS=['glew32', 'opengl32', 'wsock32', 'winmm', 'shell32'])
		
	elif (sys.platform == 'darwin'):

		env['enable_ssl'] = 0
		env.Append(CPPFLAGS=['-F/Developer/SDKs/MacOSX10.4u.sdk/System/Library/Frameworks', '-mmacosx-version-min=10.4', '-DPOSIX_ENABLED', '-DOSX_ENABLED', '-isysroot', '/Developer/SDKs/MacOSX10.4u.sdk'])
		env.Append(LIBS=['ssl']);
		if env['enable_glew']:
			env.Append(LIBS=['GLEW']);
		env.Append(LINKFLAGS=['-mmacosx-version-min=10.4', '-isysroot', '/Developer/SDKs/MacOSX10.4u.sdk', '-Wl,-syslibroot,/Developer/SDKs/MacOSX10.4u.sdk'])
		env['enable_collada'] = 0
		env['enable_postgres'] = 0
	else:
		env.Append(LIBS=['GLU','GL', 'ssl'])
		if env['enable_glew']:
			env.Append(LIBS=['GLEW']);
		if env['enable_postgres']:
			env.Append(LIBS=['pq'])
		env.Append(CXXFLAGS=['-DPOSIX_ENABLED']);

	if env['enable_glew']:
		env.Append(CPPFLAGS=['-DGLEW_ENABLED'])

	if env['enable_collada']:
		env.Append(TOLUAFLAGS=['-E', 'COLLADA_ENABLED'])
		env.Append(CXXFLAGS=['-DCOLLADA_ENABLED']);
	if env['enable_postgres']:
		env.Append(TOLUAFLAGS=['-E', 'POSTGRES_ENABLED'])
		env.Append(CXXFLAGS=['-DPOSTGRES_ENABLED']);
	if env['enable_ssl']:
		env.Append(TOLUAFLAGS=['-E', 'SSL_ENABLED'])
		env.Append(CXXFLAGS=['-DSSL_ENABLED']);
		

	if env['optimize']:
		env['OBJSUFFIX'] = "_opt"+env['OBJSUFFIX']
		#env['LIBSUFFIX'] = "_opt"+env['LIBSUFFIX']
	
	print("configuring!")
	
	
