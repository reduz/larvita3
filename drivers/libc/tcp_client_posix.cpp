#ifdef POSIX_ENABLED

#include "tcp_client_posix.h"

#include "os/memory.h"

#define INFO(a,b) printf(a " " b)

#if defined(OSX_ENABLED) || defined(IPHONE_ENABLED)
	#define MSG_NOSIGNAL	SO_NOSIGPIPE
#endif

void TcpClientPosix::set_as_default() {
	
	TCP_Client::create_func = TcpClientPosix::create;
};

TCP_Client* TcpClientPosix::create() {
	
	return memnew(TcpClientPosix);
};


Error TcpClientPosix::connect_to_host(String p_host,int p_port) {

	printf("Connecting to host %s:%d\n", p_host.ascii().get_data(), p_port);
	if (is_connected()) {
		ERR_PRINT("Trying to connect when already connected!\n");
		return FAILED;
	}

	status = STATUS_RESOLVING;
	sockfd = -1;

	Resolver* r = Resolver::get_singleton();
	resolver_id = r->lookup_hostname(p_host);
	
	if (resolver_id == RESOLVER_INVALID_ID) {
		
		status = STATUS_ERROR;
		return FAILED;
	};
	
	port = p_port;

	return poll_data()!=0?FAILED:OK;
};

void TcpClientPosix::connect_socket(Host p_host) {

	if (p_host == 0) {
		disconnect();
	};
	
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		ERR_PRINT("Socket creation failed!");
		disconnect();
		//perror("socket");
		return;
	}

	fcntl(sockfd, F_SETFL, O_NONBLOCK);

	their_addr.sin_family = AF_INET;    // host byte order
	their_addr.sin_port = htons(port);  // short, network byte order
	their_addr.sin_addr = *((struct in_addr*)&p_host);
	memset(&(their_addr.sin_zero), '\0', 8);
	errno = 0;
	if (connect(sockfd, (struct sockaddr *)&their_addr,sizeof(struct sockaddr)) == -1 && errno != EINPROGRESS) {
		
		ERR_PRINT("Connection to remote host failed!");
		disconnect();
		return;
	}
	INFO("TCP Client Posix","Connected!\n");

	if (errno == EINPROGRESS) {
		status = STATUS_CONNECTING;
	} else {
		status = STATUS_CONNECTED;
	};
	
	return;
}

int TcpClientPosix::send_data(Uint8* p_data,int p_length, bool p_block) {

	if (status == STATUS_NONE || status == STATUS_ERROR) {
	
		return -1;
	};
	if (status != STATUS_CONNECTED) {
		return 0;
	};
	
	int data_to_send = p_length;
	Uint8 *offset = p_data;
	if (sockfd == -1) return -1;
	errno = 0;
	
	while (data_to_send) {
		int sent_amount = send(sockfd, offset, data_to_send, MSG_NOSIGNAL);
		//printf("Sent TCP data of %d bytes, errno %d\n", sent_amount, errno);
		
		if (!p_block && sent_amount == -1 && errno == EAGAIN) {
			return 0;
		};
		
		while ((sent_amount == -1) && (errno == EAGAIN)) {
			errno = 0;
			//printf("Retrying to send TCP data of %d bytes\n", data_to_send, offset);
			sent_amount = send(sockfd, offset, data_to_send, MSG_NOSIGNAL);
			usleep(1); //sucks
		}
		if (sent_amount == -1) {
			perror("shit?");
			close(sockfd);
			sockfd = -1;
			ERR_PRINT("Server disconnected!\n");
			return -1;
		}
		data_to_send -= sent_amount;
		offset += sent_amount;
		
		if (!p_block) return sent_amount;
	}
	return p_length;
}

int TcpClientPosix::get_local_port(){
	struct sockaddr_in addr;
	int addr_len = sizeof(addr);
	getsockname(sockfd, (struct sockaddr *)&addr, (socklen_t *)&addr_len);
	return ntohs(addr.sin_port);
}

void TcpClientPosix::disconnect() {
	if (sockfd != -1)
		close(sockfd);
	sockfd=-1;
	
	if (resolver_id != RESOLVER_INVALID_ID && status == STATUS_RESOLVING) {
		Resolver::get_singleton()->abort(resolver_id);
	};
	
	status = STATUS_NONE;
	resolver_id = RESOLVER_INVALID_ID;
}

int TcpClientPosix::poll_data() {

	errno = 0;
	
	switch (status) {
		
	case STATUS_NONE:
	case STATUS_ERROR:
		return 1;
		
	case STATUS_RESOLVING: {
		Resolver::Status s = Resolver::get_singleton()->poll(resolver_id);

		if (s == Resolver::STATUS_ERROR) {
			disconnect();
			return 1;
		};
		if (s == Resolver::STATUS_DONE) {
			
			connect_socket(Resolver::get_singleton()->retrieve(resolver_id));
		};
		return 0;
	};
	case STATUS_CONNECTING: {

		if (connect(sockfd, (struct sockaddr *)&their_addr,sizeof(struct sockaddr)) == -1) {

			if (errno == EISCONN) {
				status = STATUS_CONNECTED;
			};
			return 0;
		} else {
			status = STATUS_CONNECTED;
			return 0;
		};
	};
	case STATUS_CONNECTED:
		if (sockfd==-1) {
			ERR_PRINT("Trying to poll when not connected!\n");
			return 1;
		}
	};
	
	// nothing?
	
	return 0;
}

bool TcpClientPosix::is_connected() {
	
	if (status == STATUS_NONE || status == STATUS_ERROR) {
	
		return false;
	};
	if (status != STATUS_CONNECTED) {
		return true;
	};

	return (sockfd!=-1);
}

int TcpClientPosix::read(Uint8* p_buffer, int p_size, bool p_block) {

	if (status == STATUS_NONE || status == STATUS_ERROR) {
	
		return -1;
	};
	if (status != STATUS_CONNECTED) {
		return 0;
	};

	if (sockfd==-1) {
		ERR_PRINT("Trying to poll when not connected!\n");
		return -1;
	}

	int to_read = p_size;
	int total_read = 0;
	while (to_read) {
		
		int read = recv(sockfd, p_buffer + total_read, to_read, 0);
		if (read > 0) {
			to_read -= read;
			total_read += read;
		};
		if (read == -1) {
			
			if (errno == EAGAIN) {
				if (!p_block) {
					return total_read;
				};
			} else {
				
				// error reading
				ERR_PRINT("Error reading");
				return -1;
			};
		};
		
		if (read == 0) {

			//ERR_PRINT("Connection reset by peer");
			disconnect();
			return total_read;
		};
	};
	
	return total_read;
}

TcpClientPosix::TcpClientPosix(){

	sockfd=-1;
	status = STATUS_NONE;
}
TcpClientPosix::~TcpClientPosix(){

	disconnect();
}

#endif // POSIX_ENABLED

