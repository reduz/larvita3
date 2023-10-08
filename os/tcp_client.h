#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include "typedefs.h"
#include "rstring.h"

class TCP_Client {
protected:
	
	static TCP_Client*(*create_func)();	
public:
	virtual Error connect_to_host(String p_host,int p_port)=0;
	
	virtual int send_data(Uint8* p_data,int p_length, bool p_block = true)=0; ///< returns bytes sent or -1 on error
	virtual int poll_data()=0;							///< returns != 0  on error
	virtual int read(Uint8* p_buffer, int p_size, bool p_block = true)=0; ///< returns bytes read, -1 on error
	virtual void disconnect()=0;
	virtual bool is_connected()=0;
	virtual int get_local_port() = 0;
	
	virtual ~TCP_Client() {};
	
	static TCP_Client *create();
};

#endif

