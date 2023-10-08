#include "gui_signal_timer.h"

#include "gui/base/window.h"
#include "error_macros.h"

namespace GUI {

void SignalTimer::timeout() {
	
	timeout_signal.call();
};

int SignalTimer::get_interval(TimerID p_timer) {

	ERR_FAIL_COND_V(id == INVALID_TIMER_ID, -1);
	return window->get_timer()->get_interval(id);
};

void SignalTimer::change_timer_interval(TimerID p_timer,int p_interval_msec) {
	
	ERR_FAIL_COND(id == INVALID_TIMER_ID);
	window->get_timer()->change_timer_interval(id, p_interval_msec);
};

SignalTimer::SignalTimer(Window* p_window, int p_interval_msec) {
	
	window = p_window;
	id = window->get_timer()->create_timer(Method(this, &SignalTimer::timeout), p_interval_msec);
};

SignalTimer::~SignalTimer() {

	if (id != INVALID_TIMER_ID) {
		window->get_timer()->remove_timer(id);
	};
};

	
};
