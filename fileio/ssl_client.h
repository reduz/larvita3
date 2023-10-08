#ifndef _ENCRYPTOR_H_
#define _ENCRYPTOR_H_

#include <openssl/ssl.h>
#include "os/http_client.h"

#define BIO_TYPE_LARVITA ( 22 | 0x0400 | 0x0100 )

BIO_METHOD *BIO_s_larvita_client(void);
BIO *BIO_new_larvita_client(TCP_Client*);

class SSL_Client: public TCP_Client {
private:
	TCP_Client* client;
	static SSL_CTX* ctx;
	BIO* bio;
	SSL* ssl;

	static void initialize_ctx();

	enum Status {
		STATUS_NONE,
		STATUS_HANDSHAKING,
		STATUS_READY,
	};
	Status status;
	
public:
	virtual Error connect_to_host(String p_host,int p_port);

	/*
	void set_connect_port(int p_port) { this->port = p_port; }
	void set_connect_host(String p_host) { this->host = p_host; }
	Error connect() { return this->connect_to_host(this->host,this->port); }
	*/
	virtual int send_data(Uint8* p_data,int p_length, bool p_block); ///< returns bytes sent or -1 on error
	virtual int poll_data();							///< returns != 0  on error
	virtual int read(Uint8* p_buffer, int p_size, bool p_block = true); ///< returns bytes read, -1 on error
	virtual void disconnect();
	virtual bool is_connected();
	virtual int get_local_port();

	static void set_certificates(char* certs_path);
	
	SSL_Client();
	virtual ~SSL_Client();

} ;


#endif

