
#ifdef WINDOWS_ENABLED

#ifndef TCP_CLIENT_WIN32_H
#define TCP_CLIENT_WIN32_H


#include <stdio.h>
#include <stdlib.h>
// #include <unistd.h>
//#include <errno.h>

//#include <netdb.h>
//#include <sys/types.h>
//#include <sys/fcntl.h>
//#include <netinet/in.h>
//#include <sys/socket.h>

#include <winsock2.h>

#include "os/tcp_client.h"
#include "os/resolver.h"
#include "os/network.h"

/**
  *@author
  */


class TcpClientWin32: public TCP_Client  {

	enum Status {

		STATUS_NONE,
		STATUS_RESOLVING,
		STATUS_CONNECTING,
		STATUS_CONNECTED,
		STATUS_ERROR,
	};

	ResolverID resolver_id;
	Status status;
	int port;

	void connect_socket(Host p_host);

	unsigned int sockfd;
	struct hostent *he;
	struct sockaddr_in their_addr; // connector's address information

public:

	Error connect_to_host(String p_host,int p_port);
	int send_data(Uint8* p_data,int p_length, bool p_block = true);
	int poll_data();
	bool is_connected();
	int read(Uint8* p_buffer, int p_size, bool p_block = true);
	int get_local_port();

	void disconnect();

	static void set_as_default();
	static TCP_Client* create();

	TcpClientWin32();
	~TcpClientWin32();
};

#endif
#endif // win32_enabled
