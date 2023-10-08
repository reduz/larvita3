EnsureSConsVersion(0,14);

import os
import os.path
import glob
import sys

# scan possible build platforms

platform_list = [] # list of platforms
platform_opts = {} # options for each platform
platform_flags = {} # flags for each platform

for x in glob.glob("build/*"):
	if (not os.path.isdir(x)):
		continue
	tmppath="./"+x
	sys.path.append(tmppath)
	import detect
	if (detect.can_build()):
		x=x.replace("build/","") # rest of world
		x=x.replace("build\\","") # win32
		platform_list+=[x]
		platform_opts[x]=detect.get_opts()
		platform_flags[x]=detect.get_flags()
	sys.path.remove(tmppath)
	sys.modules.pop('detect')
			
		
print platform_list	

import methods
scanners = Environment().Dictionary('SCANNERS')
pkgscan = Scanner(name='pkg',
					function = methods.pkg_scan_func,
					skeys = ['.pkg'],
					recursive = True)

tools = ['default']
#if os.name == 'nt':
#       tools = ['mingw']
env_native = Environment(tools = tools, SCANNERS = scanners + [pkgscan])
Export('env_native')

env_native.Append( BUILDERS = { 'Vert' : Builder(action = methods.build_shader_header, suffix = 'vert.h',src_suffix = '.vert') } )
env_native.Append( BUILDERS = { 'Frag' : Builder(action = methods.build_shader_header, suffix = 'frag.h',src_suffix = '.frag') } )
env_native.Append( BUILDERS = { 'GLSL' : Builder(action = methods.build_glsl_headers, suffix = 'glsl.h',src_suffix = '.glsl') } )

env_native.__class__.LuaBinding = methods.make_tolua_code
env_native.__class__.add_source_files = methods.add_source_files
env_native.has_tolua_target = False

opts=Options(['custom.py'], ARGUMENTS)
opts.Add('optimize', 'Optimize (0|1).', 0)
opts.Add('profile', 'Profile (0|1).', 0)
opts.Add('debug', 'Add debug symbols (0|1).', 1)
opts.Add('platform',str(platform_list)+'(sdl).','sdl')
opts.Add('distcc_hosts', 'List of host for compilation with distcc', None)
opts.Add('TOLUAPP', 'the name of the tolua++ binary', 'lua/tolua++/src/bin/tolua++5.1')
opts.Add('TOLUABOOT', 'the name of the tolua++ bootstrap binary', 'lua/tolua++/src/bin/tolua_bootstrap++5.1')
opts.Add('TOLUAFLAGS', 'tolua++ flags', [])
opts.Add('TOLUA_OUTSUFFIX', 'suffix for tolua output', '.cpp')
opts.Add('tolua_bin', 'name of tolua++ binary to output', 'tolua++5.1')
opts.Add('tolua_bootstrap', 'name of tolua++ binary to output for boostrapping', 'tolua_bootstrap++5.1')
opts.Add('lua_number_type', 'type for lua numbers', "double")
opts.Add('game', 'Game directory', False)
opts.Add('GAME_BINDINGS', 'Game bindings', "")

opts.Add('enable_collada', 'Enable collada', True)
opts.Add('enable_postgres', 'Enable postgres', True)
opts.Add('enable_ssl', 'Enable ssl', True)
opts.Add('enable_glew', 'Use glew instead lf GLee', False)

env_native['TOLUA_PROGSUFFIX'] = env_native['PROGSUFFIX']

# add platform specific flags

for k in platform_flags.keys():
	flag_list = platform_flags[k]
	for f in flag_list:
		opts.Add(f[0],f[1],f[2])
	

opts.Update(env_native) # update environment
Help(opts.GenerateHelpText(env_native)) # generate help

if 'msvc' in env_native['TOOLS']:
	env_native.Append(CPPFLAGS=['/DMSVC'])
	env_native['ENV']['INCLUDE'] = "C:\\Program Files\\Microsoft Visual Studio 9.0\\VC\\include;C:\\Program Files\\Microsoft Visual Studio 9.0\\VC\\atlmfc\\include;C:\\Program Files\\Microsoft SDKs\\Windows\\v6.0A\\include;C:\\Program Files\\Microsoft SDKs\\Windows\\v6.0A\\include\\SDL"
	env_native['ENV']['PATH'] = env_native['ENV']['PATH'] + ";C:\\Program Files\\Microsoft Visual Studio 9.0\\VC\\bin;C:\\Program Files\\Microsoft Visual Studio 9.0\\Common7\\IDE"
	env_native['ENV']['LIB'] = "C:\\Program Files\\Microsoft Visual Studio 9.0\\VC\\lib;C:\\Program Files\\Microsoft SDKs\\Windows\\v6.0A\\Lib;"
	#print("env is "+str(env_native['ENV']))

	
env_native.lib_objects = {}
env_native.lib_link_order=[]

env_native.Append(CPPPATH=['#types','#types/string','#types/iapi','#types/math','#types/image','#gui','#tools','#'])

#env.Append(LIBPATH=['#drivers','#gui_app','#gui_common','#editor','#engine','#gui','#types']);

env_native.Append(CXXFLAGS=['-DGUI_EXTERNAL_STRING="\\\"types/string/rstring.h\\\""','-DGUI_EXTERNAL_MEMORY="\\\"gui_bindings/gui_memory.h\\\""']);
env_native.platforms = {}

Export('env_native')

SConscript("build/SCsub")

env_native['OBJSUFFIX'] = ".native" + env_native['OBJSUFFIX']
env_native['LIBSUFFIX'] = ".native" + env_native['LIBSUFFIX']

for platform in env_native.platforms:

	env = env_native.platforms[platform]
	env.bin_targets=[]
	env.lib_link_order=[]
	env.lib_objects={}
	env.lib_list=[]

	if env_native['game']:
		env.Append(TOLUAFLAGS=['-E', 'GAME=$game', '-E', 'GAME_BINDINGS=$game/game_bindings.pkg'])
		env.Append(CPPPATH=['#$game'])

		env['GAME'] = env_native['game']
	
	Export('env');
	
	#build subdirs, the build order is dependent on link order.
	SConscript("types/SCsub");
	SConscript("os/SCsub");
	SConscript("fileio/SCsub");
	SConscript("audio/SCsub");
	SConscript("collision/SCsub");
	SConscript("physics/SCsub");
	SConscript("gui/SCsub");
	SConscript("gui_bindings/SCsub");
	SConscript("main/SCsub");
	SConscript("animation/SCsub");
	SConscript("render/SCsub");
	SConscript("scene/SCsub");
	SConscript("tools/SCsub");
	SConscript("drivers/SCsub");
	if env_native['game']:
		SConscript(env_native['game']+'/SCsub')
	SConscript("lua/SCsub");
	SConscript("bin/SCsub");

	#build platform
	SConscript("build/"+platform+"/SCsub"); # build selected platform

#if (env['prefix']):
#         dst_target = env.Install(env['prefix']+'/bin', env.bin_targets)
#        env.Alias('install', dst_target )

