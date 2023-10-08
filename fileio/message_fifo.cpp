#include "message_fifo.h"

#include "marshalls.h"

Error MessageFIFO::queue_message(Uint8* p_msg, int p_size) {

	ERR_FAIL_COND_V( rb.space_left() < p_size + 4, FAILED );
	
	Message msg;
	msg.header.type = MESSAGE_TYPE_QUEUED;
	
	encode_uint16(message_count, (Uint8*)&msg.header.count);
	encode_uint16(p_size + sizeof(MessageHeader), (Uint8*)&msg.header.size);

	rb.write((Uint8*)&msg, sizeof(MessageHeader));
	rb.write((Uint8*)p_msg, p_size);
	
	++message_count;
	++queue_count;

	return OK;
};

int MessageFIFO::get_queue_size() {
	
	return rb.data_left();
};

int MessageFIFO::get_pending(Uint8* p_dst, int p_dst_size) {
	
	ERR_FAIL_COND_V( p_dst_size < rb.data_left(), -1 );
	
	return rb.read(p_dst, p_dst_size, false);
};

void MessageFIFO::ack(int p_count) {

	while ( queue_count && (get_first_message_count() <= p_count) ) {
		flush_first_message();
	};
};

/* For recievers */
int MessageFIFO::input_messages(Uint8* p_msg, Uint32 p_size) {
	
	unsigned int to_read = p_size;
	Message* msg = (Message*)p_msg;
	int queued = 0;
	while (to_read) {
		switch (msg->header.type) {

			case MESSAGE_TYPE_ACK: {
				ERR_FAIL_COND_V( sizeof(MessageHeader) > to_read, -1 );
				to_read -= sizeof(MessageHeader);
				rb.write((Uint8*)msg, sizeof(MessageHeader));
				msg = (Message*)(((Uint8*)msg) + sizeof(MessageHeader));
				++queue_count;
				break;
			};
				
			case MESSAGE_TYPE_QUEUED: {
				Uint16 count = decode_uint16((Uint8*)&msg->header.count);
				++queued;
				if (msg->header.type == MESSAGE_TYPE_QUEUED && count != message_count) {
					Uint16 size = decode_uint16((Uint8*)&msg->header.size);
					to_read -= size;
					msg = (Message*)(((Uint8*)msg) + size);
					break;
				};
				++message_count;
				/* FALLTHROUGH */
			};
			case MESSAGE_TYPE_INMEDIATE: {
				Uint16 size = decode_uint16((Uint8*)&msg->header.size);
				ERR_FAIL_COND_V( size > to_read, -1 );
				rb.write((Uint8*)msg, size);
				to_read -= size;
				msg = (Message*)(((Uint8*)msg) + size);
				++queue_count;
				break;
			};
			default:
				ERR_PRINT("invalid message type");;
		};
	};
	
	return queued;
};

int MessageFIFO::get_queue_count() {
	
	return queue_count;
};

int MessageFIFO::get_first_message_count() {
	ERR_FAIL_COND_V(queue_count < 1, -1 );
	Message msg;
	rb.read( (Uint8*)&msg, sizeof(MessageHeader), false );
	return decode_uint16((Uint8*)&msg.header.count);
};

void MessageFIFO::flush_first_message() {

	ERR_FAIL_COND(queue_count < 1);
	MessageHeader header;
	rb.read((Uint8*)&header, sizeof(MessageHeader), false);
	rb.advance_read(decode_uint16((Uint8*)&header.size));
	--queue_count;
};

int MessageFIFO::read(Uint8* p_buff, Uint32 p_buff_size) {
	ERR_FAIL_COND_V( queue_count < 1, -1 );
	
	Message* msg = (Message*)p_buff;
	rb.read( (Uint8*)msg, sizeof(MessageHeader), false );
	Uint16 size = decode_uint16((Uint8*)&msg->header.size);
	ERR_FAIL_COND_V(size > p_buff_size, -1);
	
	--queue_count;
	int ret = rb.read(p_buff, size);
	msg->header.size = size;
	msg->header.count = decode_uint16((Uint8*)&msg->header.count);

	return ret;
};

int MessageFIFO::get_message_count() {
	return message_count;
};

MessageFIFO::MessageFIFO(int power_of_2_size) : rb(power_of_2_size) {
	
	message_count = 0;
	queue_count = 0;	
};

MessageFIFO::~MessageFIFO() {
	
};

