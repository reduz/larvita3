#if defined(POSIX_ENABLED) || defined(WINDOWS_ENABLED)

#include "resolver.h"

#include "os/time.h"

#ifdef WINDOWS_ENABLED
#include <winsock2.h>
#else
#include <netdb.h>
#endif

#include <stdio.h>
#include <string.h>

Resolver* Resolver::singleton = NULL;

ResolverID Resolver::find_id() {

	for (int i=0; i<MAX_QUERIES; i++) {

		if (!queue[i].in_use || ( queue[i].aborted && queue[i].status != STATUS_WAITING )) {

			queue[i].clear();
			return i;
		};
	};

	return RESOLVER_INVALID_ID;
};

Resolver* Resolver::get_singleton() {

	return singleton;
};

Error Resolver::lookup_blocking(String p_name, Host& r_host) {

	struct hostent *he;
	printf("getting hostname...(%s)\n", p_name.ascii().get_data());
	if ((he=gethostbyname(p_name.ascii().get_data())) == NULL) {  // get the host info
		ERR_PRINT("gethostbyname failed!");
		//perror("gethostbyname");
		return FAILED;
	}
	r_host = *((unsigned long*)he->h_addr);

	return OK;
};

ResolverID Resolver::lookup_hostname(String p_name) {

	ResolverID id = find_id();
	ERR_FAIL_COND_V(id == RESOLVER_INVALID_ID, id);
	queue[id].clear();

	if (cache.has(p_name)) {

		queue[id].status = STATUS_DONE;
		queue[id].in_use = true;
		queue[id].response = cache[p_name];
		return id;
	};

	if (thread) {

		ERR_FAIL_COND_V( p_name.size() >= HOSTNAME_SIZE_MAX, RESOLVER_INVALID_ID );
		strcpy(queue[id].hostname, p_name.ascii().get_data());

		queue[id].in_use = true;
		queue[id].status = STATUS_WAITING;

		notify_event();

		return id;

	} else {

		Error err = lookup_blocking(p_name, queue[id].response);
		if (err == OK) {
			queue[id].in_use = true;
			queue[id].status = STATUS_DONE;
			cache[p_name] = queue[id].response;
			return id;
		} else {
			queue[id].clear();
			return RESOLVER_INVALID_ID;
		};
	};
};

Resolver::Status Resolver::poll(ResolverID p_id) {

	ERR_FAIL_INDEX_V(p_id, MAX_QUERIES, STATUS_ERROR);
	// nothing

	return queue[p_id].status;
};

Host Resolver::retrieve(ResolverID p_id) {

	ERR_FAIL_INDEX_V( p_id, MAX_QUERIES, 0 );
	ERR_FAIL_COND_V( !queue[p_id].in_use || queue[p_id].status != STATUS_DONE, 0 );

	Host host = queue[p_id].response;
	cache[queue[p_id].hostname] = host;

	queue[p_id].clear();

	return host;
};

void Resolver::abort(ResolverID p_id) {

	ERR_FAIL_INDEX( p_id, MAX_QUERIES );

	if (thread) {
		if (queue[p_id].in_use) {

			queue[p_id].aborted = true;
		};
	} else {

		queue[p_id].clear();
	};
};

/*************************************** thread stuff ***************************************/

void Resolver::resolver_thread() {

	printf("Resolver thread started!\n");
	thread_last_count = thread_counter;

	while (!thread_abort) {

		wait_for_events();

		process_events();
	};
	printf("Resolver thread ended!\n");
};

void Resolver::wait_for_events() {

	while (!thread_abort && thread_last_count == thread_counter) {

		OSTime::get_singleton()->delay_usec(500000); // sleep 0.5 secs
	};

	thread_last_count = thread_counter;
};

void Resolver::process_events() {

	for (int i=0; i<MAX_QUERIES; i++) {

		if (thread_abort) {
			return;
		};

		if (queue[i].status == STATUS_WAITING) {

			if (queue[i].aborted) {
				queue[i].status = STATUS_DONE;
				continue;
			};

			if (lookup_blocking(queue[i].hostname, queue[i].response) != OK) {

				queue[i].status = STATUS_ERROR;
			} else {

				queue[i].status = STATUS_DONE;
			};
		};
	};
};

void Resolver::notify_event() {

	++thread_counter; // atomic
};

/*************************************** end thread stuff ***************************************/

#ifdef WINDOWS_ENABLED
extern void winsock_init();
extern void winsock_close();
#endif


Resolver::Resolver() {

	singleton = this;

	thread_counter = 0;
	thread_abort = false;
	thread = Thread::create(Method(this, &Resolver::resolver_thread));

	#ifdef WINDOWS_ENABLED
	winsock_init();
	#endif
};

Resolver::~Resolver() {

	if (thread) {
		thread_abort = true;
		Thread::wait_to_finish(thread);
		memdelete(thread);
	};

	#ifdef WINDOWS_ENABLED
	winsock_close();
	#endif
};


#endif

