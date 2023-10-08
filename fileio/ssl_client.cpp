#ifdef SSL_ENABLED

#include "os/memory.h"
#include "os/tcp_client.h"

#include <openssl/err.h>
#include <openssl/ssl.h>
#include "fileio/ssl_client.h"

static const char *CIPHERS_LIST = "DHE-RSA-AES256-SHA:DHE-DSS-AES256-SHA:AES256-SHA:EDH-RSA-DES-CBC3-SHA:EDH-DSS-DES-CBC3-SHA:DES-CBC3-SHA:DES-CBC3-MD5:DHE-RSA-AES128-SHA:DHE-DSS-AES128-SHA:AES128-SHA:RC2-CBC-MD5:RC4-SHA:RC4-MD5:RC4-MD5:EDH-RSA-DES-CBC-SHA:EDH-DSS-DES-CBC-SHA:DES-CBC-SHA:DES-CBC-MD5:EXP-EDH-RSA-DES-CBC-SHA:EXP-EDH-DSS-DES-CBC-SHA:EXP-DES-CBC-SHA:EXP-RC2-CBC-MD5:EXP-RC2-CBC-MD5:EXP-RC4-MD5:EXP-RC4-MD5";

static int larvita_write(BIO*, const char*, int);
static int larvita_read(BIO*, char*, int);
//static int larvita_puts(BIO*, const char*, int);
//static int larvita_gets(BIO*, char*, int);
//static int larvita_ctrl(BIO*, int, long, void*);
static int larvita_new(BIO*);
static int larvita_free(BIO*);
static BIO_METHOD larvita_method = {
	BIO_TYPE_LARVITA,
	"larvita socket",
	larvita_write,
	larvita_read,
	NULL,
	//larvita_puts,
	NULL,
	//larvita_gets,
	NULL,
	//larvita_ctrl,
	larvita_new,
	larvita_free,
	NULL,
};

static int larvita_write(BIO* bio, const char* buf, int len) {

	printf("larvita_write(");
	for(int i = 0 ; i < len ; i++)
		printf("%u:",buf[i]);
	printf(")\n");

	int ret;
	//clear_sys_error();
	ret = ((TCP_Client*)bio->ptr)->send_data((Uint8*)buf,len, false);
	printf("sent %i bytes\n", ret);
	BIO_clear_retry_flags(bio);
	if(ret == 0) {
		BIO_set_retry_write(bio);
		return -1;
	}

	return ret;
}

static int larvita_read(BIO* b, char* out, int len) {

	int ret = 0;

	BIO_clear_retry_flags(b);
	ret = ((TCP_Client*)b->ptr)->read((Uint8*)out,len, false);
	printf("read %i bytes\n", ret);
	if(ret == 0) {

		BIO_set_retry_read(b);
		return -1;
	}
	return ret;
}
/*
static int larvita_puts(BIO*, const char*, int) {
	return 0;
}

static int larvita_gets(BIO*, char*, int) {
	return 0;
}

static int larvita_ctrl(BIO* bio, int cmd, long num, void *ptr) {

}
*/

static int larvita_new(BIO* bio) {

	bio->init = 1;
	bio->num = 0;
	bio->ptr = NULL;
	bio->flags = 0;
	return 1;
}

static int larvita_free(BIO* bio) {

	if(bio == NULL) return 0;
	if(bio->shutdown) {

		if(bio->init) {

			if(bio->ptr) {

				memdelete((TCP_Client*)bio->ptr);
				bio->ptr = NULL;
			}
		}
		bio->init = 0;
		bio->flags = 0;
	}
	return 1;
}

BIO_METHOD *BIO_s_larvita_client(){
	return (&larvita_method);
}

BIO *BIO_new_larvita_client(TCP_Client* client){
	BIO* ret = NULL;

	if((ret = BIO_new(BIO_s_larvita_client()))) {

		ret->ptr = client;
	}
	return ret;
}

Error SSL_Client::connect_to_host(String p_host,int p_port) {

	printf("ssl connecting to host %s:%d\n",p_host.ascii().get_data(),p_port);
	Error ret = this->client->connect_to_host(p_host,p_port);
	printf("connected? err: %d\n",ret);
	
	if(ret == OK) {

		status = STATUS_HANDSHAKING;
	} else {
		status = STATUS_NONE;
	};

	return ret;
}

int SSL_Client::send_data(Uint8* p_data,int p_length, bool p_block) {

	int ret = SSL_write(this->ssl,(void*)p_data,p_length);
	int err = SSL_get_error(this->ssl,ret);
	switch(err) {

		case SSL_ERROR_NONE:

			break;
		case SSL_ERROR_WANT_WRITE:
		case SSL_ERROR_WANT_READ:

			ret = 0;
			break;
		default:

			ERR_PRINT("Unexpected SSL error");
			printf("%s\n",ERR_error_string(err,NULL));
			return -1;
			break;
	}
	return ret;
}

int SSL_Client::poll_data() {

	int ret = this->client->poll_data();

	if (!client->is_connected()) {
ERR_PRINT("client disconnected");
		return ret;
	};
	
	if (status == STATUS_HANDSHAKING) {

		int err = SSL_connect(ssl);

		if (err <= 0) {

			switch((err = SSL_get_error(this->ssl,err))) {

				case SSL_ERROR_WANT_READ:
					break;
				case SSL_ERROR_WANT_WRITE:
					break;
				default:
					client->disconnect();
					ERR_PRINT("failed during handshake");
					printf("%s\n",ERR_error_string(err,NULL));
					return 1;
					break;
			}
		} else {
			
			status = STATUS_READY;
			printf("checking certificate\n");
			//if(SSL_get_verify_result != X509_V_OK) {
				//ret = ERR_INVALID_HOSTNAME;
			//	ERR_PRINT("INVALID_HOSTNAME");
			//}
		};
	} else {

		
	};

	
	
	return 0;
}

int SSL_Client::read(Uint8* p_buffer, int p_size, bool p_block) {

	int ret = SSL_read(this->ssl,p_buffer,p_size);

	int err = SSL_get_error(this->ssl,ret);
	switch(err) {

		case SSL_ERROR_NONE:

			break;
		case SSL_ERROR_ZERO_RETURN:

			this->disconnect();
			ret = -1;
			ERR_PRINT("failed reading");
			break;
		case SSL_ERROR_WANT_READ:
		case SSL_ERROR_WANT_WRITE:

			ret = 0;
			break;
		default:

			ERR_PRINT("Unexpected SSL error");
			printf("%s\n",ERR_error_string(err,NULL));
			break;
	}
	return ret;
}

void SSL_Client::disconnect() {

	int ret = SSL_shutdown(this->ssl);
	if( ret == 0)
	{
		int err;
		while((err = SSL_shutdown(this->ssl)) == 0);
		if(err != 1)
			ERR_PRINT("ERROR SHUTTING DOWN SSL CONNECTION");
	} else if(ret != 1) {

		ERR_PRINT("ERROR SHUTTING DOWN SSL CONNECTION");
	}
}

bool SSL_Client::is_connected() {

	return this->client->is_connected();
}

int SSL_Client::get_local_port() {

	return this->client->get_local_port();
}

void SSL_Client::set_certificates(char* certs_path) {

	if(!SSL_CTX_load_verify_locations(SSL_Client::ctx,NULL,certs_path))
		printf("SSL_Client ERROR loading certificates");
}

SSL_Client::~SSL_Client() {

	if(this->client)
		memdelete(this->client);
}

SSL_CTX* SSL_Client::ctx = NULL;

void SSL_Client::initialize_ctx() {

	SSL_library_init();
	SSL_load_error_strings();
	ERR_load_crypto_strings();
	SSL_Client::ctx = SSL_CTX_new(SSLv23_method());
	SSL_CTX_set_cipher_list(SSL_Client::ctx,CIPHERS_LIST);

	printf("*****************setting certs\n");
	//SSL_Client::set_certificates("/home/soundwave/projects/pc_atmosphir/trunk/src/game2/atmo/certs");
	//SSL_CTX_set_verify(SSL_Client::ctx,SSL_VERIFY_PEER,NULL);
}

SSL_Client::SSL_Client() {

	if(!SSL_Client::ctx)
		SSL_Client::initialize_ctx();

	this->client = TCP_Client::create();
	this->ssl = SSL_new(this->ctx);
	this->bio = BIO_new_larvita_client(this->client);
	SSL_set_bio(this->ssl,this->bio,this->bio);
	SSL_set_connect_state(this->ssl);
	status = STATUS_NONE;
}

#endif
