#ifdef WINDOWS_ENABLED

#include "tcp_client_win32.h"

#include "os/memory.h"

int winsock_refcount = 0;
void winsock_init() {

	//*
	if (winsock_refcount == 0) {

		WSADATA data;
		WSAStartup(MAKEWORD(2,2), &data);
	};
	//	*/

	++winsock_refcount;
};

void winsock_close() {

	--winsock_refcount;
	if (!winsock_refcount) {
		WSACleanup();
	};
};


void TcpClientWin32::set_as_default() {

	TCP_Client::create_func = TcpClientWin32::create;
};

TCP_Client* TcpClientWin32::create() {

	return memnew(TcpClientWin32);
};


Error TcpClientWin32::connect_to_host(String p_host,int p_port) {

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
}

void TcpClientWin32::connect_socket(Host p_host) {

	if (p_host == 0) {
		disconnect();
	};

	if ((sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) {
		ERR_PRINT("Socket creation failed!");
		disconnect();
		//perror("socket");
		return;
	}

	unsigned long par = 1;
	if (ioctlsocket(sockfd, FIONBIO, &par)) {
		perror("setting non-block mode");
		disconnect();
		return;
	};

	their_addr.sin_family = AF_INET;    // host byte order
	their_addr.sin_port = htons(port);  // short, network byte order
	their_addr.sin_addr = *((struct in_addr*)&p_host);
	memset(&(their_addr.sin_zero), '\0', 8);
	errno = 0;
	if (connect(sockfd, (struct sockaddr *)&their_addr,sizeof(struct sockaddr)) == SOCKET_ERROR) {

		if (WSAGetLastError() != WSAEWOULDBLOCK) {

			ERR_PRINT("Connection to remote host failed!");
			disconnect();
			return;
		};
	}

	status = STATUS_CONNECTING;

	return;
}


int TcpClientWin32::send_data(Uint8* p_data,int p_length, bool p_block) {

	if (status == STATUS_NONE || status == STATUS_ERROR) {

		return -1;
	};
	if (status != STATUS_CONNECTED) {
		return 0;
	};

	int data_to_send=p_length;
	Uint8 * offset=p_data;
	if (sockfd==INVALID_SOCKET) return -1;

	while (data_to_send) {


		int sent_amount=send(sockfd,(char*)offset,data_to_send, 0);
		if (!p_block && sent_amount == -1 && WSAGetLastError()==WSAEWOULDBLOCK) {
			return 0;
		};

		while ((sent_amount==-1) && (WSAGetLastError()==WSAEWOULDBLOCK)) {

			sent_amount=send(sockfd,(char*)offset,data_to_send, 0);

		}

		if (sent_amount==-1) {

			perror("shit?: ");
			disconnect();

			return -1;
		}

		if (!p_block) return sent_amount;

		data_to_send-=sent_amount;
		offset+=sent_amount;
	}

	return p_length;
}


void TcpClientWin32::disconnect() {

	if (sockfd != INVALID_SOCKET)
		closesocket(sockfd);
	sockfd=INVALID_SOCKET;

	if (resolver_id != RESOLVER_INVALID_ID && status == STATUS_RESOLVING) {
		Resolver::get_singleton()->abort(resolver_id);
	};

	status = STATUS_NONE;
	resolver_id = RESOLVER_INVALID_ID;
}


int TcpClientWin32::poll_data() {

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

		if (connect(sockfd, (struct sockaddr *)&their_addr,sizeof(struct sockaddr)) == SOCKET_ERROR) {

			int err = WSAGetLastError();
			if (err == WSAEISCONN) {
				status = STATUS_CONNECTED;
			};
			return 0;
		};
		return 1;
	};
	case STATUS_CONNECTED:
		if (sockfd==INVALID_SOCKET) {
			ERR_PRINT("Trying to poll when not connected!\n");
			return 1;
		}
	};

	return 0;
}

int TcpClientWin32::get_local_port() {
	struct sockaddr_in addr;
	int addr_len = sizeof(addr);
	getsockname(sockfd, (struct sockaddr *)&addr, &addr_len);
	return ntohs(addr.sin_port);
}

int TcpClientWin32::read(Uint8* p_buffer, int p_size, bool p_block) {

	if (status == STATUS_NONE || status == STATUS_ERROR) {

		return -1;
	};
	if (status != STATUS_CONNECTED) {
		return 0;
	};

	if (sockfd==INVALID_SOCKET) {
		ERR_PRINT("Trying to poll when not connected!\n");
		return -1;
	}

	int to_read = p_size;
	int total_read = 0;
	while (to_read) {

		int read = recv(sockfd, (char*)(p_buffer + total_read), to_read, 0);
		if (read > 0) {
			to_read -= read;
			total_read += read;
		};

		if (read == -1) {

			if (WSAGetLastError() == WSAEWOULDBLOCK) {
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
};

bool TcpClientWin32::is_connected() {

	if (status == STATUS_NONE || status == STATUS_ERROR) {

		return false;
	};
	if (status == STATUS_RESOLVING) {
		return true;
	};

	return (sockfd!=INVALID_SOCKET);
}

TcpClientWin32::TcpClientWin32(){

	status = STATUS_NONE;
	sockfd=INVALID_SOCKET;

	winsock_init();
}
TcpClientWin32::~TcpClientWin32(){

	winsock_close();
}

#endif
