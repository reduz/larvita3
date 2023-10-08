#ifdef WINDOWS_ENABLED

#ifndef UDP_CONNECTION_WINDOWS
#define UDP_CONNECTION_WINDOWS

#include "os/udp_connection.h"

#include "table.h"
#include "ringbuffer.h"

class UDPConnectionWindows : public UDPConnection {

	enum {
		BUFFER_SIZE_POWER_OF_2=16, // 64k
		RECV_BUFFER_SIZE=4096, // 4k
	};
	
	Ringbuffer<Uint8> rb;
	Uint8 recv_buffer[RECV_BUFFER_SIZE];

	int sockfd;
	
	int get_socket();

	int queue_count;

	static UDPConnection* create();
	
public:

	void set_listen_port(int p_port);
	void close();
	Error send(const NetworkPeer& destination, Uint8* p_msg, int p_size);
	
	int get_message_count();
	int get_message_size();
	int read_message(NetworkPeer& sender, Uint8* p_buf, Uint32 p_buf_size);

	Error poll();

	static void set_as_default();
	
	UDPConnectionWindows();	
	~UDPConnectionWindows();
};

#endif

#endif
