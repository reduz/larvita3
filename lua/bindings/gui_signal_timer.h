#ifndef GUI_SIGNAL_TIMER
#define GUI_SIGNAL_TIMER

#include "gui/base/timer.h"

#include "types/signals/signals.h"

namespace GUI {

class Window;
	
class SignalTimer : public SignalTarget {

	TimerID id;
	Window* window;
	
	void timeout();
	
public:
	
	Signal<> timeout_signal;
	
	int get_interval(TimerID p_timer);
	void change_timer_interval(TimerID p_timer,int p_interval_msec);
	
	SignalTimer(Window* p_window, int p_interval_msec);
	~SignalTimer();
};
	
};

#endif
