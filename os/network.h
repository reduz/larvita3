#ifndef NETWORK_H
#define NETWORK_H

#include "typedefs.h"

typedef unsigned long Host;
struct NetworkPeer {
	Host host;
	int port;

	bool operator==(const NetworkPeer& p_r) {
		return p_r.host == host && p_r.port == port;
	};
	
	const NetworkPeer& operator=(const NetworkPeer& p_r) {
		host = p_r.host;
		port = p_r.port;
		return *this;
	};
	
	inline operator const void*() const {
		return host == 0 && port == -1 ? 0 : this;
	};
	
	static inline Uint32 hash(const NetworkPeer& p_np) {
		return p_np.host ^ p_np.port;
	};

	void clear() {
		host = 0;
		port = -1;
	};
	
	NetworkPeer() {
		clear();
	};
	NetworkPeer(const NetworkPeer& p_r) {
		operator=(p_r);
	};
	NetworkPeer(const Host p_h, int p_p) {
		host = p_h;
		port = p_p;
	};
};

#endif
