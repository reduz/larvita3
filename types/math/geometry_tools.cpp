//
// C++ Implementation: geometry_tools
//
// Description:
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "geometry_tools.h"
#include "variant.h"
#include "print_string.h"

struct _FaceClassify {

	struct _Link {

		int face;
		int edge;
		void clear() { face=-1; edge=-1; }
		_Link() { face=-1; edge=-1; }
	};
	bool valid;
	int group;
	_Link links[3];
	Face3 face;
	_FaceClassify() {
		group=-1;
		valid=false;
	};
};

static bool _connect_faces(_FaceClassify *p_faces, int len, int p_group) {
	/* connect faces, error will occur if an edge is shared between more than 2 faces */
	/* clear connections */

	bool error=false;

	for (int i=0;i<len;i++) {

		for (int j=0;j<3;j++) {

			p_faces[i].links[j].clear();
		}
	}

	for (int i=0;i<len;i++) {

		if (p_faces[i].group!=p_group)
			continue;
		for (int j=i+1;j<len;j++) {

			if (p_faces[j].group!=p_group)
				continue;

			for (int k=0;k<3;k++) {

				Vector3 vi1=p_faces[i].face.vertex[k];
				Vector3 vi2=p_faces[i].face.vertex[(k+1)%3];

				for (int l=0;l<3;l++) {

					Vector3 vj2=p_faces[j].face.vertex[l];
					Vector3 vj1=p_faces[j].face.vertex[(l+1)%3];

					if (vi1.distance_to(vj1)<0.00001 &&
					    vi2.distance_to(vj2)<0.00001
					   ) {
						if (p_faces[i].links[k].face!=-1) {

							ERR_PRINT("already linked\n");
							error=true;
							break;
						}
						if (p_faces[j].links[l].face!=-1) {

							ERR_PRINT("already linked\n");
							error=true;
							break;
						}

						p_faces[i].links[k].face=j;
						p_faces[i].links[k].edge=l;
						p_faces[j].links[l].face=i;
						p_faces[j].links[l].edge=k;
					   }
				}
				if (error)
					break;

			}
			if (error)
				break;

		}
		if (error)
			break;
	}

	for (int i=0;i<len;i++) {

		p_faces[i].valid=true;
		for (int j=0;j<3;j++) {

			if (p_faces[i].links[j].face==-1)
				p_faces[i].valid=false;
		}
		/*printf("face %i is valid: %i, group %i. connected to %i:%i,%i:%i,%i:%i\n",i,p_faces[i].valid,p_faces[i].group,
			p_faces[i].links[0].face,
			p_faces[i].links[0].edge,
			p_faces[i].links[1].face,
			p_faces[i].links[1].edge,
			p_faces[i].links[2].face,
			p_faces[i].links[2].edge);*/
	}
	return error;
}

static bool _group_face(_FaceClassify *p_faces, int len, int p_index,int p_group) {

	if (p_faces[p_index].group>=0)
		return false;

	p_faces[p_index].group=p_group;

	for (int i=0;i<3;i++) {

		ERR_FAIL_INDEX_V(p_faces[p_index].links[i].face,len,true);
		_group_face(p_faces,len,p_faces[p_index].links[i].face,p_group);
	}

	return true;
}


DVector< DVector< Face3 > > GeometryTools::separate_objects( DVector< Face3 > p_array ) {

	DVector< DVector< Face3 > > objects;

	int len = p_array.size();

	p_array.read_lock();
	const Face3* arrayptr = p_array.read();

	DVector< _FaceClassify> fc;

	fc.resize( len );

	fc.write_lock();
	_FaceClassify * _fcptr = fc.write();

	for (int i=0;i<len;i++) {

		_fcptr[i].face=arrayptr[i];
	}

	printf("connect faces\n");
	bool error=_connect_faces(_fcptr,len,-1);

	if (error) {

		p_array.read_unlock();
		fc.write_lock();

		ERR_FAIL_COND_V(error, DVector< DVector< Face3 > >() ); // invalid geometry
	}

	/* group connected faces in separate objects */

	printf("group faces\n");

	int group=0;
	for (int i=0;i<len;i++) {

		if (!_fcptr[i].valid)
			continue;
		if (_group_face(_fcptr,len,i,group)) {
			group++;
		}
	}

	printf("created %i groups\n",group);

	/* group connected faces in separate objects */


	for (int i=0;i<len;i++) {

		_fcptr[i].face=arrayptr[i];
	}

	if (group>=0) {

		objects.resize(group);
		objects.write_lock();
		DVector< Face3 > *group_faces = objects.write();

		for (int i=0;i<len;i++) {
			if (!_fcptr[i].valid)
				continue;
			if (_fcptr[i].group>=0 && _fcptr[i].group<group) {

				group_faces[_fcptr[i].group].push_back( _fcptr[i].face );
			}
		}
		objects.write_unlock();
	}

	p_array.read_unlock();
	fc.write_unlock();
	printf("creation successful\n");
	return objects;

}

enum _CellFlags {
	
	_CELL_SOLID=1,
 	_CELL_EXTERIOR=2,
  	_CELL_STEP_MASK=0x1C,
   	_CELL_STEP_NONE=0<<2,
	_CELL_STEP_Y_POS=1<<2,
 	_CELL_STEP_Y_NEG=2<<2,
  	_CELL_STEP_X_POS=3<<2,
   	_CELL_STEP_X_NEG=4<<2,
    	_CELL_STEP_Z_POS=5<<2,
     	_CELL_STEP_Z_NEG=6<<2,
      	_CELL_STEP_DONE=7<<2,
	_CELL_PREV_MASK=0xE0,
	_CELL_PREV_NONE=0<<5,
	_CELL_PREV_Y_POS=1<<5,
	_CELL_PREV_Y_NEG=2<<5,
	_CELL_PREV_X_POS=3<<5,
	_CELL_PREV_X_NEG=4<<5,
	_CELL_PREV_Z_POS=5<<5,
	_CELL_PREV_Z_NEG=6<<5,
	_CELL_PREV_FIRST=7<<5,
	
};

static inline void _plot_face(Uint8*** p_cell_status,int x,int y,int z,int len_x,int len_y,int len_z,const Vector3& voxelsize,const Face3& p_face) {

	AABB aabb( Vector3(x,y,z),Vector3(len_x,len_y,len_z));
	aabb.pos=aabb.pos*voxelsize;
	aabb.size=aabb.size*voxelsize;

	if (!p_face.intersects_aabb(aabb))
		return;

	if (len_x==1 && len_y==1 && len_z==1) {

		p_cell_status[x][y][z]=_CELL_SOLID;
		return;
	}
	
	
 
	int div_x=len_x>1?2:1;
	int div_y=len_y>1?2:1;
	int div_z=len_z>1?2:1;

#define _SPLIT(m_i,m_div,m_v,m_len_v,m_new_v,m_new_len_v)\
	if (m_div==1) {\
		m_new_v=m_v;\
		m_new_len_v=1;	\
	} else if (m_i==0) {\
		m_new_v=m_v;\
		m_new_len_v=m_len_v/2;\
	} else {\
		m_new_v=m_v+m_len_v/2;\
		m_new_len_v=m_len_v-m_len_v/2;		\
	}

	int new_x;
	int new_len_x;
	int new_y;
	int new_len_y;
	int new_z;
	int new_len_z;

	for (int i=0;i<div_x;i++) {
		
		
		_SPLIT(i,div_x,x,len_x,new_x,new_len_x);
				
		for (int j=0;j<div_y;j++) {

			_SPLIT(j,div_y,y,len_y,new_y,new_len_y);
			
			for (int k=0;k<div_z;k++) {

				_SPLIT(k,div_z,z,len_z,new_z,new_len_z);

				_plot_face(p_cell_status,new_x,new_y,new_z,new_len_x,new_len_y,new_len_z,voxelsize,p_face);
			}
		}
	}
}

static inline void _mark_outside(Uint8*** p_cell_status,int x,int y,int z,int len_x,int len_y,int len_z) {

	if (p_cell_status[x][y][z]&3)
		return; // nothing to do, already used and/or visited
	
	p_cell_status[x][y][z]=_CELL_PREV_FIRST;
	
	while(true) {
		
		Uint8 &c = p_cell_status[x][y][z];
		
		//printf("at %i,%i,%i\n",x,y,z);
		
		if ( (c&_CELL_STEP_MASK)==_CELL_STEP_NONE) {	
			/* Haven't been in here, mark as outside */
			p_cell_status[x][y][z]|=_CELL_EXTERIOR;
			//printf("not marked as anything, marking exterior\n");
		}
		
		//printf("cell step is %i\n",(c&_CELL_STEP_MASK));
		
		if ( (c&_CELL_STEP_MASK)!=_CELL_STEP_DONE) {
			/* if not done, increase step */
			c+=1<<2;
			//printf("incrementing cell step\n");
		}
		
		if ( (c&_CELL_STEP_MASK)==_CELL_STEP_DONE) {
			/* Go back */
			//printf("done, going back a cell\n");
			
			switch(c&_CELL_PREV_MASK) {
				case _CELL_PREV_FIRST: {
					//printf("at end, finished marking\n");				
					return;
				} break;
				case _CELL_PREV_Y_POS: {	
					y++;
					ERR_FAIL_COND(y>=len_y);
				} break;
				case _CELL_PREV_Y_NEG: {
					y--;
					ERR_FAIL_COND(y<0);
				} break;
				case _CELL_PREV_X_POS: {
					x++;
					ERR_FAIL_COND(x>=len_x);
				} break;
				case _CELL_PREV_X_NEG: {
					x--;
					ERR_FAIL_COND(x<0);
				} break;
				case _CELL_PREV_Z_POS: {
					z++;
					ERR_FAIL_COND(z>=len_z);
				} break;
				case _CELL_PREV_Z_NEG: {
					z--;
					ERR_FAIL_COND(z<0);
				} break;
				default: {
					ERR_FAIL();
				}
			}
			continue;
		}
		
		//printf("attempting new cell!\n");
		
		int next_x=x,next_y=y,next_z=z;
		Uint8 prev=0;
		
		switch(c&_CELL_STEP_MASK) {
			
			case _CELL_STEP_Y_POS: {
				
				next_y++;
				prev=_CELL_PREV_Y_NEG;
			} break;
			case _CELL_STEP_Y_NEG: {
				next_y--;
				prev=_CELL_PREV_Y_POS;
			} break;
			case _CELL_STEP_X_POS: {
				next_x++;
				prev=_CELL_PREV_X_NEG;
			} break;
			case _CELL_STEP_X_NEG: {
				next_x--;
				prev=_CELL_PREV_X_POS;
			} break;
			case _CELL_STEP_Z_POS: {
				next_z++;
				prev=_CELL_PREV_Z_NEG;				
			} break;
			case _CELL_STEP_Z_NEG: {
				next_z--;
				prev=_CELL_PREV_Z_POS;				
			} break;
			default: ERR_FAIL(); 
			
		}
		
		//printf("testing if new cell will be ok...!\n");
		
		if (next_x<0 || next_x>=len_x)
			continue;
		if (next_y<0 || next_y>=len_y)
			continue;
		if (next_z<0 || next_z>=len_z)
			continue;
			
		//printf("testing if new cell is traversable\n");
			
		if (p_cell_status[next_x][next_y][next_z]&3)
			continue;
		
		//printf("move to it\n");
		
		x=next_x;
		y=next_y;
		z=next_z;
		p_cell_status[x][y][z]|=prev;
	}
}

static inline void _build_faces(Uint8*** p_cell_status,int x,int y,int z,int len_x,int len_y,int len_z,DVector<Face3>& p_faces) {
	
	ERR_FAIL_INDEX(x,len_x);
	ERR_FAIL_INDEX(y,len_y);
	ERR_FAIL_INDEX(z,len_z);
	
	if (p_cell_status[x][y][z]&_CELL_EXTERIOR)
		return;	
	
	static const Vector3 vertices[8]={
		Vector3(0,0,0),
		Vector3(0,0,1),
		Vector3(0,1,0),
		Vector3(0,1,1),
		Vector3(1,0,0),
		Vector3(1,0,1),
		Vector3(1,1,0),
		Vector3(1,1,1),
	};
	
#define vert(m_idx) Vector3( (m_idx&4)>>2, (m_idx&2)>>1, m_idx&1 )

	static const Uint8 indices[6][4]={
		{7,6,4,5},		
		{7,3,2,6},
		{7,5,1,3},				
		{0,2,3,1},			
		{0,1,5,4},		
		{0,4,6,2},

	};
/*	

		{0,1,2,3},			
		{0,1,4,5},		
		{0,2,4,6},
		{4,5,6,7},		
		{2,3,7,6},
		{1,3,5,7},				

		{0,2,3,1},			
		{0,1,5,4},		
		{0,4,6,2},
		{7,6,4,5},		
		{7,3,2,6},
		{7,5,1,3},				
*/
	
	for (int i=0;i<6;i++) {
			
		Vector3 face_points[4];
		int disp_x=x+((i%3)==0?((i<3)?1:-1):0);
		int disp_y=y+(((i-1)%3)==0?((i<3)?1:-1):0);
		int disp_z=z+(((i-2)%3)==0?((i<3)?1:-1):0);
		
		bool plot=false;
		
		if (disp_x<0 || disp_x>=len_x)
			plot=true;
		if (disp_y<0 || disp_y>=len_y)
			plot=true;
		if (disp_z<0 || disp_z>=len_z)
			plot=true;
		
		if (!plot && (p_cell_status[disp_x][disp_y][disp_z]&_CELL_EXTERIOR))
			plot=true;
			
		if (!plot)
			continue;
		
		for (int j=0;j<4;j++)
			face_points[j]=vert( indices[i][j] ) + Vector3(x,y,z);
		
		p_faces.push_back( 
			Face3( 
				face_points[0],
				face_points[1],
				face_points[2]
			     )
		);
    
		p_faces.push_back( 
			Face3( 
				face_points[2],
				face_points[3],
				face_points[0]
			     )
		);
		
	}	

}

DVector< Face3 > GeometryTools::wrap_geometry( DVector< Face3 > p_array ) {

#define _MIN_SIZE 1.0
#define _MAX_LENGTH 50

	int face_count=p_array.size();
	p_array.read_lock();
	const Face3 *faces = p_array.read();

	AABB global_aabb;

	for(int i=0;i<face_count;i++) {

		if (i==0) {

			global_aabb=faces[i].get_aabb();
		} else {

			global_aabb.merge_with( faces[i].get_aabb() );
		}
	}
	
	global_aabb.grow_by(0.01); // avoid numerical error
	
	print_line("AABB Size: "+Variant(global_aabb).get_string());

	// determine amount of cells in grid axis
	int div_x,div_y,div_z;

	if (global_aabb.size.x/_MIN_SIZE<_MAX_LENGTH)
		div_x=(int)(global_aabb.size.x/_MIN_SIZE)+1;
	else
		div_x=_MIN_SIZE;

	if (global_aabb.size.y/_MIN_SIZE<_MAX_LENGTH)
		div_y=(int)(global_aabb.size.y/_MIN_SIZE)+1;
	else
		div_y=_MIN_SIZE;

	if (global_aabb.size.z/_MIN_SIZE<_MAX_LENGTH)
		div_z=(int)(global_aabb.size.z/_MIN_SIZE)+1;
	else
		div_z=_MIN_SIZE;

	Vector3 voxelsize=global_aabb.size;
	voxelsize.x/=div_x;
	voxelsize.y/=div_y;
	voxelsize.z/=div_z;

	print_line("voxelsize: "+Variant(voxelsize).get_string());

	// create and initialize cells to zero

	Uint8 ***cell_status=memnew_arr(Uint8**,div_x);
	for(int i=0;i<div_x;i++) {

		cell_status[i]=memnew_arr(Uint8*,div_y);

		for(int j=0;j<div_y;j++) {

			cell_status[i][j]=memnew_arr(Uint8,div_z);

			for(int k=0;k<div_z;k++) {

				cell_status[i][j][k]=0;
			}
		}
	}

	// plot faces into cells
	
	for (int i=0;i<face_count;i++) {

		Face3 f=faces[i];
		for (int j=0;j<3;j++) {

			f.vertex[j]-=global_aabb.pos;
		}
		_plot_face(cell_status,0,0,0,div_x,div_y,div_z,voxelsize,f);
	}

	
	p_array.read_unlock();

	// determine which cells connect to the outside by traversing the outside and recursively flood-fill marking

	for (int i=0;i<div_x;i++) {
		
		for (int j=0;j<div_y;j++) {
		
			_mark_outside(cell_status,i,j,0,div_x,div_y,div_z);
			_mark_outside(cell_status,i,j,div_z-1,div_x,div_y,div_z);
		}
	}
	
	for (int i=0;i<div_z;i++) {
		
		for (int j=0;j<div_y;j++) {
		
			_mark_outside(cell_status,0,j,i,div_x,div_y,div_z);
			_mark_outside(cell_status,div_x-1,j,i,div_x,div_y,div_z);
		}
	}
	
	for (int i=0;i<div_x;i++) {
		
		for (int j=0;j<div_z;j++) {
		
			_mark_outside(cell_status,i,0,j,div_x,div_y,div_z);
			_mark_outside(cell_status,i,div_y-1,j,div_x,div_y,div_z);
		}
	}
	
	// build faces for the inside-outside cell divisors
	
	DVector<Face3> wrapped_faces;
	
	for (int i=0;i<div_x;i++) {
		
		for (int j=0;j<div_y;j++) {
		
			for (int k=0;k<div_z;k++) {
			
				_build_faces(cell_status,i,j,k,div_x,div_y,div_z,wrapped_faces);				
			}
		}
	}
	
	// transform face vertices to global coords
	
	int wrapped_faces_count=wrapped_faces.size();
	wrapped_faces.write_lock();
	Face3* wrapped_faces_ptr=wrapped_faces.write();
	
	for(int i=0;i<wrapped_faces_count;i++) {
	
		for(int j=0;j<3;j++) {
		
			Vector3& v = wrapped_faces_ptr[i].vertex[j];
			v=v*voxelsize;
			v+=global_aabb.pos;
		}
	}
	
	wrapped_faces.write_unlock();
	
	// clean up grid 
	
	for(int i=0;i<div_x;i++) {

		for(int j=0;j<div_y;j++) {

			memdelete_arr( cell_status[i][j] );
		}
		
		memdelete_arr( cell_status[i] );
	}
	
	memdelete_arr(cell_status);

	
	return wrapped_faces;
}

