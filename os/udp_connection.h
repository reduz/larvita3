#ifndef UDP_CONNECTION
#define UDP_CONNECTION

#include "os/network.h"
#include "typedefs.h"

class UDPConnection {

protected:
	static UDPConnection*(*create_func)();	
	
public:

	virtual void set_listen_port(int p_port)=0;
	virtual Error send(const NetworkPeer& destination, Uint8* p_msg, int p_size)=0;
	
	virtual int get_message_count()=0;
	virtual int get_message_size()=0;
	virtual int read_message(NetworkPeer& sender, Uint8* p_buf, Uint32 p_buf_size)=0;

	virtual void close()=0;
	
	virtual Error poll()=0;

	static UDPConnection* create();
	static bool can_create();
	
	virtual ~UDPConnection();
};

#endif
