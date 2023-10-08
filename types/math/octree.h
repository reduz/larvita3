
//
// C++ Interface: octree
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef OCTREE_H
#define OCTREE_H

#include "vector3.h"
#include "aabb.h"
#include "convex_group.h"
#include "list.h"
#include "variant.h"
#include "performance_metrics.h"
/**
	@author Juan Linietsky <reduzio@gmail.com>
*/

template<class T>
class Octree {
public:
	
	enum {
		NEG_NEG_NEG,
  		NEG_NEG_POS,
		NEG_POS_NEG,
		NEG_POS_POS,
		POS_NEG_NEG,
		POS_NEG_POS,
		POS_POS_NEG,
		POS_POS_POS,
	};
	
	enum {

		NO_CHILD=-1,
		CELL_MAX=8,
	};

	struct Octant;
	
	struct Element {
		
		T t;
		Uint32 pass;
		AABB aabb;
		
		// elements in the octant 
		
		struct OctantInfo {
			
			typename List<Element*>::Iterator* I;
			Octant *octant;
			OctantInfo() { I=NULL; octant=NULL; }
		};
		
		List< OctantInfo > octant_iterator_list;		
		
		Element() {
			
			pass=0;
		}
				
	};
	
	struct Octant {

		Octant *parent;
		Octant *childs[8];
		
		List<Element*> element_list;
		
		AABB aabb;

		Octant() { 
			
			parent=NULL;
			for (int i=0;i<8;i++)
				childs[i]=NULL;
		}
		~Octant() { 
						
			for (int i=0;i<8;i++) {
				if (childs[i])
					memdelete(childs[i]);
			}
		}
	};

	Octant *root;

	real_t subdivide_threshold;
	real_t smallest_size;

	Uint32 pass;

	void add_element(Element *e);
	void add_element_to_octant(Octant *p_octant,Element *e);
	void attempt_remove_octant(Octant *o);
	void remove_element(Element *e);
	
	void cull_convex(Octant *p_octant,const Plane* p_planes,int p_plane_count, T** p_result_array,int *p_result_idx,int p_result_max);
	void cull_AABB(Octant *p_octant,const AABB& p_AABB, T** p_result_array,int *p_result_idx,int p_result_max);
	void cull_segment(Octant *p_octant,const Vector3& p_from, const Vector3& p_to,T** p_result_array,int *p_result_idx,int p_result_max);
	
	
public:

	T *create(const AABB& p_AABB=AABB());
	void move(T* p_object, const AABB& p_AABB);
	void erase(T* p_object);
	
	int cull_convex(const ConvexGroup& p_convex,T** p_result_array,int p_result_max);
	int cull_AABB(const AABB& p_aabb,T** p_result_array,int p_result_max);
	int cull_segment(const Vector3& p_from, const Vector3& p_to,T** p_result_array,int p_result_max);
	
	Octree(real_t p_smallest=1.0,real_t p_subdivide_thresh=0.5);
	~Octree() { if (root) memdelete(root); }
};

template<class T>
void Octree<T>::add_element_to_octant(Octant *p_octant,Element *e) {
	
	float longest_axis_size = p_octant->aabb.get_longest_axis_size();
	if ( longest_axis_size < 0.01 )
		longest_axis_size = 0.01;
	
	if (p_octant->aabb.get_longest_axis_size()*subdivide_threshold < longest_axis_size ) {
		
		/* at biggest possible size for the element, add it here */
	
		p_octant->element_list.push_back(e);
		typename Element::OctantInfo oinfo;
		oinfo.octant=p_octant;
		oinfo.I=p_octant->element_list.back();
		
		e->octant_iterator_list.push_back( oinfo );
	} else {
		/* not big enough, send it to subitems */
		for (int i=0;i<8;i++) {
					
			if (p_octant->childs[i]) {
				/* element exists, go straight to it */
				if (p_octant->childs[i]->aabb.intersects( e->aabb ) )
					add_element_to_octant( p_octant->childs[i],e );
			} else {
				/* check againt AABB where child should be */
				
				AABB aabb=p_octant->aabb;
				aabb.size/=2.0;
				
				if (i&1)
					aabb.pos.x+=aabb.size.x;
				if (i&2)
					aabb.pos.y+=aabb.size.y;
				if (i&4)
					aabb.pos.z+=aabb.size.z;
				
				if (aabb.intersects( e->aabb) ) {
					/* if actually intersects, create the child */
					
					Octant *child = memnew( Octant );
					p_octant->childs[i]=child;
					child->parent=p_octant;
					child->aabb=aabb;
					add_element_to_octant( child,e );
				}
			}
		}
	}
}

template<class T>
void Octree<T>::add_element(Element *e) {
	
	if (e->aabb.size.x < CMP_EPSILON && e->aabb.size.y < CMP_EPSILON && e->aabb.size.z < CMP_EPSILON)
		return;
	
	/* growing to fit an AABB should always be done the same way */
	if (!root) {
		
		AABB base( Vector3(), Vector3( smallest_size, smallest_size, smallest_size ) );
		
		while ( !base.contains(e->aabb) ) {
			
			if ( ABS(base.pos.x+base.size.x) <= ABS(base.pos.x) ) {
				/* grow towards positive */
				base.size*=2.0;
			} else {
				base.pos-=base.size;				
				base.size*=2.0;
			}
		}
		
		root = memnew( Octant );
		root->parent=NULL;
		root->aabb=base;
	} else {
		
		AABB base=root->aabb;
		
		while( !base.contains( e->aabb ) ) {
			
			Octant * gp = memnew( Octant );
			root->parent=gp;
			
			if ( ABS(base.pos.x+base.size.x) <= ABS(base.pos.x) ) {
				/* grow towards positive */
				base.size*=2.0;
				gp->aabb=base;
				gp->childs[NEG_NEG_NEG]=root;
			} else {
				base.pos-=base.size;				
				base.size*=2.0;
				gp->aabb=base;
				gp->childs[POS_POS_POS]=root;
			}			
			root=gp;			
		}
	}
	
	
	ERR_FAIL_COND( !root );
	if (!root->aabb.contains( e->aabb))
		printf("root %s can't contain e->aabb %s? \n",Variant( root->aabb ).get_string().ascii().get_data(),Variant( e->aabb ).get_string().ascii().get_data() );
	ERR_FAIL_COND( !root->aabb.contains( e->aabb) );
	
	add_element_to_octant(root,e);	
}

template<class T>
void Octree<T>::attempt_remove_octant(Octant *o) {

	if (!o->element_list.empty())
		return;
	for(int i=0;i<8;i++) {
		
		if (o->childs[i])
			return;
	}
	
	if (o->parent) {
		
		for(int i=0;i<8;i++) {
			if (o->parent->childs[i]==o)
				o->parent->childs[i]=NULL;
		}
				  
		attempt_remove_octant( o->parent );
		
	} else if (root==o) {
		
		root=NULL;
	}
	
	memdelete( o );
}

template<class T>
void Octree<T>::remove_element(Element *e) {
	
	if (e->aabb.size.x < CMP_EPSILON && e->aabb.size.y < CMP_EPSILON && e->aabb.size.z < CMP_EPSILON)
		return;
	//OMG!!!
	typename List< typename Element::OctantInfo >::Iterator *I=e->octant_iterator_list.begin();
	
	for(;I;I=I->next()) {
		
		Octant *o=I->get().octant;
		o->element_list.erase( I->get().I );
		
		attempt_remove_octant( o );
	}
	
	e->octant_iterator_list.clear();
}


template<class T>
T *Octree<T>::create(const AABB& p_AABB) {
	
	Element * e = memnew( Element );
	e->aabb=p_AABB;
	
	add_element(e);
	
	T*t=&e->t;
	ERR_FAIL_COND_V((void*)t!=(void*)e,NULL);
	return t;
}

template<class T>
void Octree<T>::move(T* p_object, const AABB& p_AABB) {
	
	MEASURE_BEGIN_P("octree_move"); 	
	Element *e=(Element*)p_object;	
	
	//OMG!!!
	typename List< typename Element::OctantInfo >::Iterator *I=e->octant_iterator_list.begin();
	
	Octant *remove_attempt[16]; // see 
	int remove_attempt_count=0;
	typedef List<Octant*> OctantList;
	OctantList remove_attempt_extra;
	
	for(;I;I=I->next()) {
		
		Octant *o=I->get().octant;
		o->element_list.erase( I->get().I );
		
		if (remove_attempt_count<16)
			remove_attempt[remove_attempt_count++]=o;
		else
			remove_attempt_extra.push_back(o);
	}
	
	e->octant_iterator_list.clear();
	
//	remove_element(e);
	e->aabb=p_AABB;	
	
	add_element(e);
	
	for (int i=0;i<remove_attempt_count;i++) {
		
		attempt_remove_octant( remove_attempt[i] );
	}
	
	typename OctantList::Iterator *I2=remove_attempt_extra.begin();
	
	while(I2) {
		
		attempt_remove_octant( I2->get() );
		I2=I2->next();
	}
	
	MEASURE_END_P("octree_move"); 	
}


template<class T>
void Octree<T>::erase(T* p_object) {

	Element *e=(Element*)p_object;
	
	remove_element(e);
	
	memdelete(e);
}

template<class T>
void Octree<T>::cull_convex(Octant *p_octant,const Plane* p_planes,int p_plane_count, T** p_result_array,int *p_result_idx,int p_result_max) {
	
	if (*p_result_idx==p_result_max)
		return; //pointless
	
	if (!p_octant->element_list.empty()) {
		
		typename List< Element* >::Iterator *I;
		I=p_octant->element_list.begin();
		for(;I;I=I->next()) {
			
			Element *e=I->get();
			
			if (e->pass==pass)
				continue;
			e->pass=pass;
			
			if (e->aabb.intersects_convex_poly(p_planes,p_plane_count)) {
				
				if (*p_result_idx<p_result_max) {
					
					p_result_array[*p_result_idx] = &e->t;
					(*p_result_idx)++;
				} else {
					
					return; // pointless to continue
				}
			}
		}
	}
	
	
	for (int i=0;i<8;i++) {
		
		if (p_octant->childs[i] && p_octant->childs[i]->aabb.intersects_convex_poly(p_planes,p_plane_count)) {
			cull_convex(p_octant->childs[i],p_planes,p_plane_count, p_result_array,p_result_idx,p_result_max);
		}
	}	
}


template<class T>
void Octree<T>::cull_AABB(Octant *p_octant,const AABB& p_AABB, T** p_result_array,int *p_result_idx,int p_result_max) {
	
	if (*p_result_idx==p_result_max)
		return; //pointless
	
	if (!p_octant->element_list.empty()) {
		
		typename List< Element* >::Iterator *I;
		I=p_octant->element_list.begin();
		for(;I;I=I->next()) {
			
			Element *e=I->get();
			
			if (e->pass==pass)
				continue;
			e->pass=pass;
			
			if (p_AABB.intersects(e->aabb)) {
				
				if (*p_result_idx<p_result_max) {
					
					p_result_array[*p_result_idx] = &e->t;
					(*p_result_idx)++;
				} else {
					
					return; // pointless to continue
				}
			}
		}
	}
	
	
	for (int i=0;i<8;i++) {
		
		if (p_octant->childs[i] && p_octant->childs[i]->aabb.intersects(p_AABB)) {
			cull_AABB(p_octant->childs[i],p_AABB, p_result_array,p_result_idx,p_result_max);
		}
	}	
}

template<class T>
void Octree<T>::cull_segment(Octant *p_octant,const Vector3& p_from, const Vector3& p_to,T** p_result_array,int *p_result_idx,int p_result_max) {

	static Vector3 c,n; // don't annoy

	if (*p_result_idx==p_result_max)
		return; //pointless
	
	if (!p_octant->element_list.empty()) {
		
		typename List< Element* >::Iterator *I;
		I=p_octant->element_list.begin();
		for(;I;I=I->next()) {
			
			Element *e=I->get();
			
			if (e->pass==pass)
				continue;
			e->pass=pass;
									
			if (e->aabb.intersect_segment(p_from,p_to,c,n)) {
				
				if (*p_result_idx<p_result_max) {
					
					p_result_array[*p_result_idx] = &e->t;
					(*p_result_idx)++;
				} else {
					
					return; // pointless to continue
				}
			}
		}
	}
	
	
	for (int i=0;i<8;i++) {
		
		if (p_octant->childs[i] && p_octant->childs[i]->aabb.intersect_segment(p_from,p_to,c,n)) {
			cull_segment(p_octant->childs[i],p_from,p_to, p_result_array,p_result_idx,p_result_max);
		}
	}	
}

template<class T>
int Octree<T>::cull_convex(const ConvexGroup& p_convex,T** p_result_array,int p_result_max) {

	if (!root)
		return 0;
	
	p_convex.read_lock();

	int result_count=0;
	pass++;
	cull_convex(root,p_convex.read(),p_convex.size(),p_result_array,&result_count,p_result_max);
		
	p_convex.read_unlock();
	
	return result_count;
}



template<class T>
int Octree<T>::cull_AABB(const AABB& p_aabb,T** p_result_array,int p_result_max) {


	if (!root)
		return 0;
	
	int result_count=0;
	pass++;
	cull_AABB(root,p_aabb,p_result_array,&result_count,p_result_max);
			
	return result_count;
}


template<class T>
int Octree<T>::cull_segment(const Vector3& p_from, const Vector3& p_to,T** p_result_array,int p_result_max) {

	if (!root)
		return 0;
	
	int result_count=0;
	pass++;
	cull_segment(root,p_from,p_to,p_result_array,&result_count,p_result_max);
			
	return result_count;

}


template<class T>
Octree<T>::Octree(real_t p_smallest,real_t p_subdivide_thresh) {

	smallest_size=p_smallest;
	subdivide_threshold=p_subdivide_thresh;
	root=NULL;
}


#endif
