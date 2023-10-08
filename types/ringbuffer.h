#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include "vector.h"

template <typename T>
class Ringbuffer {

	Vector<T> data;
	int read_pos;
	int write_pos;
	int size_mask;
	
	inline int inc(int& p_var, int p_size) {
		int ret = p_var;
		p_var += p_size;
		p_var = p_var&size_mask;
		return ret;
	};
	
public:

	T read() {
		ERR_FAIL_COND_V(space_left() < 1, T());
		return data[inc(read_pos, 1)];
	};
	
	int read(T* p_buf, int p_size, bool p_advance=true) {
		int left = data_left();
		p_size = MIN(left, p_size);
		int pos = read_pos;
		int to_read = p_size;
		int dst = 0;
		while(to_read) {
			int end = pos + to_read;
			end = MIN(end, size());
			int total = end - pos;
			for (int i=0; i<total; i++) {
				p_buf[dst++] = data[pos + i];
			};
			to_read -= total;
			pos = 0;
		};
		if (p_advance) {
			inc(read_pos, p_size);
		};
		return p_size;
	};
	
	inline int advance_read(int p_n) {
		p_n = MIN(p_n, data_left());
		inc(read_pos, p_n);
		return p_n;
	};
	
	Error write(const T& p_v) {
		ERR_FAIL_COND_V( space_left() < 1, FAILED);
		data[inc(write_pos, 1)] = p_v;
		return OK;
	};
	
	int write(const T* p_buf, int p_size) {
		
		int left = space_left();
		p_size = MIN(left, p_size);
		
		int pos = write_pos;
		int to_write = p_size;
		int src = 0;
		while (to_write) {

			int end = pos + to_write;
			end = MIN(end, size());
			int total = end - pos;
			
			for (int i=0; i<total; i++) {
				data[pos+i] = p_buf[src++];
			};
			to_write -= total;
			pos = 0;
		};

		inc(write_pos, p_size);
		return p_size;
	};
	
	inline int space_left() {
		int left = read_pos - write_pos;
		if (left < 0) {
			return size() + left;
		};
		if (left == 0) {
			return size();
		};
		return left;
	};
	inline int data_left() {
		return size() - space_left();
	};
	
	inline int size() {
		return data.size();
	};
	
	void resize(int p_power) {
		int old_size = size();
		int new_size = 1<<p_power;
		int mask = new_size - 1;
		data.resize(1<<p_power);
		if (old_size < new_size && read_pos > write_pos) {
			for (int i=0; i<write_pos; i++) {
				data[(old_size + i)&mask] = data[i];
			};
			write_pos = (old_size + write_pos) & mask;
		} else {
			read_pos = read_pos & mask;
			write_pos = write_pos & mask;
		};
		
		size_mask = mask;
	};
	
	Ringbuffer<T>(int p_power) {
		read_pos = 0;
		write_pos = 0;
		resize(p_power);
	};
	~Ringbuffer<T>() {};
};

#endif
