
//
// C++ Interface: signals
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SIGNALS_H
#define SIGNALS_H

#include "error_macros.h"
//#include "typedefs.h"

#include "os/memory.h"

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/



class SignalBase;


class SignalTarget {

	struct Connection {

		int refcount; //amount of times this signal is connected to methods of this class!
		
		
		SignalBase * signal;

		Connection *next;

		Connection() { next=0; signal=0; refcount=0; }
	};

	// using underscore because some older compilers will complain
	
	Connection *_conn_list; //signals connected are referenced here, so as you see, it's very lightweight
	
	friend class SignalBase;
	bool _register_signal( SignalBase *p_signal ); ///< return true if signal already exists, to warn and avoid connecting twice.
	void _clear_signal( SignalBase *p_signal );
	
protected:
	
	void clear_connections();
	
public:
	
		
	SignalTarget();
	~SignalTarget();
};


/** This amazing hack is based on the FastDelegates theory */

// tale of an amazing hack.. //

// if you declare an unexisting class..
class __UnexistingClass;

// .. a forward-referenced method to that class can be defined ..
typedef void (__UnexistingClass::*__UnexistingMethod)();

// .. since the compiler can't tell its type (virtual, multiple inheritance, etc).. 
// .. its size is the maximum size a method can take ..
#define METHOD_CONTAINER_SIZE (sizeof(void*)+sizeof(__UnexistingMethod))

// .. which allows reliable marshalling of methods ..
struct MethodMarshall {

	void *instance;
	unsigned char method[sizeof(__UnexistingMethod)];  

	bool operator==(const MethodMarshall& p_marshall) const {
		
		if (instance!=p_marshall.instance)
			return false;
		
		for (unsigned int i=0;i<sizeof(__UnexistingMethod);i++)
			if (method[i]!=p_marshall.method[i])
				return false;
		
		return true;
	}	
};

// .. against the legal instance and method pairs ..
template<class T, class M>
struct MethodStruct {

	T* instance;
	M method;


};

// .. by using this union ..
template<class T, class M>
union MethodUnion {

	MethodStruct<T,M> s;
	MethodMarshall m;
	
	MethodUnion() {
	/* Initialize to zero */
		m.instance=0;
		for (unsigned int i=0;i<sizeof(__UnexistingMethod);i++)
		m.method[i]=0;
	}
};

// .. created with this! ..
template<class T, class M>
MethodMarshall _method_marshallize(T* p_instance, M p_method) {

	MethodUnion<T,M> mu;
	mu.s.instance=p_instance;
	mu.s.method=p_method;
	return mu.m;
  
}


template<class P1,class P2,class P3,class P4,class P5,class P6>
class Method6 {
	
	typedef void (*CallFuncPtr)(const MethodMarshall& method,P1 p1,P2 p2,P3 p3,P4 p4,P5 p5, P6 p6);
	
	template<class T,class M>
	static void call_func_template(const MethodMarshall& p_marshall,P1 p1,P2 p2,P3 p3,P4 p4,P5 p5, P6 p6) {
	
		MethodUnion<T,M> u;
		u.m=p_marshall;
		T *instance=u.s.instance; M method=u.s.method;
		(instance->*method)(p1,p2,p3,p4,p5,p6);
	
	}
	
	CallFuncPtr call_func;
	MethodMarshall method;
	SignalTarget *instance;
      
public:

	typedef P1 Type1;
	typedef P2 Type2;
	typedef P3 Type3;
	typedef P4 Type4;
	typedef P5 Type5;
	typedef P6 Type6;

	bool operator==(const Method6& p_method) const { return method==p_method.method; }
	bool operator!=(const Method6& p_method) const { return !( *this==p_method ); }

	SignalTarget *get_instance() const { return instance; }
	
	void call(P1 p1,P2 p2,P3 p3,P4 p4,P5 p5, P6 p6) {
		
		if (call_func) call_func(method,p1,p2,p3,p4,p5,p6);
	}
	
	template<class T>
	void set( T* p_instance,void (T::*p_method)(P1,P2,P3,P4,P5,P6) ) {
		
		method=_method_marshallize(p_instance,p_method);	
		call_func=&call_func_template<T,void (T::*)(P1,P2,P3,P4,P5,P6)>;
		instance=p_instance;
	}
		
	void clear() { call_func=0; }
	
	Method6() {
		
		call_func=0;
		method.instance = 0;
	}
	
	template<class T>
	Method6( T* p_instance,void (T::*p_method)(P1,P2,P3,P4,P5,P6)) {
		set( p_instance,p_method );
	}

};













template<class P1,class P2,class P3,class P4,class P5>
struct Bind5to6_Base {

	virtual SignalTarget *get_instance() const=0;
	//virtual _EmptyFunc get_method()const =0;
	virtual void call(P1,P2,P3,P4,P5)=0;
	virtual Bind5to6_Base * copy() const=0;
	virtual ~Bind5to6_Base() {}
};


template<class P1,class P2,class P3,class P4,class P5,class P6>
struct Bind5to6 : public Bind5to6_Base<P1,P2,P3,P4,P5> {

	P6 data;
	Method6<P1,P2,P3,P4,P5,P6> method;
	
	//_EmptyFunc get_method() const { return method.get_method(); }
	SignalTarget *get_instance() const { return method.get_instance(); }
	
	void call(P1 p_data1,P2 p_data2,P3 p_data3,P4 p_data4,P5 p_data5) { method.call(p_data1,p_data2,p_data3,p_data4,p_data5, data); }
	Bind5to6_Base<P1,P2,P3,P4,P5> * copy() const {
		Bind5to6<P1,P2,P3,P4,P5,P6> *_copy = memnew( (Bind5to6<P1,P2,P3,P4,P5,P6>)(method, data));
		*_copy=*this;
		return _copy;
	}
	
	Bind5to6(const Method6<P1,P2,P3,P4,P5,P6>& p_method, P6 p_data) {
	
		data = p_data;
		method=p_method;
	}
};

template< class P1,class P2,class P3,class P4,class P5 >
class Method5 {
	
	typedef void (*CallFuncPtr)(const MethodMarshall& method, P1 p1,P2 p2,P3 p3,P4 p4,P5 p5);
	
	template<class T,class M>
	static void call_func_template(const MethodMarshall& p_marshall, P1 p1,P2 p2,P3 p3,P4 p4,P5 p5) {
	
		MethodUnion<T,M> u;
		u.m=p_marshall;
		T *instance=u.s.instance; M method=u.s.method;
		(instance->*method)(p1,p2,p3,p4,p5);
	
	}
	
	CallFuncPtr call_func;
	MethodMarshall method;
	SignalTarget *instance;
	Bind5to6_Base< P1,P2,P3,P4,P5 > *bindobj;
      
public:

	typedef P1 Type1;typedef P2 Type2;typedef P3 Type3;typedef P4 Type4;typedef P5 Type5;
	/* placeholders so Signal<> compiles */
	//typedef int Type5;
	typedef int Type6;

	bool operator==(const Method5& p_method) const { return bindobj!=0?false:method==p_method.method; }
	bool operator!=(const Method5& p_method) const { return !( *this==p_method ); }

	SignalTarget *get_instance() const { return bindobj?bindobj->get_instance():instance; }
	
	void call(P1 p1,P2 p2,P3 p3,P4 p4,P5 p5) {
		
		if (bindobj) {
			
			bindobj->call( p1,p2,p3,p4,p5 );
		} else {
		
			if (call_func) call_func(method, p1,p2,p3,p4,p5 );
		};
	}
	
	template<class T>
	void set( T* p_instance,void (T::*p_method)(P1,P2,P3,P4,P5) ) {

		if (bindobj)
			memdelete(bindobj);
		bindobj = 0;
		
		method=_method_marshallize(p_instance,p_method);	
		call_func=&call_func_template<T,void (T::*)(P1,P2,P3,P4,P5)>;
		instance=p_instance;
	}

	template<class TBind>
	void bind(const Method6<P1,P2,P3,P4,P5, TBind>& p_method, TBind p_data) {

		clear();
		bindobj = memnew((Bind5to6<P1,P2,P3,P4,P5, TBind>)(p_method, p_data));
	}

	void clear() { call_func=0; if (bindobj) memdelete(bindobj); bindobj = 0; }

	void operator=(const Method5& p_method) {

		if (bindobj)
				memdelete(bindobj);
		bindobj=0;

		if (p_method.bindobj) {

			bindobj=p_method.bindobj->copy();
			call_func=0;
			instance = p_method.instance;

		} else {

			call_func=p_method.call_func;
			method = p_method.method;
			instance = p_method.instance;
			bindobj=0;
		}
	}

	
	Method5() {
		
		call_func=0;
		bindobj = 0;
		method.instance = 0;
	}
	
	template<class T>
	Method5( T* p_instance,void (T::*p_method)(P1,P2,P3,P4,P5)) {
		bindobj = 0;
		set( p_instance,p_method );
	}

	
	template<class TBind>
	Method5(const Method6<P1,P2,P3,P4,P5,TBind>& p_method, TBind p_data) {
		bindobj=0;
		bind( p_method, p_data );
	}

	Method5(const Method5& p_method) {
		
		bindobj = 0;
		operator=(p_method);
	}
	
	~Method5() {
	
		clear();
	};

};



template<class P1,class P2,class P3,class P4>
struct Bind4to5_Base {

	virtual SignalTarget *get_instance() const=0;
	//virtual _EmptyFunc get_method()const =0;
	virtual void call(P1,P2,P3,P4)=0;
	virtual Bind4to5_Base * copy() const=0;
	virtual ~Bind4to5_Base() {}
};


template<class P1,class P2,class P3,class P4,class P5>
struct Bind4to5 : public Bind4to5_Base<P1,P2,P3,P4> {

	P5 data;
	Method5<P1,P2,P3,P4,P5> method;
	
	//_EmptyFunc get_method() const { return method.get_method(); }
	SignalTarget *get_instance() const { return method.get_instance(); }
	
	void call(P1 p_data1,P2 p_data2,P3 p_data3,P4 p_data4) { method.call(p_data1,p_data2,p_data3,p_data4, data); }
	Bind4to5_Base<P1,P2,P3,P4> * copy() const {
		Bind4to5<P1,P2,P3,P4,P5> *_copy = memnew( (Bind4to5<P1,P2,P3,P4,P5>)(method, data));
		*_copy=*this;
		return _copy;
	}
	
	Bind4to5(const Method5<P1,P2,P3,P4,P5>& p_method, P5 p_data) {
	
		data = p_data;
		method=p_method;
	}
};

template< class P1,class P2,class P3,class P4 >
class Method4 {
	
	typedef void (*CallFuncPtr)(const MethodMarshall& method, P1 p1,P2 p2,P3 p3,P4 p4);
	
	template<class T,class M>
	static void call_func_template(const MethodMarshall& p_marshall, P1 p1,P2 p2,P3 p3,P4 p4) {
	
		MethodUnion<T,M> u;
		u.m=p_marshall;
		T *instance=u.s.instance; M method=u.s.method;
		(instance->*method)(p1,p2,p3,p4);
	
	}
	
	CallFuncPtr call_func;
	MethodMarshall method;
	SignalTarget *instance;
	Bind4to5_Base< P1,P2,P3,P4 > *bindobj;
      
public:

	typedef P1 Type1;typedef P2 Type2;typedef P3 Type3;typedef P4 Type4;
	/* placeholders so Signal<> compiles */
	//typedef int Type4;
	typedef int Type5;
	typedef int Type6;

	bool operator==(const Method4& p_method) const { return bindobj!=0?false:method==p_method.method; }
	bool operator!=(const Method4& p_method) const { return !( *this==p_method ); }

	SignalTarget *get_instance() const { return bindobj?bindobj->get_instance():instance; }
	
	void call(P1 p1,P2 p2,P3 p3,P4 p4) {
		
		if (bindobj) {
			
			bindobj->call( p1,p2,p3,p4 );
		} else {
		
			if (call_func) call_func(method, p1,p2,p3,p4 );
		};
	}
	
	template<class T>
	void set( T* p_instance,void (T::*p_method)(P1,P2,P3,P4) ) {

		if (bindobj)
			memdelete(bindobj);
		bindobj = 0;
		
		method=_method_marshallize(p_instance,p_method);	
		call_func=&call_func_template<T,void (T::*)(P1,P2,P3,P4)>;
		instance=p_instance;
	}

	template<class TBind>
	void bind(const Method5<P1,P2,P3,P4, TBind>& p_method, TBind p_data) {

		clear();
		bindobj = memnew((Bind4to5<P1,P2,P3,P4, TBind>)(p_method, p_data));
	}

	void clear() { call_func=0; if (bindobj) memdelete(bindobj); bindobj = 0; }

	void operator=(const Method4& p_method) {

		if (bindobj)
				memdelete(bindobj);
		bindobj=0;

		if (p_method.bindobj) {

			bindobj=p_method.bindobj->copy();
			call_func=0;
			instance = p_method.instance;

		} else {

			call_func=p_method.call_func;
			method = p_method.method;
			instance = p_method.instance;
			bindobj=0;
		}
	}

	
	Method4() {
		
		call_func=0;
		bindobj = 0;
		method.instance = 0;
	}
	
	template<class T>
	Method4( T* p_instance,void (T::*p_method)(P1,P2,P3,P4)) {
		bindobj = 0;
		set( p_instance,p_method );
	}

	
	template<class TBind>
	Method4(const Method5<P1,P2,P3,P4,TBind>& p_method, TBind p_data) {
		bindobj=0;
		bind( p_method, p_data );
	}

	Method4(const Method4& p_method) {
		
		bindobj = 0;
		operator=(p_method);
	}
	
	~Method4() {
	
		clear();
	};

};



template<class P1,class P2,class P3>
struct Bind3to4_Base {

	virtual SignalTarget *get_instance() const=0;
	//virtual _EmptyFunc get_method()const =0;
	virtual void call(P1,P2,P3)=0;
	virtual Bind3to4_Base * copy() const=0;
	virtual ~Bind3to4_Base() {}
};


template<class P1,class P2,class P3,class P4>
struct Bind3to4 : public Bind3to4_Base<P1,P2,P3> {

	P4 data;
	Method4<P1,P2,P3,P4> method;
	
	//_EmptyFunc get_method() const { return method.get_method(); }
	SignalTarget *get_instance() const { return method.get_instance(); }
	
	void call(P1 p_data1,P2 p_data2,P3 p_data3) { method.call(p_data1,p_data2,p_data3, data); }
	Bind3to4_Base<P1,P2,P3> * copy() const {
		Bind3to4<P1,P2,P3,P4> *_copy = memnew( (Bind3to4<P1,P2,P3,P4>)(method, data));
		*_copy=*this;
		return _copy;
	}
	
	Bind3to4(const Method4<P1,P2,P3,P4>& p_method, P4 p_data) {
	
		data = p_data;
		method=p_method;
	}
};

template< class P1,class P2,class P3 >
class Method3 {
	
	typedef void (*CallFuncPtr)(const MethodMarshall& method, P1 p1,P2 p2,P3 p3);
	
	template<class T,class M>
	static void call_func_template(const MethodMarshall& p_marshall, P1 p1,P2 p2,P3 p3) {
	
		MethodUnion<T,M> u;
		u.m=p_marshall;
		T *instance=u.s.instance; M method=u.s.method;
		(instance->*method)(p1,p2,p3);
	
	}
	
	CallFuncPtr call_func;
	MethodMarshall method;
	SignalTarget *instance;
	Bind3to4_Base< P1,P2,P3 > *bindobj;
      
public:

	typedef P1 Type1;typedef P2 Type2;typedef P3 Type3;
	/* placeholders so Signal<> compiles */
	//typedef int Type3;
	typedef int Type4;
	typedef int Type5;
	typedef int Type6;

	bool operator==(const Method3& p_method) const { return bindobj!=0?false:method==p_method.method; }
	bool operator!=(const Method3& p_method) const { return !( *this==p_method ); }

	SignalTarget *get_instance() const { return bindobj?bindobj->get_instance():instance; }
	
	void call(P1 p1,P2 p2,P3 p3) {
		
		if (bindobj) {
			
			bindobj->call( p1,p2,p3 );
		} else {
		
			if (call_func) call_func(method, p1,p2,p3 );
		};
	}
	
	template<class T>
	void set( T* p_instance,void (T::*p_method)(P1,P2,P3) ) {

		if (bindobj)
			memdelete(bindobj);
		bindobj = 0;
		
		method=_method_marshallize(p_instance,p_method);	
		call_func=&call_func_template<T,void (T::*)(P1,P2,P3)>;
		instance=p_instance;
	}

	template<class TBind>
	void bind(const Method4<P1,P2,P3, TBind>& p_method, TBind p_data) {

		clear();
		bindobj = memnew((Bind3to4<P1,P2,P3, TBind>)(p_method, p_data));
	}

	void clear() { call_func=0; if (bindobj) memdelete(bindobj); bindobj = 0; }

	void operator=(const Method3& p_method) {

		if (bindobj)
				memdelete(bindobj);
		bindobj=0;

		if (p_method.bindobj) {

			bindobj=p_method.bindobj->copy();
			call_func=0;
			instance = p_method.instance;

		} else {

			call_func=p_method.call_func;
			method = p_method.method;
			instance = p_method.instance;
			bindobj=0;
		}
	}

	
	Method3() {
		
		call_func=0;
		bindobj = 0;
		method.instance = 0;
	}
	
	template<class T>
	Method3( T* p_instance,void (T::*p_method)(P1,P2,P3)) {
		bindobj = 0;
		set( p_instance,p_method );
	}

	
	template<class TBind>
	Method3(const Method4<P1,P2,P3,TBind>& p_method, TBind p_data) {
		bindobj=0;
		bind( p_method, p_data );
	}

	Method3(const Method3& p_method) {
		
		bindobj = 0;
		operator=(p_method);
	}
	
	~Method3() {
	
		clear();
	};

};



template<class P1,class P2>
struct Bind2to3_Base {

	virtual SignalTarget *get_instance() const=0;
	//virtual _EmptyFunc get_method()const =0;
	virtual void call(P1,P2)=0;
	virtual Bind2to3_Base * copy() const=0;
	virtual ~Bind2to3_Base() {}
};


template<class P1,class P2,class P3>
struct Bind2to3 : public Bind2to3_Base<P1,P2> {

	P3 data;
	Method3<P1,P2,P3> method;
	
	//_EmptyFunc get_method() const { return method.get_method(); }
	SignalTarget *get_instance() const { return method.get_instance(); }
	
	void call(P1 p_data1,P2 p_data2) { method.call(p_data1,p_data2, data); }
	Bind2to3_Base<P1,P2> * copy() const {
		Bind2to3<P1,P2,P3> *_copy = memnew( (Bind2to3<P1,P2,P3>)(method, data));
		*_copy=*this;
		return _copy;
	}
	
	Bind2to3(const Method3<P1,P2,P3>& p_method, P3 p_data) {
	
		data = p_data;
		method=p_method;
	}
};

template< class P1,class P2 >
class Method2 {
	
	typedef void (*CallFuncPtr)(const MethodMarshall& method, P1 p1,P2 p2);
	
	template<class T,class M>
	static void call_func_template(const MethodMarshall& p_marshall, P1 p1,P2 p2) {
	
		MethodUnion<T,M> u;
		u.m=p_marshall;
		T *instance=u.s.instance; M method=u.s.method;
		(instance->*method)(p1,p2);
	
	}
	
	CallFuncPtr call_func;
	MethodMarshall method;
	SignalTarget *instance;
	Bind2to3_Base< P1,P2 > *bindobj;
      
public:

	typedef P1 Type1;typedef P2 Type2;
	/* placeholders so Signal<> compiles */
	//typedef int Type2;
	typedef int Type3;
	typedef int Type4;
	typedef int Type5;
	typedef int Type6;

	bool operator==(const Method2& p_method) const { return bindobj!=0?false:method==p_method.method; }
	bool operator!=(const Method2& p_method) const { return !( *this==p_method ); }

	SignalTarget *get_instance() const { return bindobj?bindobj->get_instance():instance; }
	
	void call(P1 p1,P2 p2) {
		
		if (bindobj) {
			
			bindobj->call( p1,p2 );
		} else {
		
			if (call_func) call_func(method, p1,p2 );
		};
	}
	
	template<class T>
	void set( T* p_instance,void (T::*p_method)(P1,P2) ) {

		if (bindobj)
			memdelete(bindobj);
		bindobj = 0;
		
		method=_method_marshallize(p_instance,p_method);	
		call_func=&call_func_template<T,void (T::*)(P1,P2)>;
		instance=p_instance;
	}

	template<class TBind>
	void bind(const Method3<P1,P2, TBind>& p_method, TBind p_data) {

		clear();
		bindobj = memnew((Bind2to3<P1,P2, TBind>)(p_method, p_data));
	}

	void clear() { call_func=0; if (bindobj) memdelete(bindobj); bindobj = 0; }

	void operator=(const Method2& p_method) {

		if (bindobj)
				memdelete(bindobj);
		bindobj=0;

		if (p_method.bindobj) {

			bindobj=p_method.bindobj->copy();
			call_func=0;
			instance = p_method.instance;

		} else {

			call_func=p_method.call_func;
			method = p_method.method;
			instance = p_method.instance;
			bindobj=0;
		}
	}

	
	Method2() {
		
		call_func=0;
		bindobj = 0;
		method.instance = 0;
	}
	
	template<class T>
	Method2( T* p_instance,void (T::*p_method)(P1,P2)) {
		bindobj = 0;
		set( p_instance,p_method );
	}

	
	template<class TBind>
	Method2(const Method3<P1,P2,TBind>& p_method, TBind p_data) {
		bindobj=0;
		bind( p_method, p_data );
	}

	Method2(const Method2& p_method) {
		
		bindobj = 0;
		operator=(p_method);
	}
	
	~Method2() {
	
		clear();
	};

};



template<class P1>
struct Bind1to2_Base {

	virtual SignalTarget *get_instance() const=0;
	//virtual _EmptyFunc get_method()const =0;
	virtual void call(P1)=0;
	virtual Bind1to2_Base * copy() const=0;
	virtual ~Bind1to2_Base() {}
};


template<class P1,class P2>
struct Bind1to2 : public Bind1to2_Base<P1> {

	P2 data;
	Method2<P1,P2> method;
	
	//_EmptyFunc get_method() const { return method.get_method(); }
	SignalTarget *get_instance() const { return method.get_instance(); }
	
	void call(P1 p_data1) { method.call(p_data1, data); }
	Bind1to2_Base<P1> * copy() const {
		Bind1to2<P1,P2> *_copy = memnew( (Bind1to2<P1,P2>)(method, data));
		*_copy=*this;
		return _copy;
	}
	
	Bind1to2(const Method2<P1,P2>& p_method, P2 p_data) {
	
		data = p_data;
		method=p_method;
	}
};

template< class P1 >
class Method1 {
	
	typedef void (*CallFuncPtr)(const MethodMarshall& method, P1 p1);
	
	template<class T,class M>
	static void call_func_template(const MethodMarshall& p_marshall, P1 p1) {
	
		MethodUnion<T,M> u;
		u.m=p_marshall;
		T *instance=u.s.instance; M method=u.s.method;
		(instance->*method)(p1);
	
	}
	
	CallFuncPtr call_func;
	MethodMarshall method;
	SignalTarget *instance;
	Bind1to2_Base< P1 > *bindobj;
      
public:

	typedef P1 Type1;
	/* placeholders so Signal<> compiles */
	//typedef int Type1;
	typedef int Type2;
	typedef int Type3;
	typedef int Type4;
	typedef int Type5;
	typedef int Type6;

	bool operator==(const Method1& p_method) const { return bindobj!=0?false:method==p_method.method; }
	bool operator!=(const Method1& p_method) const { return !( *this==p_method ); }

	SignalTarget *get_instance() const { return bindobj?bindobj->get_instance():instance; }
	
	void call(P1 p1) {
		
		if (bindobj) {
			
			bindobj->call( p1 );
		} else {
		
			if (call_func) call_func(method, p1 );
		};
	}
	
	template<class T>
	void set( T* p_instance,void (T::*p_method)(P1) ) {

		if (bindobj)
			memdelete(bindobj);
		bindobj = 0;
		
		method=_method_marshallize(p_instance,p_method);	
		call_func=&call_func_template<T,void (T::*)(P1)>;
		instance=p_instance;
	}

	template<class TBind>
	void bind(const Method2<P1, TBind>& p_method, TBind p_data) {

		clear();
		bindobj = memnew((Bind1to2<P1, TBind>)(p_method, p_data));
	}

	void clear() { call_func=0; if (bindobj) memdelete(bindobj); bindobj = 0; }

	void operator=(const Method1& p_method) {

		if (bindobj)
				memdelete(bindobj);
		bindobj=0;

		if (p_method.bindobj) {

			bindobj=p_method.bindobj->copy();
			call_func=0;
			instance = p_method.instance;

		} else {

			call_func=p_method.call_func;
			method = p_method.method;
			instance = p_method.instance;
			bindobj=0;
		}
	}

	
	Method1() {
		
		call_func=0;
		bindobj = 0;
		method.instance = 0;
	}
	
	template<class T>
	Method1( T* p_instance,void (T::*p_method)(P1)) {
		bindobj = 0;
		set( p_instance,p_method );
	}

	
	template<class TBind>
	Method1(const Method2<P1,TBind>& p_method, TBind p_data) {
		bindobj=0;
		bind( p_method, p_data );
	}

	Method1(const Method1& p_method) {
		
		bindobj = 0;
		operator=(p_method);
	}
	
	~Method1() {
	
		clear();
	};

};


/** Bind Method1 to Method0 **/

struct Bind0to1_Base {

        virtual SignalTarget *get_instance() const=0;
        //virtual _EmptyFunc get_method()const =0;
        virtual void call()=0;
        virtual Bind0to1_Base * copy() const=0;
        virtual ~Bind0to1_Base() {}
};

template<class P1>
struct Bind0to1 : public Bind0to1_Base {

	P1 data1;
	Method1<P1> method;

	//_EmptyFunc get_method() const { return method.get_method(); }
	SignalTarget *get_instance() const { return method.get_instance(); }

	void call() { method.call(data1); }
	Bind0to1_Base * copy() const {
		Bind0to1<P1> *_copy = memnew(Bind0to1<P1>(method,data1));
		*_copy=*this;
		return _copy;
	}

	Bind0to1(const Method1<P1>& p_method1, P1 p_data1) {data1=p_data1; method=p_method1; }
};


class Method {
	
	typedef void (*CallFuncPtr)(const MethodMarshall& method);
	
	template<class T,class M>
	static void call_func_template(const MethodMarshall& p_marshall) {
	
		MethodUnion<T,M> u;
		u.m=p_marshall;
		T *instance=u.s.instance; M method=u.s.method;
		(instance->*method)();
	
	}
	
	CallFuncPtr call_func;
	MethodMarshall method;
	SignalTarget *instance;
	Bind0to1_Base* bindobj;
      
public:

	/* placeholders so Signal<> compiles */
	typedef int Type1;
	typedef int Type2;
	typedef int Type3;
	typedef int Type4;
	typedef int Type5;
	typedef int Type6;

	bool operator==(const Method& p_method) const { return bindobj!=0?false:method==p_method.method; }
	bool operator!=(const Method& p_method) const { return !( *this==p_method ); }

	bool is_empty() const  { return (call_func==0 && bindobj==0); }
	
	SignalTarget *get_instance() const {
	
		return bindobj?bindobj->get_instance():instance;
	};
	
	void call() {

		if (bindobj) {
		
			bindobj->call();
			
		} else {
	
			if (call_func) call_func(method);
		};
	}
	
	template<class T>
	void set( T* p_instance,void (T::*p_method)() ) {

		clear();
		method=_method_marshallize(p_instance,p_method);	
		call_func=&call_func_template<T,void (T::*)()>;
		instance = p_instance;
	}

	template<class P1>
	void bind(const Method1<P1>& p_method1, P1 p_data1) {

		clear();
		bindobj = memnew(Bind0to1<P1>(p_method1,p_data1));
	}

	
	void clear() { call_func=0; if (bindobj) memdelete(bindobj); bindobj = 0; instance = 0; }
	
	void operator=(const Method& p_method) {

		if (bindobj)
				memdelete(bindobj);
		bindobj=0;

		if (p_method.bindobj) {

			bindobj=p_method.bindobj->copy();
			call_func=0;
			instance = p_method.instance;

		} else {

			call_func=p_method.call_func;
			method = p_method.method;
			instance = p_method.instance;
			bindobj=0;
		}
	}
	
	
	Method() {

		bindobj = 0;
		clear();
	}
	
	template<class T>
	Method( T* p_instance,void (T::*p_method)()) {
		bindobj = 0;
		set( p_instance,p_method );
	}

	template<class P1>
	Method(const Method1<P1>& p_method1, P1 p_data1) {
		bindobj=0; clear();
		bind( p_method1,p_data1 );
	}

	Method(const Method& p_method) {

		bindobj = 0;
		operator=(p_method);
	}
	
	~Method() {
	
		clear();
	};

};



/**
 * SignalTarget:
 *
 * Signals can only be connected to objects inheriting from this class.
 * (normal or multiple inheritance is fine)
 * This is so, when the object dies, signals pointing to it are cleaned up,
 * so it becomes impossible (or at least very hard) to call a no longer existing instance.
 */


class SignalBase {

public:
	virtual void disconnect(int p_count)=0;

protected:
friend class SignalTarget;

	static int connection_count;

	virtual void remove_target( SignalTarget *p_target )=0;

	bool register_at_target( SignalTarget *p_target ); //return true if failed! (signal already connected to
	void remove_from_target( SignalTarget *p_target );
	
	virtual ~SignalBase() {};

};


/*
	signal_call
	N: 5
*/




template<class M=Method>
class Signal : public SignalBase {

	struct Connection {

		SignalTarget *instance;
		Connection *next;
		M method;
		
		int count;
		
		bool removed; //workaround to avoid a complex situation (target is deleted while in call)
		bool added;

		Connection() { next=0; removed=false; added=true; instance=0; count = -1; }
	};


	Connection *conn_list;
	
	int call_ref; // each time we call, increments by one, then decrements by one
	bool remotion_on_call; //workaround to avoid a complex situation (target is deleted while in call)
	bool addition_on_call;

	virtual void remove_target( SignalTarget *p_target ) {

		Connection *c = conn_list;
				
		while (c) {

			
			if (c->instance==p_target) {
				
				c->removed=true;
			}
			
			c=c->next;
			
		}
		
		//make sure we are not inside a call. if we are, the call will erase this later
		if (call_ref==0) 
			remove_pending();
		else
			remotion_on_call=true;
		
	}
	
	void remove_pending() {

		Connection *c = conn_list;
		Connection **cp = &conn_list;
				
		while (c) {

			
			if (c->removed) {

				Connection *aux=c;
				*cp=c->next;
				c=c->next;

				remove_from_target( aux->instance );
				memdelete(aux); //good bye connection

				continue;
				
			}

			cp=&c->next;
			c=c->next;
			
		}
		
		remotion_on_call=false;
	}
	
	void add_pending() {

		Connection *c = conn_list;
				
		while (c) {

			
			if (c->added) {
				c->added=false;
			}
			
			c=c->next;
			
		}
		
		addition_on_call=false;
	}
	
	/// for now, you can't really copy a signal to another 
	Signal(const Signal& p_cc);
	void operator=(const Signal& p_cc);
public:
	
	
	template<class T, class U>
	int connect( T* p_instance, U p_method ) {
		
		return connect( M(p_instance,p_method) );
	}
	
	int connect( const M& p_method ) {
	
		M m=p_method;

		//if (call_ref>0) //cant connect while call-ref-ing
		//	return;
		
		Connection *c = conn_list;
		Connection *last=0;

		/* Must check beforehand if we have this method */
		
		while (c) {
			
			if (!c->removed && c->method==m) {

				return -1; // already connected there! dont do it again
			};

			if (!c->next)
				last=c;
			c=c->next;
		}

		register_at_target( m.get_instance() );
			//instance already has us? do we have that method?


		c = memnew(Connection);
		c->method=m;
		c->instance=m.get_instance();
		c->count = SignalBase::connection_count++;
		if (call_ref>0) {
			c->added=true;
			addition_on_call=true;
		} else {
		
			c->added=false;
		}
		if (conn_list && last) {
			
			last->next=c;
		} else {
			c->next=conn_list;
			conn_list=c;
		}

		return c->count;
	}

	int get_target_count() const {
		
		int count = 0;
		Connection* c = conn_list;
		while (c) {
			++count;
			c = c->next;
		};
		
		return count;
	};
	
	template<class T,class U>
	void disconnect(T p_instance, U p_method_ptr ) {

		disconnect( M( p_instance, p_method_ptr ) );
	}
	
	void disconnect(int p_count) {
	
		Connection *c = conn_list;

		/* Must check beforehand if we have this method */
		
		while (c) {
			
			if (c->count == p_count) {
				c->removed = true;
				break;
			};

			c=c->next;
		}
		if (!c) {
			ERR_PRINT("Invalid connection count.");
			return;
		};
		// unless inside a call, cleanup! otherwise autocleanup will happen later
		
		if (call_ref==0) 
			remove_pending();
		else
			remotion_on_call=true;
	};
	
	template<class T>
	void disconnect(T p_instance ) {

		remove_target(p_instance);
	}

	void disconnect( const M& p_method ) {

		Connection *c=conn_list;
				
		while (c) {

			
			if (c->method==p_method) {

				
				c->removed=true;
				break;
			}
			
			c=c->next;
		}
						
		// unless inside a call, cleanup! otherwise autocleanup will happen later
		
		if (call_ref==0) 
			remove_pending();
		else
			remotion_on_call=true;
		
	}
	
	/* You can only call the right method! using the wrong one will cause a compiler error! */
	
	void call() {
		
		call_ref++;
		
		Connection *c = conn_list;
				
		while (c) {
			
			if (!c->removed && !c->added) //removed in a previous call
				c->method.call();
			c=c->next;
		}			
		call_ref--;
		
		if (call_ref==0 && remotion_on_call)
			remove_pending();
			
		if (call_ref==0 && addition_on_call)
			add_pending();
			
	}

//	template<class P1>
	void call(typename M::Type1 p1) {

		call_ref++;

		Connection *c = conn_list;

		while (c) {

			if (!c->removed && !c->added) //removed in a previous call
				c->method.call(p1);
			c=c->next;
		}		

		call_ref--;

		if (call_ref==0 && remotion_on_call)
			remove_pending();
			
		if (call_ref==0 && addition_on_call)
			add_pending();

	}

//	template<class P1,class P2>
	void call(	typename M::Type1 p1,
			typename M::Type2 p2) {

		call_ref++;

		Connection *c = conn_list;

		while (c) {

			if (!c->removed && !c->added) //removed in a previous call
				c->method.call(p1,p2);
			c=c->next;
		}		

		call_ref--;

		if (call_ref==0 && remotion_on_call)
			remove_pending();
		if (call_ref==0 && addition_on_call)
			add_pending();

	}

//		template<class P1,class P2,class P3>
	void call(	typename M::Type1 p1,
			typename M::Type2 p2,
			typename M::Type3 p3) {

		call_ref++;

		Connection *c = conn_list;

		while (c) {

			if (!c->removed && !c->added) //removed in a previous call
				c->method.call(p1,p2,p3);
			c=c->next;
		}		

		call_ref--;

		if (call_ref==0 && remotion_on_call)
			remove_pending();
		if (call_ref==0 && addition_on_call)
			add_pending();

	}

//		template<class P1,class P2,class P3,class P4>
	void call(	typename M::Type1 p1,
			typename M::Type2 p2,
			typename M::Type3 p3,
			typename M::Type4 p4) {

		call_ref++;

		Connection *c = conn_list;

		while (c) {

			if (!c->removed && !c->added) //removed in a previous call
				c->method.call(p1,p2,p3,p4);
			c=c->next;
		}		

		call_ref--;

		if (call_ref==0 && remotion_on_call)
			remove_pending();
		if (call_ref==0 && addition_on_call)
			add_pending();

	}

	void call(	typename M::Type1 p1,
			typename M::Type2 p2,
			typename M::Type3 p3,
			typename M::Type4 p4,
			typename M::Type5 p5) {

		call_ref++;

		Connection *c = conn_list;

		while (c) {

			if (!c->removed && !c->added) //removed in a previous call
				c->method.call(p1,p2,p3,p4,p5);
			c=c->next;
		}		

		call_ref--;

		if (call_ref==0 && remotion_on_call)
			remove_pending();
		if (call_ref==0 && addition_on_call)
			add_pending();

	}

	void call(	typename M::Type1 p1,
			typename M::Type2 p2,
			typename M::Type3 p3,
			typename M::Type4 p4,
			typename M::Type5 p5,
			typename M::Type6 p6) {

		call_ref++;

		Connection *c = conn_list;

		while (c) {

			if (!c->removed && !c->added) //removed in a previous call
				c->method.call(p1,p2,p3,p4,p5,p6);
			c=c->next;
		}		

		call_ref--;

		if (call_ref==0 && remotion_on_call)
			remove_pending();
		if (call_ref==0 && addition_on_call)
			add_pending();

	}

	void clear() {

		if (call_ref>0) {
			
			ERR_PRINT(" DO NOT CLEAR A SIGNAL IN THE MIDDLE OF A CALL() ");
			
		}
		
		Connection *c = conn_list;
				
		while (c) {
			
			c->removed=true;
			c=c->next;
		}
		remove_pending();
	};
	
	Signal() {

		conn_list=0;
		call_ref=0; 
		remotion_on_call=false;		
		addition_on_call=false;		
	}
	
	
	~Signal() {

		if (call_ref>0) {
			
			ERR_PRINT(" DO NOT DELETE A SIGNAL IN THE MIDDLE OF A CALL() ");
			
		}
		
		clear();
	};

};
	
#endif



