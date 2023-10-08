#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include "types/table.h"
#include "types/variant.h"
#include "types/string/rstring.h"
#include "tcp_client.h"

class HTTPClient {
	
public:
	enum Status {

		STATUS_NONE,
		STATUS_CONNECTED,
		STATUS_SENDING,
		STATUS_WAITING_REPLY,
		STATUS_WAITING_DATA,
		STATUS_DATA_READY,
		STATUS_ERROR,
	};
	
private:

	TCP_Client* client;

	struct Header {

		String header;

		String proto_version;
		int http_status;
		int size;
		String content_type;
	} header;
	
	struct Connection {
		
		Status status;
		int data_left;
		String host;
		int port;
	} connection;

	String user;
	String pass;

	int header_pos;
	void poll_header();
	void parse_header();
	
	void poll_send();
	
	DVector<Uint8> data_to_send;
	int send_offset;
	
public:
	
	int connect(String p_host, int p_port = 80);
	int get(String p_path, int p_range_from=-1, int p_range_to=-1, bool p_block = true); ///< returns != 0 on error

	int post(String p_path, Table<String,Variant>);
	
	int poll();
	float get_progress(float up_ratio = 0.5, float down_ratio = 0.5);
	
	Status get_status();
	int get_http_status();
	int data_left();
	int get_data(Uint8* p_dst, int p_len); ///< returns bytes read, or -1 on error

	int get_total_file_size();

	void close(); /// close the current connection

	void set_username_password(String user, String pass);
	HTTPClient(String user="",String pass="",bool https=false);
	~HTTPClient();
};

#endif
