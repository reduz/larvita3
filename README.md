# Larvita3
Larvita 3 Game Engine

This game engine is the precursor of [Godot Engine](http://www.godotengine.org) developed by Ariel Manzur and Juan Linietsky.
It was used for a couple of games and prototypes, most notably (Atmosphir)[https://www.youtube.com/watch?v=DvQsmPC27VU].

![larvita2](https://github.com/reduz/larvita3/assets/6265307/6687cd24-0751-4e11-b779-8acfd3db93e9)
![image](https://github.com/reduz/larvita3/assets/6265307/2ffce16d-e9e6-4ba7-bed4-ee00cacd8bf0)



## Compilation

### Windows

#### Prerequisites

The following tools are required to compile Larvita3 on Windows:

* Python 2.7
* Scons 2.5.1
* Microsoft Visual C++ 2008 Express
* SDL 1.2 for Visual C

Compiling with mingw or any newer versions of Visual Studio is untested.

#### Instructions

!! THESE INSTRUCTIONS WILL NEED TO BE UPDATED IF THE REQUIREMENTS ARE CHANGED !!

Install Python 2.7 to `C:\Python27` or wherever works best for you.

Install SCons build system 2.5.1 from [SourceForge](https://sourceforge.net/projects/scons/files/scons/2.5.1/) to Python 2.7.

Make sure `C:\Python27\` and `C:\Python27\Scripts\` are in your system PATH. You should be able to type `scons -ver` into CMD to see if it worked.

Download SDL-devel-1.2.13-VC8.zip from [SourceForge](https://sourceforge.net/projects/libsdl/files/SDL/1.2.13/) and extract the contents of the folder within to `C:\SDL\`. `C:\SDL\include\` and `C:\SDL\lib\` should exist in those places if done correctly.

Install Visual C++ 2008 Express from [here](http://download.microsoft.com/download/8/B/5/8B5804AD-4990-40D0-A6AA-CE894CBBB3DC/VS2008ExpressENUX1397868.iso]). Make sure you install it to the C drive!

Next, make sure you have the repository cloned or downloaded some placed on your computer.

Run "Visual Studio 2008 Command Prompt" by searching for it in windows. If it doesn't show up, you can also run `"C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\vcvarsall.bat" x86` in CMD to bring it up.

Change the directory of CMD to the larvita3 folder you just downloaded. You should be able to just run `scons` in the command prompt at the root of the larvita3 directory to compile everything.

If all goes well, you now have a working Larvita3 engine compilation.

## Usage

The editor is written in Lua (this is a Lua game engine).
You open the editor like this:
```
cd bin
script_exec editor
```

To execute games:

```
script_exec game_folder
```

Games are implemented by inheriting SceneMainLoop from Lua, then doing whathever you want.

## Documentation

There is none.

## Origins

Our previous engine was called "Larvotor" (used for Regnum Online). This was originally a 3D engine aimed to run on the Nintendo 3DS, hence the name "Larvita" (Larvita 1 was a prototype engine for that platform). Life happened and we ended up getting to work for clients wanting to make PC games.

An explanation of Larvita 3 design and how we moved from it to Godot can be [read here](https://gist.github.com/reduz/9b9d1278848237fd9a9a8b6cc77c8270).

