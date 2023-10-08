#include "tcp_client.h"

TCP_Client* (*TCP_Client::create_func)()=0;


TCP_Client* TCP_Client::create() {
	
	ERR_FAIL_COND_V( !create_func, 0 );
	
	return create_func();
	
}
