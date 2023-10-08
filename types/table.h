//
// C++ Interface: table
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef TABLE_H
#define TABLE_H

#include "hashfuncs.h"
#include "error_macros.h"
#include "rstring.h"
#include "os/memory.h"

class TableHahserDefault {
public:
	
	static inline Uint32 hash(const String &p_string)  { return p_string.hash(); }
	static inline Uint32 hash(const char *p_cstr)  { return hash_djb2(p_cstr); }
	static inline Uint32 hash(const Uint64 p_int) { return (Uint32)p_int ^ (Uint32)(p_int >> 32); };
	static inline Uint32 hash(const Sint64 p_int) { return (Uint32)p_int ^ (Uint32)(p_int >> 32); };
	static inline Uint32 hash(const Uint32 p_int)  { return p_int; }
	static inline Uint32 hash(const Sint32 p_int)  { return (Uint32)p_int; }
	static inline Uint32 hash(const Uint16 p_int)  { return p_int; }
	static inline Uint32 hash(const Sint16 p_int)  { return (Uint32)p_int; }
	static inline Uint32 hash(const Uint8 p_int)  { return p_int; }
	static inline Uint32 hash(const Sint8 p_int)  { return (Uint32)p_int; }
};

/**
 * @class Table
 * @author Juan Linietsky <reduzio@gmail.com> 
 * 
 * Implementation of a standard Hashing Table, for quick lookups of Data associated with a Key.
 * The implementation provides hashers for the default types, if you need a special kind of hasher, provide
 * your own.
 * @param TKey  Key, search is based on it, needs to be hasheable. It is unique in this container.
 * @param TData Data, data associated with the key
 * @param Hasher Hasher object, needs to provide a valid static hash function for TKey
 * @param MIN_HASH_TABLE_POWER Miminum size of the hash table, as a power of two. You rarely need to change this parameter.
 * @param RELATIONSHIP Relationship at which the hash table is resized. if amount of elements is RELATIONSHIP
 * times bigger than the hash table, table is resized to solve this condition. if RELATIONSHIP is zero, table is always MIN_HASH_TABLE_POWER.
 * 
*/

template<class TKey, class TData, class Hasher=TableHahserDefault,Uint8 MIN_HASH_TABLE_POWER=3,Uint8 RELATIONSHIP=16>
class Table {
public:	
	
	struct Pair {
			
		TKey key;
		TData data;
		
		Pair() {}
		Pair(const TKey& p_key, const TData& p_data) { key=p_key; data=p_data; }
	};
	
		
private:	
	struct Entry {
		
		Uint32 hash;
		Entry *next;
		Pair pair;
		
		Entry() { next=0; }
	};
	
	Entry **hash_table;
	Uint8 hash_table_power;
	Uint32 elements;

	
	void make_hash_table() {
		
		ERR_FAIL_COND( hash_table );
		
		
		hash_table = (Entry**)Memory::alloc_static( sizeof(Entry*)*(1<<MIN_HASH_TABLE_POWER),"HashTable::Entry**");
		
		hash_table_power = MIN_HASH_TABLE_POWER;
		elements=0;
		for (int i=0;i<(1<<MIN_HASH_TABLE_POWER);i++)
			hash_table[i]=0;
	}
	
	void erase_hash_table() {
	
		ERR_FAIL_COND(elements);
		
		memfree( hash_table );
		hash_table=0;
		hash_table_power=0;		
		elements=0;	
	}	
	
	void check_hash_table() {
			return;		
#if 0		
		int new_hash_table_power=-1;
	
		if (elements > ( (1<<hash_table_power) * RELATIONSHIP ) ) {
			/* rehash up */
			new_hash_table_power=hash_table_power+1;
		
			while( elements > ( (1<<new_hash_table_power) * RELATIONSHIP ) ) {
		
				new_hash_table_power++;
			}
	
		} else if ( (hash_table_power>MIN_HASH_TABLE_POWER) && (elements < ( (1<<(hash_table_power-1)) * RELATIONSHIP )  )  ) {
	
			/* rehash down */
			new_hash_table_power=hash_table_power-1;
		
			while( elements < ( (1<<(new_hash_table_power-1)) * RELATIONSHIP ) ) {
		
				new_hash_table_power--;
			}
	
			if (new_hash_table_power<MIN_HASH_TABLE_POWER)
				new_hash_table_power=MIN_HASH_TABLE_POWER;
		}
		
		
		if (new_hash_table_power==-1)
			return;
		
		Entry ** new_hash_table = memnew_arr( Entry*, (1<<new_hash_table_power) );
		if (!new_hash_table) {
	
			ERR_PRINT("Out of Memory");
			return;
		}
	
		for (unsigned int i=0;i<(1<<new_hash_table_power);i++) {
	
			new_hash_table[i]=0;
		}
	
		for (unsigned int i=0;i<(1<<hash_table_power);i++) {
	
			while( hash_table[i] ) {
		
				Entry *se=hash_table[i];
				hash_table[i]=se->next;
				int new_pos = se->hash & ((1<<new_hash_table_power)-1);
				se->next=new_hash_table[new_pos];
				new_hash_table[new_pos]=se;
			}
		
		}
	
		memdelete_arr( hash_table );
		hash_table=new_hash_table;
		hash_table_power=new_hash_table_power;	
#endif
	}


	/* I want to have only one function.. */
	const Entry * get_entry( const TKey& p_key ) const {
		
		Uint32 hash = Hasher::hash( p_key );
		Uint32 index = hash&((1<<hash_table_power)-1);
		
		Entry *e = hash_table[index];
		
		while (e) {
			
			/* checking hash first avoids comparing key, which may take longer */
			if (e->hash == hash && e->pair.key == p_key ) {
				
				/* the pair exists in this hashtable, so just update data */
				return e;
			}
			
			e=e->next;
		}
				
		return NULL;
	}
	
	Entry * create_entry(const TKey& p_key) {
				
		/* if entry doesn't exist, create it */
		Entry *e = memnew( Entry );
		ERR_FAIL_COND_V(!e,NULL); /* out of memory */
		Uint32 hash = Hasher::hash( p_key );
		Uint32 index = hash&((1<<hash_table_power)-1);
		e->next = hash_table[index];	
		e->hash = hash;
		e->pair.key=p_key;
				
		hash_table[index]=e;
		elements++;
		
		return e;
	}
	
	
	void copy_from(const Table& p_t) {
		
		if (&p_t==this)
			return; /* much less bother with that */
		
		clear();
		
		if (!p_t.hash_table || p_t.hash_table_power==0)
			return; /* not copying from empty table */
		
		hash_table = (Entry**)Memory::alloc_static( (1<<p_t.hash_table_power)*sizeof(Entry*),"HashTable::Entry**");
		hash_table_power=p_t.hash_table_power;
		elements=p_t.elements;
		
		for (int i=0;i<( 1<<p_t.hash_table_power );i++) {
			
			hash_table[i]=NULL;
			/* elements will be in the reverse order, but it doesn't matter */
			
			const Entry *e = p_t.hash_table[i];
			
			while(e) {
				
				Entry *le = memnew( Entry ); /* local entry */
				
				*le=*e; /* copy data */
				
				/* add to list and reassign pointers */
				le->next=hash_table[i];
				hash_table[i]=le;
				
				e=e->next;
			}
			
			
		}
		
		
	}
public:
	
	
	void set( const TKey& p_key, const TData& p_data ) {
	
		set( Pair( p_key, p_data ) );
		
	}
	
	void set( const Pair& p_pair ) {
		
		if (!hash_table)
			make_hash_table(); // if no table, make one
		else
			check_hash_table(); // perform mantenience routine
		
		/* As said, i want to have only one get_entry */
		Entry *e = const_cast<Entry*>( get_entry(p_pair.key) );
		
		/* if we made it up to here, the pair doesn't exist, create and assign */
		
		if (!e) {
			
			e=create_entry(p_pair.key);
			if (!e)
				return;
		}
		
		e->pair.data = p_pair.data;
		
	}
	
	
	bool has( const TKey& p_key ) const {
		
		return getptr(p_key)!=NULL;
	}
	
	/**
	 * Get a key from data, return a const reference. 
	 * WARNING: this doesn't check errors, use either getptr and check NULL, or check
	 * first with has(key)
	 */
	
	const TData& get( const TKey& p_key ) const {
		
		const TData* res = getptr(p_key);
		ERR_FAIL_COND_V(!res,*res);
		return *res;
	}
	
	/**
	 * Same as get, except it can return NULL when item was not found.
	 * This is mainly used for speed purposes.
	 */
	
	const TData* getptr( const TKey& p_key ) const {
		
		if (!hash_table)
			return NULL;
		
		const Entry *e=get_entry(p_key );
		
		if (e)
			return &e->pair.data;
		
		return NULL;

	}
	
	TData* getptr( const TKey& p_key ) {
		
		if (!hash_table)
			return NULL;
		
		Entry *e=const_cast<Entry*>(get_entry(p_key ));
		
		if (e)
			return &e->pair.data;
		
		return NULL;

	}
	
	/**
	 * Erase an item, return true if erasing was succesful
	 */
	
	bool erase( const TKey& p_key ) {
		
		if (!hash_table)
			return false;
		
		Uint32 hash = Hasher::hash( p_key );
		Uint32 index = hash&((1<<hash_table_power)-1);
		
		
		Entry *e = hash_table[index];
		Entry *p=NULL;
		while (e) {
			
			/* checking hash first avoids comparing key, which may take longer */
			if (e->hash == hash && e->pair.key == p_key ) {
				
				if (p) {
					
					p->next=e->next;
				} else { 
					//begin of list
					hash_table[index]=e->next;
				}
				
				memdelete(e);
				elements--;
				
				if (elements==0)
					erase_hash_table();
				else
					check_hash_table();		
				return true;
			}
			
			p=e;
			e=e->next;
		}
		
		
		return false;
		
	}
	
	const TData& operator[](const TKey& p_key) const { //constref
		
		return get(p_key);
	}
	TData& operator[](const TKey& p_key ) { //assignment
		
		if (!hash_table)
			make_hash_table(); // if no table, make one
		else
			check_hash_table(); // perform mantenience routine
			
		Entry *e = const_cast<Entry*>( get_entry(p_key) );
		
		/* if we made it up to here, the pair doesn't exist, create */
		if (!e) {
			
			e=create_entry(p_key);
			if (!e)
				return *(TData*)NULL; /* panic! */
		}
		
		return e->pair.data;
		
	}
	
	/** 
	 * Get the next key to p_key, and the first key if p_key is null. 
	 * Returns a pointer to the next key if found, NULL otherwise.
	 * Adding/Removing elements while iterating will, of course, have unexpected results, don't do it.
	 * 
	 * Example:
	 * 
	 * 	const TKey *k=NULL;
	 * 
	 * 	while( (k=table.next(k)) ) {
	 * 
	 * 		print( *k );
	 * 	}
         *	
	*/
	const TKey* next(const TKey* p_key) const {
		
		if (!hash_table) return NULL;
		
		if (!p_key) { /* get the first key */
			
			for (int i=0;i<(1<<hash_table_power);i++) {
				
				if (hash_table[i]) {
					return &hash_table[i]->pair.key;
				}
			}

		} else { /* get the next key */
			
			const Entry *e = get_entry( *p_key );
			ERR_FAIL_COND_V( !e, false ); /* invalid key supplied */
			
			if (e->next) {
				/* if there is a "next" in the list, return that */
				return &e->next->pair.key;
			} else {
				/* go to next entries */
				Uint32 index = e->hash&((1<<hash_table_power)-1);
				index++;
				for (int i=index;i<(1<<hash_table_power);i++) {
				
					if (hash_table[i]) {
						return &hash_table[i]->pair.key;
					}
				}
			}
				
			/* nothing found, was at end */
			
		}
		
		
		return NULL; /* nothing found */
	}
	
	unsigned int size() const {
		
		return elements;
	}
	
	bool empty() const {
		
		return elements==0;
	}
	
	void clear() {
		
		/* clean up */
		if (hash_table) {
			for (int i=0;i<(1<<hash_table_power);i++) {
				
				while (hash_table[i]) {
								
					Entry *e=hash_table[i];
					hash_table[i]=e->next;
					memdelete( e );
				}
			}
		
			memfree( hash_table );
		}
		
		hash_table=0;
		hash_table_power=0;
		elements=0;		
	}
	
	
	void operator=(const Table& p_table) {
		
		copy_from(p_table);
	}
	
	Table() { 
		hash_table=NULL;
		elements=0;
		hash_table_power=0;
	}
	
	Table(const Table& p_table) { 
	
		hash_table=NULL;
		elements=0;
		hash_table_power=0;
		
		copy_from(p_table);
		
	}
	
	~Table() {
		
		clear();
	}

};

#endif
