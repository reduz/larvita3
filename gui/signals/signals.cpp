//
// C++ Implementation: signals
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "signals.h"

namespace GUI {

int SignalBase::connection_count = 0;

bool SignalBase::register_at_target( SignalTarget *p_target ) {
	
	return p_target->_register_signal(this);
}
void SignalBase::remove_from_target( SignalTarget *p_target ) {
	
	p_target->_clear_signal(this);
	
}


bool SignalTarget::_register_signal( SignalBase *p_signal ) {

	Connection *c=_conn_list;

	while (c) {

		if (c->signal==p_signal) {
			
			c->refcount++;
			return true;
		}

		c=c->next;
	}

	c = GUI_NEW(Connection);

	c->next=_conn_list;
	c->signal=p_signal;
	_conn_list=c;
	
	return false;
}
void SignalTarget::_clear_signal( SignalBase *p_signal ) {


	Connection **pc=&_conn_list; //previous
	Connection *c=_conn_list;

	while (c) {

		if (c->signal==p_signal) {
			
			c->refcount--;
			if (c->refcount<=0) {
				*pc=c->next;
				GUI_DELETE(c);
			}
			
			return; // just be connected only once anyway
		}
		pc=&c->next;
		c=c->next;
	}

}

void SignalTarget::clear_connections() {

	while (_conn_list) {

		Connection *c=_conn_list;
		_conn_list=_conn_list->next;
		c->signal->remove_target( this );
		GUI_DELETE(c);
	}		
	
};

SignalTarget::SignalTarget() {

	_conn_list=0; //no connections to here
}
SignalTarget::~SignalTarget() {

	clear_connections();
}


}