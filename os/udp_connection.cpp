#include "udp_connection.h"

UDPConnection*(*UDPConnection::create_func)()=0;	

bool UDPConnection::can_create() {
	return create_func != 0;
};

UDPConnection* UDPConnection::create() {
	
	ERR_FAIL_COND_V( !create_func, 0 );
	
	return create_func();
	
}

UDPConnection::~UDPConnection() {
	
};

