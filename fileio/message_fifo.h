#ifndef MESSAGE_FIFO_H
#define MESSAGE_FIFO_H

#include "ringbuffer.h"

enum MessageType {
	
	MESSAGE_TYPE_ACK,
	MESSAGE_TYPE_QUEUED,
	MESSAGE_TYPE_INMEDIATE,
};

struct MessageHeader {
	Uint16 size;
	Uint8 type;
	Uint16 count;
};

struct Message {
	MessageHeader header;
	Uint8 payload[1];
};

class MessageFIFO {

public:
	enum {
		NON_QUEUED=0xffffffff,
	};
private:
	int message_count;

	Ringbuffer<Uint8> rb;
	int queue_count;
	
	void flush_first_message();
	
public:

	/* For senders */
	Error queue_message(Uint8* p_msg, int p_size);
	int get_queue_size();
	int get_pending(Uint8* p_dst, int p_dst_size);
	void ack(int p_count);

	/* For recievers */
	int input_messages(Uint8* p_msg, Uint32 p_size); ///< returns number of messages accepted
	int get_queue_count();
	int read(Uint8* p_buff, Uint32 p_buff_size);
	int get_first_message_count();

	int get_message_count();
	
	MessageFIFO(int power_of_2_size);
	~MessageFIFO();
};

#endif
