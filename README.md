# Larvita3
Larvita 3 Game Engine

This game engine is the precursor of [Godot Engine](http://www.godotengine.org) developed by Ariel Manzur and Juan Linietsky.
It was used for a couple of games and prototypes, most notably [Atmosphir](https://www.youtube.com/watch?v=DvQsmPC27VU).

![larvita2](https://github.com/reduz/larvita3/assets/6265307/6687cd24-0751-4e11-b779-8acfd3db93e9)
![image](https://github.com/reduz/larvita3/assets/6265307/2ffce16d-e9e6-4ba7-bed4-ee00cacd8bf0)



#### Compilation

I have no idea, please update instructions.
I think it went something like:

```
scons bin/script_exec
```

#### Usage

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

#### Documentation

There is none.

#### Origins

Our previous engine was called "Larvotor" (used for Regnum Online). This was originally a 3D engine aimed to run on the Nintendo 3DS, hence the name "Larvita" (Larvita 1 was a prototype engine for that platform). Life happened and we ended up getting to work for clients wanting to make PC games.

An explanation of Larvita 3 design and how we moved from it to Godot can be [read here](https://gist.github.com/reduz/9b9d1278848237fd9a9a8b6cc77c8270).

