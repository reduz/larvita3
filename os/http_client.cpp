#include "http_client.h"
#ifdef SSL_ENABLED
#include "fileio/ssl_client.h"
#endif

#include <stdio.h>
#include "os/memory.h"
#include "os/copymem.h"
#include "types/dvector.h"
#include <string.h>

//extern int tcp_printf(const char* format, ...);
//#define printf tcp_printf

int HTTPClient::connect(String p_host, int p_port) {

	int ret = client->connect_to_host(p_host, p_port);

	if (ret == 0) {

		connection.host = p_host;
		connection.port = p_port;

		connection.status = STATUS_CONNECTED;
	};

	return ret;
};

String to_base_64(String in) {

	//printf("transforming %s\n",in.ascii().get_data());
	union base64Transformer {
		char chars[3];
		struct {
			unsigned b1:6;
			unsigned b2:6;
			unsigned b3:6;
			unsigned b4:6; };
	};
	char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";
	int len = in.length() / 3;
	int extra = in.length() % 3;
	base64Transformer buf;
	String ret;

	//printf("len=%d,extra=%d\n",len,extra);

	int i;
	for(i = 0 ; i < len ; i++) {

		int start_char = 3*i;
		for(int j = 0 ; j < 3 ; j++) {
			buf.chars[j] = in.ascii().get_data()[start_char + 2-j];
		};
		ret += alphabet[buf.b4];
		ret += alphabet[buf.b3];
		ret += alphabet[buf.b2];
		ret += alphabet[buf.b1];
	}
	switch(extra) {
		case 1:
			buf.chars[2] = in[3*i];
			ret += alphabet[buf.b4];
			ret += alphabet[buf.b3];
			ret += alphabet[64];
			ret += alphabet[64];
			break;
		case 2:
			buf.chars[2] = in[3*i];
			buf.chars[1] = in[3*i + 1];
			ret += alphabet[buf.b4];
			ret += alphabet[buf.b3];
			ret += alphabet[buf.b2];
			ret += alphabet[64];
			break;
		default:
			break;
	}

	return ret;
}

int HTTPClient::get(String p_path, int p_range_from, int p_range_to, bool p_block) {

	if (connection.status != STATUS_CONNECTED) {
		ERR_PRINT("Error: not connected (or already sent a request)");
		return 1;
	};

	// stuff here
	String request;
	request = "GET "+p_path+" HTTP/1.1\r\n";
	request += "Host: "+connection.host+"\r\n";

	if (p_range_from != -1 && p_range_to > p_range_from) {
		request += "Range: bytes="+String::num(p_range_from)+"-"+String::num(p_range_to)+"\r\n";
	};

	// not keep alive
	request += "Connection: close\r\n";

	if(this->user != "") {

		//printf("accessing with %s:%s\n",user.ascii().get_data(),pass.ascii().get_data());
		//printf("%s\n",to_base_64(this->user + ":" + this->pass).ascii().get_data());
		request += "Authorization: Basic " + to_base_64(this->user +":"+ this->pass) + "\r\n";
	}

	request += "\r\n\r\n";

	//printf("** sending request:\n%s", (char*)request.ascii());

	send_offset = 0;
	data_to_send.resize(request.size());
	data_to_send.write_lock();
	memcpy(data_to_send.write(), request.ascii().get_data(), request.size());
	data_to_send.write_unlock();

	connection.status = STATUS_SENDING;

	if (!p_block) {

		return 0;
	};

	while (connection.status != STATUS_DATA_READY) {

		poll();
		//sleep(1);
		if (connection.status == STATUS_ERROR) {
			printf("header so far is \n%s\n", header.header.ascii().get_data());
			ERR_PRINT("Error recieving headers");
			return 1;
		};
	};
	// headers are on

	return 0;
};

int HTTPClient::post(String p_path, Table<String,Variant> p_fields) {
	const String MULTIPART_BOUNDARY("----------YvEVPKsYKTikMhHmd4ZFHA");

	DVector<char> data;

	String host = connection.host;
	String entity = p_path;

	// action overrides p_path
	if (p_fields.has("action")) {

		String action = p_fields["action"].get_string().get_slice("//",1);
		host = action.get_slice("/",0);
		entity = action.substr(action.find("/") + 1, action.length());
	};

	String header;
	String content;

	int header_length = 0;

	header = "POST " + entity + " HTTP/1.1\r\n";
	header += "Host: " + host + "\r\n";
	header += "Connection: close\r\n";

	if(this->user != "") {

		header += "Authorization: Basic " + to_base_64(this->user +":"+ this->pass) + "\r\n";
	}

	header += "Content-Type: multipart/form-data; boundary=" + MULTIPART_BOUNDARY + "\r\n";
	header += "Content-Length: "; //+ content_length + "\r\n\r\n";

	header_length += header.length();

	const int length_reserved_bytes = 14;

	// I alloc enough space for the header plus 14 bytes for the length of the bundle size string
	// resize returns true on error
	ERR_FAIL_COND_V(data.resize(header_length + length_reserved_bytes ),-1); // if I can't alloc the memory needed so far, return with error

	int data_start = data.size() + 2;

	const String* field = p_fields.next(NULL);
	int old_size = data.size();
	while(field)
	{
		int field_size = 0;

		if(p_fields[*field].get_type() == Variant::BYTE_ARRAY)
		{
			content = "--" + MULTIPART_BOUNDARY + "\r\n";
			content += "Content-Disposition: form-data; name=\"" + *field + "\"";
			content += "; filename=\"" + *field + "\"\r\n";
			content += "Content-Type: application/octet-stream\r\n\r\n";

			field_size += p_fields[*field].size() + content.length() + 2;
			//field_size += content.length();

			// I get the space for the binary data
			ERR_FAIL_COND_V(data.resize(old_size + field_size),-1);
			data.write_lock();
			// copy the header from the old last position
			CharString ascii = content.ascii();
			copymem(&data.write()[old_size],ascii.get_data(),content.length());
			old_size += content.length();

			// and then the binary data from the end of the header
			// hopefully this uses copy-on-write
			DVector<Uint8> bytes = p_fields[*field].get_byte_array();
			bytes.read_lock();
			copymem(&data.write()[old_size], bytes.read(), bytes.size());
			bytes.read_unlock();

			copymem(&data.write()[old_size + p_fields[*field].size()], "\r\n", 2);

			data.write_unlock();
			//	*/
		} else {
			content = "--" + MULTIPART_BOUNDARY + "\r\n";
			content += "Content-Disposition: form-data; name=\"" + *field + "\"";
			content += "\r\n\r\n";
			content += p_fields[*field].get_string();
			content += "\r\n";
			field_size += content.length();

			ERR_FAIL_COND_V(data.resize(old_size + field_size),-1);
			data.write_lock();
			CharString ascii = content.ascii();
			copymem(&data.write()[old_size],ascii.get_data(),field_size);
			data.write_unlock();
		}

		old_size = data.size();

		field = p_fields.next(field);
	}

	String final = "--" + MULTIPART_BOUNDARY + "--\r\n";
	ERR_FAIL_COND_V(data.resize(old_size + final.length()),-1);
	data.write_lock();

	CharString ascii = final.ascii();
	copymem(&data.write()[old_size],ascii.get_data(),final.length());

	int length_bytes = data.size() - data_start;
	String len_str = String::num(length_bytes) + String("\r\n\r\n");
	header += len_str;
	int start_pos = length_reserved_bytes - len_str.size();

	ascii = header.ascii();
	copymem(&data.write()[start_pos],ascii.get_data(),header.length());

	data_to_send.resize(data.size() - start_pos);
	data_to_send.write_lock();
	memcpy(data_to_send.write(), &data.write()[start_pos], data.size() - start_pos);
	data_to_send.write_unlock();
	send_offset = 0;

	data.write_unlock();

	connection.status = STATUS_SENDING;

	return 0;
}

void HTTPClient::poll_header() {

	static const char header_delim[] = "\r\n\r\n";
	static int delim_size = sizeof(header_delim)-1;

	// more stuff here
	Uint8 byte;
	int data_read;
	do {

		data_read = client->read(&byte, 1, false); // block reading 1 byte
		if (data_read == -1) {
			ERR_PRINT("Error reading header");
			connection.status = STATUS_ERROR;
			return;
		};
		if (data_read == 0) return;

		if (byte == header_delim[header_pos]) {
			++header_pos;
		} else {
			header_pos = 0;
		};

		header.header += byte;

		if (header_pos == delim_size) {
			break;
		};
	} while (data_read);

	if (header_pos == delim_size) {

		parse_header();
	};
};

void HTTPClient::parse_header() {

	int pos = 0;
	int next;
	String line;
	printf("header is \n%s", header.header.ascii().get_data());

	if (header.header.substr(0, 5) != "HTTP/") {
		connection.status = STATUS_ERROR;
		return;
	};
	header.proto_version = header.header.substr(5, 3);

	header.http_status = header.header.substr(9, 3).to_int();

	if (header.http_status != 200 && header.http_status != 206) {
		connection.status = STATUS_ERROR;
		return;
	};

	pos = header.header.find("\n", 1)+1;
	next = header.header.find("\n", pos);
	while (next >= 0 && next < header.header.size()-2) {

		line = header.header.substr(pos, (next - pos) - 1); // no \r\n
		if (line == "") continue;

		int sep = line.find(":");
		if (sep < 0) {
			connection.status = STATUS_ERROR;
			return;
		};
		String name = line.substr(0, sep);
		String value = line.substr(sep+2, line.size() - sep);

		if (name.nocasecmp_to("content-type") == 0) {
			header.content_type = value;
		};

		if (name.nocasecmp_to("content-length") == 0) {
			connection.data_left = value.to_int();
			if (header.http_status == 200) {
				header.size = connection.data_left;
			};
		};

		if (name.nocasecmp_to("content-range") == 0) {

			int delim = value.find("/");
			String total = value.substr(delim+1, value.size() - delim);
			if (total == "*") {
				// oh shit ?
			} else {
				header.size = total.to_int();
			};
		};

		pos = next+1;
		next = header.header.find("\n", pos);
	};

	connection.status = STATUS_DATA_READY;

	return;
};

void HTTPClient::poll_send() {

	data_to_send.read_lock();
	int sent = client->send_data((Uint8*)&data_to_send.read()[send_offset], data_to_send.size() - send_offset, false);
	if (sent == -1) {
ERR_PRINT("seld_data failed");
		connection.status = STATUS_ERROR;
	};
	send_offset += sent;
	data_to_send.read_unlock();

	if (send_offset == data_to_send.size()) {

		send_offset = 0;
		data_to_send.clear();
		connection.status = STATUS_WAITING_REPLY;
	};
};

int HTTPClient::poll() {

	int err = client->poll_data();

	if (err != 0) {
ERR_PRINT("poll_data failed");
		connection.status = STATUS_ERROR;
		return err;
	};

	if (connection.status == STATUS_SENDING) {

		poll_send();
		return 0;

	} else {

		if (connection.status != STATUS_DATA_READY) {

			poll_header();
		};
		return err;
	};

};


HTTPClient::Status HTTPClient::get_status() {

	return connection.status;
};

float HTTPClient::get_progress(float up_ratio, float down_ratio) {

	switch (connection.status) {

	case STATUS_SENDING:
		return 	(float)send_offset / (float)data_to_send.size() * up_ratio;

	case STATUS_WAITING_REPLY:
		return up_ratio;
	case STATUS_DATA_READY:
		return up_ratio + ( (float)(header.size - connection.data_left) / (float)header.size * down_ratio);

	default:
		return 0;
	};
};

int HTTPClient::get_http_status() {

	return this->header.http_status;
};

int HTTPClient::data_left() {

	return connection.data_left;
};

int HTTPClient::get_data(Uint8* p_dst, int p_len) {

	ERR_FAIL_COND_V(connection.status != STATUS_DATA_READY, -1);

	int read = client->read(p_dst, p_len, false);

	if (read == -1 && connection.data_left > p_len) {
		// error
		ERR_PRINT("Error reading data");
		connection.status = STATUS_ERROR;
	} else {
		connection.data_left -= read;
	};

	return read;
};

int HTTPClient::get_total_file_size() {

	ERR_FAIL_COND_V(connection.status != STATUS_DATA_READY, -1);

	return header.size;
};

void HTTPClient::close() {

	client->disconnect();
	connection.status = STATUS_NONE;
	send_offset = 0;
	data_to_send.resize(0);
	header.http_status = 0;
};

void HTTPClient::set_username_password(String user, String pass) {

	this->user = user;
	this->pass = pass;
}

HTTPClient::HTTPClient(String user, String pass, bool https) {


	#ifdef SSL_ENABLED
	if(https) {
		client = memnew(SSL_Client);
	} else {
		client = TCP_Client::create();
	};
	#else
		client = TCP_Client::create();
	#endif

	connection.status = STATUS_NONE;
	connection.data_left = 0;

	header_pos = 0;

	this->user = user;
	this->pass = pass;
};

HTTPClient::~HTTPClient() {

	if (client) {
		memdelete(client);
	};
};


