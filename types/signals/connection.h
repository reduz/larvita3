#ifndef CONNECTION_H
#define CONNECTION_H

#include "signals.h"

class Connection {
	
	int count;	
	SignalBase* signal;

public:
	void disconnect() {
		
		signal->disconnect(count);
	};

	Connection(SignalBase* p_s, int p_count) {
		signal = p_s;
		count = p_count;
	};
	
	~Connection() {};
};

#endif
