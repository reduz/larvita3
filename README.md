# Larvita3
Larvita 3 Game Engine

This game engine is the precursor of [Godot Engine](http://www.godotengine.org) developed by Ariel Manzur and Juan Linietsky.
It was used for a couple of games and prototypes, most notably (Atmosphir)[https://www.youtube.com/watch?v=DvQsmPC27VU].

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
