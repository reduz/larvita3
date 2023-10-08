class("ViewPort", GUI.Lua__Widget)

function ViewPort:mouse_button(p_pos, p_button,p_press,p_modifier_mask)

	if not self.event_reciever then
		return
	end
	
	self.event_reciever:mouse_button(p_pos, p_button, p_press, p_modifier_mask)
end

function ViewPort:mouse_doubleclick(p_pos,p_modifier_mask)

	if not self.event_reciever then
		return
	end
	self.event_reciever:mouse_doubleclick(p_pos,p_modifier_mask)
end

function ViewPort:mouse_motion(p_pos, p_rel, p_button_mask)

	if not self.event_reciever then
		return
	end
	self.event_reciever:mouse_motion(p_pos, p_rel, p_button_mask)
end

function ViewPort:key(p_unicode, p_scan_code,p_press,p_repeat,p_modifier_mask)

	if not self.event_reciever then
		return
	end
	self.event_reciever:key(p_unicode, p_scan_code,p_press,p_repeat,p_modifier_mask)
end

function ViewPort:set_event_reciever(res)
	self.event_reciever = res
end

function ViewPort:__init__()

	self:set_focus_mode(GUI.FOCUS_CLICK)
	self:get_focus()
end