#ifdef POSIX_ENABLED

#include "udp_connection_posix.h"
#include "list.h"

#include <errno.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/fcntl.h>
#include <netinet/in.h>
#include <stdio.h>

void UDPConnectionPosix::set_as_default() {

	UDPConnection::create_func = UDPConnectionPosix::create;
};

UDPConnection* UDPConnectionPosix::create() {

	return memnew(UDPConnectionPosix);
};

void UDPConnectionPosix::close() {
	if (sockfd != -1)
		::close(sockfd);
};

void UDPConnectionPosix::set_listen_port(int p_port) {
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

int UDPConnectionPosix::get_socket() {
	
	if (sockfd != -1)
		return sockfd;
	
	sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	ERR_FAIL_COND_V( sockfd == -1, -1 );
	//fcntl(sockfd, F_SETFL, O_NONBLOCK);
	
	return sockfd;
};

Error UDPConnectionPosix::send(const NetworkPeer& destination, Uint8* p_msg, int p_size) {

	int sock = get_socket();
	ERR_FAIL_COND_V( sock == -1, FAILED );
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(destination.port);
	addr.sin_addr = *((struct in_addr*)&destination.host);
	
	errno = 0;
	int err;
	while ( (err = sendto(sock, p_msg, p_size, 0, (struct sockaddr*)&addr, sizeof(addr))) != p_size) {
		
		if (errno != EAGAIN) {
			return FAILED;
		};
	};

	return OK;	
};

int UDPConnectionPosix::get_message_count() {

	return queue_count;
};

int UDPConnectionPosix::get_message_size() {

	ERR_FAIL_COND_V(queue_count < 1, -1);
	int size[3];
	rb.read((Uint8*)&size, 12, false);
	return size[2];
};

int UDPConnectionPosix::read_message(NetworkPeer& sender, Uint8* p_buf, Uint32 p_buf_size) {
	
	Uint32 size = get_message_size();
	ERR_FAIL_COND_V(size > p_buf_size, -1);
	rb.read((Uint8*)&sender.host, 4);
	rb.read((Uint8*)&sender.port, 4);
	rb.read(p_buf, 4); // size
	--queue_count;
	return rb.read(p_buf, size);
};

Error UDPConnectionPosix::poll() {

	struct sockaddr_in from = {0};
	socklen_t len = sizeof(struct sockaddr_in);
	int ret;
	while ( (ret = recvfrom(sockfd, recv_buffer, sizeof(recv_buffer), MSG_DONTWAIT, (struct sockaddr*)&from, &len)) > 0) {
		rb.write((Uint8*)&from.sin_addr, 4);
		Uint32 port = ntohs(from.sin_port);
		rb.write((Uint8*)&port, 4);
		rb.write((Uint8*)&ret, 4);
		rb.write(recv_buffer, ret);

		len = sizeof(struct sockaddr_in);
		++queue_count;
	};

	if (ret == 0 || (ret == -1 && errno != EAGAIN) ) {
		close();
		return FAILED;
	};
	
	return OK;
};

UDPConnectionPosix::UDPConnectionPosix() : rb(BUFFER_SIZE_POWER_OF_2) {

	sockfd = -1;
	queue_count = 0;
};

UDPConnectionPosix::~UDPConnectionPosix() {
	close();
};

#endif
