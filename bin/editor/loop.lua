class("Loop", Lua__MainLoop)

function Loop:iteration(time)

	if self.game_loop then
		if self.game_loop:iteration(time) then
			self.quit=true;
		end
	end
	
	return self.quit
end

function Loop:get_iteration_frequency()
	
	if self.game_loop then
		return self.game_loop:get_iteration_frequency()
	end
	
	return 60
end

function Loop:draw()

	Renderer:get_singleton():render_clear( bw_or(Renderer.CLEAR_COLOR, Renderer.CLEAR_DEPTH), Color(80,80,100) )
	if self.game_loop then
		self.game_loop:draw();
	end
			
	local r=Renderer:get_singleton();
	r:render_set_matrix( Renderer.MATRIX_PROJECTION, Matrix4() )
	r:render_set_matrix( Renderer.MATRIX_CAMERA, Matrix4() )

	r:render_primitive_set_material( self.gui_material )

	local vm = Main:get_singleton():get_video_mode()
	local screen_size = GUI.Point(vm.width, vm.height )
				
	if screen_size ~= self.gui_main_window:get_size() then
		self.gui_main_window:set_size( screen_size )
		self.gui_painter:set_screen_size( screen_size )
	end
		
	local gui_mtx = Matrix4();
	gui_mtx:scale( 2.0 / screen_size.width, -2.0 / screen_size.height, 1.0 )
	gui_mtx:translate( -(screen_size.width / 2.0), -(screen_size.height / 2.0), 0.0 )

	r:render_set_matrix( Renderer.MATRIX_WORLD, gui_mtx )

	self.gui_main_window:check_for_updates()
	self.gui_main_window:redraw_all(GUI.Rect( GUI.Point(), screen_size ))

end

function Loop:request_quit(confirm)


	if confirm then
		self.quit = true
	else
		self.window:file_quit()
	end
end

function Loop:finish()

	print "finish"
end

function Loop:get_painter()

	return self.gui_painter
end

function Loop:get_gui_main_window()

	return self.gui_main_window
end

function Loop:event(p_event)

	self.gui_event_translator:push_event( p_event )
	if self.game_loop and self.game_loop:get_process_flags() ~= Node.MODE_FLAG_EDITOR then
		self.game_loop:event(p_event)
	end
end

function Loop:get_game_loop()

	return self.game_loop
end

function Loop:set_game_loop(p_game_loop)
	self.game_loop = p_game_loop or SceneMainLoop()
end

function Loop:init()

	self.window = self:get_gui_main_window()

	self:get_gui_main_window():init()
	if self.game_loop then
		self.game_loop:init()
		self.game_loop:set_process_flags(Node.MODE_FLAG_EDITOR)
	end
end

function Loop:__finalize__()

	self.gui_main_window:delete()
	self.game_loop:set_scene_tree(nil)
end

function Loop:__init__() -- constructor

	self.selection_updated = LuaSignal:new()

	self.gui_painter = PainterRenderer()
	self.gui_timer = GUI_Timer()
	self.gui_skin = GUI.Skin()
	self.gui_main_window = GUI.Window(self.gui_painter,self.gui_timer,self.gui_skin)
	self.gui_event_translator = GUI_EventTranslator(self.gui_main_window)
	
	self.gui_material = Renderer:get_singleton():get_gui_material()
end

return Loop
