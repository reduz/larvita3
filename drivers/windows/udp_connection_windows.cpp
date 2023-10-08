#ifdef WINDOWS_ENABLED

#include "udp_connection_windows.h"
#include "list.h"


#include <winsock.h>

void UDPConnectionWindows::set_as_default() {

	UDPConnection::create_func = UDPConnectionWindows::create;
};

UDPConnection* UDPConnectionWindows::create() {

	return memnew(UDPConnectionWindows);
};

void UDPConnectionWindows::close() {
	if (sockfd != -1)
		::closesocket(sockfd);
};

void UDPConnectionWindows::set_listen_port(int p_port) {
	close();
	sockfd = get_socket();
	ERR_FAIL_COND( sockfd == -1 );
	sockaddr_in addr = {0};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(p_port);
	addr.sin_addr.s_addr = INADDR_ANY;
	ERR_FAIL_COND( bind(sockfd, (struct sockaddr*)&addr, sizeof(sockaddr_in)) == -1 );
	printf("UDP Connection listening on port %i\n", p_port);
};

int UDPConnectionWindows::get_socket() {

	if (sockfd != -1)
		return sockfd;

	sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	ERR_FAIL_COND_V( sockfd == -1, -1 );
	//fcntl(sockfd, F_SETFL, O_NONBLOCK);

	unsigned long par = 1;
	if (ioctlsocket(sockfd, FIONBIO, &par)) {
		perror("setting non-block mode");
		close();
		return -1;
	};

	return sockfd;
};

Error UDPConnectionWindows::send(const NetworkPeer& destination, Uint8* p_msg, int p_size) {

	int sock = get_socket();
	ERR_FAIL_COND_V( sock == -1, FAILED );
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(destination.port);
	addr.sin_addr = *((struct in_addr*)&destination.host);

	errno = 0;
	int err;
	while ( (err = sendto(sock, (const char*)p_msg, p_size, 0, (struct sockaddr*)&addr, sizeof(addr))) != p_size) {

		if (WSAGetLastError() != WSAEWOULDBLOCK) {
			return FAILED;
		};
	};

	return OK;
};

int UDPConnectionWindows::get_message_count() {

	return queue_count;
};

int UDPConnectionWindows::get_message_size() {

	ERR_FAIL_COND_V(queue_count < 1, -1);
	int size[3];
	rb.read((Uint8*)&size, 12, false);
	return size[2];
};

int UDPConnectionWindows::read_message(NetworkPeer& sender, Uint8* p_buf, Uint32 p_buf_size) {

	Uint32 size = get_message_size();
	ERR_FAIL_COND_V(size > p_buf_size, -1);
	rb.read((Uint8*)&sender.host, 4);
	rb.read((Uint8*)&sender.port, 4);
	rb.read(p_buf, 4); // size
	--queue_count;
	return rb.read(p_buf, size);
};

Error UDPConnectionWindows::poll() {

	struct sockaddr_in from = {0};
	int len = sizeof(struct sockaddr_in);
	int ret;
	while ( (ret = recvfrom(sockfd, (char*)recv_buffer, sizeof(recv_buffer), 0, (struct sockaddr*)&from, &len)) > 0) {
		rb.write((Uint8*)&from.sin_addr, 4);
		Uint32 port = ntohs(from.sin_port);
		rb.write((Uint8*)&port, 4);
		rb.write((Uint8*)&ret, 4);
		rb.write(recv_buffer, ret);

		len = sizeof(struct sockaddr_in);
		++queue_count;
	};

	if (ret == 0 || (ret == SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK) ) {
		close();
		return FAILED;
	};

	return OK;
};

extern void winsock_init();
extern void winsock_close();


UDPConnectionWindows::UDPConnectionWindows() : rb(BUFFER_SIZE_POWER_OF_2) {

	sockfd = -1;
	queue_count = 0;

	winsock_init();
};

UDPConnectionWindows::~UDPConnectionWindows() {
	close();

	winsock_close();
};

#endif
