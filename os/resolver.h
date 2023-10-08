#ifndef RESOLVER_H
#define RESOLVER_H

#include "os/network.h"
#include "types/string/rstring.h"

#include "os/thread.h"
#include "os/semaphore.h"
#include "types/table.h"

enum {
	RESOLVER_INVALID_ID = -1,
};

typedef int ResolverID;

class Resolver : public SignalTarget {

public:
	
	enum {
		MAX_QUERIES = 32,
		HOSTNAME_SIZE_MAX = 256,
	};
	
	enum Status {
		
		STATUS_NONE,
		STATUS_WAITING,
		STATUS_DONE,
		STATUS_ERROR,
	};
	
private:
	static Resolver* singleton;

	struct QueueItem {
		
		Status status;
		Host response;
		bool in_use;
		bool aborted;
		char hostname[HOSTNAME_SIZE_MAX];
		
		void clear() {
			status = STATUS_NONE;
			response = 0;
			in_use = false;
			aborted = false;
		};
		QueueItem() {
			clear();
		};
	};
	
	QueueItem queue[MAX_QUERIES];
	
	ResolverID find_id();
	
	Thread* thread;
	//Semaphore* semaphore;
	bool thread_abort;
	int thread_counter;
	int thread_last_count;

	void resolver_thread();
	void wait_for_events();
	void process_events();
	
	void notify_event();

	Table<String, Host> cache;
	
public:

	static Resolver* get_singleton();
	
	Error lookup_blocking(String p_name, Host& r_host);
	
	ResolverID lookup_hostname(String p_name);
	Status poll(ResolverID p_id);
	Host retrieve(ResolverID p_id);
	
	void abort(ResolverID p_id);

	Resolver();
	virtual ~Resolver();
};

#endif
