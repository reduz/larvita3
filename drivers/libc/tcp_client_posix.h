#ifdef POSIX_ENABLED

#ifndef TCP_CLIENT_POSIX_H
#define TCP_CLIENT_POSIX_H


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "os/tcp_client.h"
#include "os/resolver.h"

/**
  *@author
  */


#define MAXDATASIZE 300

class TcpClientPosix : public TCP_Client  {

	enum Status {
		
		STATUS_NONE,
		STATUS_RESOLVING,
		STATUS_CONNECTING,
		STATUS_CONNECTED,
		STATUS_ERROR,
	};
	
	int sockfd, numbytes;
	struct hostent *he;
	struct sockaddr_in their_addr; // connector's address information
	int port;
	ResolverID resolver_id;
	Status status;

	void connect_socket(Host p_host);

public:

	Error connect_to_host(String p_host,int p_port);
	
	int send_data(Uint8* p_data,int p_length, bool p_block = true); ///< returns bytes sent or -1 on error
	int poll_data();							///< returns != 0  on error
	int read(Uint8* p_buffer, int p_size, bool p_block = true); ///< returns bytes read, -1 on error
	void disconnect();
	bool is_connected();
	int get_local_port();
	
	static void set_as_default();
	static TCP_Client* create();

	TcpClientPosix();
	~TcpClientPosix();
};

#endif
#endif
