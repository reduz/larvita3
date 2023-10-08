#ifndef COPYMEM_H
#define COPYMEM_H

///@TODO use optimized routines for this, depending on platform. these are just the standard ones

#define copymem(m_to,m_from,m_count) \
	do {			 	\
		unsigned char * _from=(unsigned char*)m_from;	\
		unsigned char * _to=(unsigned char*)m_to;	\
		int _count=m_count;		\
		for (int _i=0;_i<_count;_i++)	\
			_to[_i]=_from[_i];	\
	} while (0);
	/*
					case 0:	*_dto++ = *_dfrom++;	\
					case 7:	*_dto++ = *_dfrom++;	\
							case 6:	*_dto++ = *_dfrom++;	\
									case 5:	*_dto++ = *_dfrom++;	\
										case 4:	*_dto++ = *_dfrom++;	\
										case 3:	*_dto++ = *_dfrom++;	\
										case 2:	*_dto++ = *_dfrom++;	\
										case 1:	*_dto++ = *_dfrom++;	\
	*/
#define movemem_duff(m_to, m_from, m_count) \
	do { \
		if (m_to<m_from) {	\
			unsigned char* _dto = (unsigned char*)m_to;	\
			unsigned char* _dfrom = (unsigned char*)m_from;	\
			int n = (m_count + 7) / 8;	\
			switch (m_count % 8) {	\
				do {					\
					case 0:	*_dto++ = *_dfrom++;	\
					case 7:	*_dto++ = *_dfrom++;	\
					case 6:	*_dto++ = *_dfrom++;	\
					case 5:	*_dto++ = *_dfrom++;	\
					case 4:	*_dto++ = *_dfrom++;	\
					case 3:	*_dto++ = *_dfrom++;	\
					case 2:	*_dto++ = *_dfrom++;	\
					case 1:	*_dto++ = *_dfrom++;	\
				} while (--n > 0);			\
			};						\
		} else if (m_to>m_from) {					\
			unsigned char* _dto = &((unsigned char*)m_to)[m_count-1];	\
			unsigned char* _dfrom = &((unsigned char*)m_from)[m_count-1];	\
			int n = (m_count + 7) / 8;	\
			switch (m_count % 8) {	\
				do {	\
					case 0:	*_dto-- = *_dfrom--;	\
					case 7:	*_dto-- = *_dfrom--;	\
					case 6:	*_dto-- = *_dfrom--;	\
					case 5:	*_dto-- = *_dfrom--;	\
					case 4:	*_dto-- = *_dfrom--;	\
					case 3:	*_dto-- = *_dfrom--;	\
					case 2:	*_dto-- = *_dfrom--;	\
					case 1:	*_dto-- = *_dfrom--;	\
				} while (--n > 0);			\
			};						\
		}                           				\
	} while(0)                           				\

#define movemem_conventional(m_to,m_from,m_count) \
	do {			 	\
		if (m_to<m_from) {					\
			unsigned char * _from=(unsigned char*)m_from;	\
			unsigned char * _to=(unsigned char*)m_to;	\
			int _count=m_count;				\
			for (int _i=0;_i<_count;_i++)			\
				_to[_i]=_from[_i];			\
									\
		} else if (m_to>m_from) {				\
			unsigned char * _from=(unsigned char*)m_from;	\
			unsigned char * _to=(unsigned char*)m_to;	\
			int _count=m_count;				\
			while (_count--) 				\
				_to[_count]=_from[_count];		\
									\
									\
		}							\
	} while (0);							\

void movemem_system(void*,void*,int);

#define movemem movemem_system

#endif

