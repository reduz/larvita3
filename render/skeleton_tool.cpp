//
// C++ Implementation: skeleton_tool
//
// Description: 
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#if 0

#include "skeleton_tool.h"
#include "render/surface_tool.h"
#include "render/renderer.h"

#define EQ_VERTEX_DIST 0.00001

static int count_vertex_bones(const SurfaceTool::Vertex& vertex) {

	int ret = 0;
	for(int j = 0 ; j < vertex.weight.size() ; j++) {

		if(vertex.weight.get(j) > 0)
			ret++;
	}
	return ret;
}

static int count_face_bones(const SkeletonTool::Face& face) {

	return face.affected_bones.size();

	int ret = 0;
	for(int i = 0 ; i < 3 ; i++) {

		const SurfaceTool::Vertex& vertex = face.vertices[i];
		ret += count_vertex_bones(vertex);
	}
	return ret;
}

static int get_face_bone_weight(SkeletonTool::Face& face, int bone_idx) {

	int bone_weight = 0;
	for(int vertex_idx = 0 ; vertex_idx < 3 ; vertex_idx++) {

		SurfaceTool::Vertex& vertex = face.vertices[vertex_idx];
		if(vertex.weight[bone_idx] > 0) {

			bone_weight += vertex.weight[bone_idx];
		}
	}
	return bone_weight;
}

static void eliminate_face_bone(SkeletonTool::Face& face, int p_bone, int p_bone_weight = 0) {
		
	ERR_FAIL_COND(face.affected_bones.find(p_bone) < 0);
	if(p_bone_weight == 0)
		p_bone_weight = get_face_bone_weight(face,p_bone);

	for(int vertex_idx = 0 ; vertex_idx < 3 ; vertex_idx++) {

		Vector<Uint8> &weights = face.vertices[vertex_idx].weight;
		
		if(weights[p_bone] > 0) {

			int bones_qty = count_vertex_bones(face.vertices[vertex_idx]) - 1;

			for(int bone_idx = 0 ; bone_idx < 
						face.vertices[vertex_idx].weight.size() ;
					bone_idx++) {

				if(bone_idx != p_bone)
					if(weights[bone_idx] > 0)
						weights[bone_idx] =	weights[bone_idx] + (p_bone_weight / bones_qty);
			}
			weights[p_bone] = 0;
		}
	}
	int local_bone_idx = face.affected_bones.find(p_bone);
	face.affected_bones.remove(local_bone_idx);
}

static void eliminate_smallest_bone(SkeletonTool::Face& face) {

	Uint32 min_weight = 0xffffffff;
	int min_index = -1;

	int bones_qnt = face.vertices[0].weight.size();
	for(int bone_idx = 0 ; bone_idx < bones_qnt ; bone_idx++) {

		Uint8 bone_weight = get_face_bone_weight(face,bone_idx);

		if(bone_weight > 0 && bone_weight < min_weight) {

			min_weight = bone_weight;
			min_index = bone_idx;
		}
	}

	if(min_index > -1) 
		eliminate_face_bone(face,min_index,min_weight);
}

void SkeletonTool::eliminate_bone(int bone) {

	for(int i = 0 ; i < this->faces.size() ; i++) {
		for(int j = 0 ; j < 3 ; j++) {

			this->faces[i].vertices[j].weight.remove(bone);
		}
	}
}

void SkeletonTool::eliminate_zero_bones() {

	for(int i = 0 ; i < this->skeleton.size() ; i++) {

		if(this->count_bone_vertices(i) == 0) {

			this->eliminate_bone(i--);
		}
	}
}

void SkeletonTool::eliminate_extra_bones() {

	for(int face_idx = 0 ; face_idx < this->faces.size() ; face_idx++) {

		Face& face = this->faces[face_idx];
		while(count_face_bones(face) > 8) {

			eliminate_smallest_bone(face);
		}/*
		for(int i = 0 ; i < face.affected_bones.size() ; i++) {

			for(int v_idx = 0 ; v_idx < 3 ; v_idx++) {

				if(face.vertices[v_idx].weight[face.affected_bones[i]] < 4) {

					eliminate_face_bone(face,face.affected_bones[i--]);
					break;
				}
			}
		}*/
	}

	//this->eliminate_zero_bones();
}

int SkeletonTool::count_bone_vertices(int bone) {

	int ret = 0;
	for(int i = 0 ; i < this->faces.size() ; i++) {

		for(int j = 0 ; j < 3 ; j++) {

			if(this->faces[i].vertices[j].weight.get(bone) > 0) {

				ret++;
			}
		}
	}
	return ret;
}

Vector<int> SkeletonTool::get_faces_from_bone(int p_bone, int* p_faces) {

	Vector<int> ret;
	for(int i = 0 ; i < this->faces.size() ; i++) {

		for(int j = 0 ; j < 3 ; j++) {
			if((this->faces.get(i).vertices[j].weight.get(p_bone) > 0) &&
					(p_faces[i] == -1)) {

				p_faces[i] = p_bone;
				ret.push_back(i);
			}
		}
	}
	return ret;
}

static int get_bone_weight(const Vector<SkeletonTool::Face>& working_faces, const Vector<int>& bones, int bone_cnt) {

	/*
	Vector<int> aux_aff_bones;
	for(int face_cnt = 0 ; face_cnt < working_faces.size() ; face_cnt++) {

		const SkeletonTool::Face& face = working_faces[face_cnt];
		for(int aff_bone_cnt = 0 ; aff_bone_cnt < bones.size() ; aff_bone_cnt++) {

			if(aff_bone_cnt != bone_cnt) {

				int aff_bone_idx = bones[aff_bone_cnt];
				if(face.affected_bones.find(aff_bone_idx) >= 0) {

					if(aux_aff_bones.find(aff_bone_idx) < 0)
						aux_aff_bones.push_back(aff_bone_idx);
				}
			}
		}
	}
	return aux_aff_bones.size();
	*/
	int ret = 0;

	for(int face_cnt = 0 ; face_cnt < working_faces.size() ; face_cnt++) {

		const SkeletonTool::Face& face = working_faces[face_cnt];
		if(face.affected_bones.find(bones[bone_cnt]) >= 0)
			ret++;
	}
	return ret;
}

static void print_bone(const Vector<int>& bones,int bone_idx,int level,const Vector<SkeletonTool::Face>& faces) {

	for(int i = 0 ; i < level ; i++)
		printf(" ");
	printf("%d/\n",bone_idx);/*
	for(int i = 0 ; i < faces.size() ; i++) {

		if(faces[i].affected_bones.find(bone_idx) >= 0)
			printf("%d ",i);
	}
	printf("\n");*/

	for(int i = bone_idx ; i < bones.size() ; i++) {

		if(bones[i] == bone_idx)
			print_bone(bones,i,level + 1,faces);
	}
}

static void print_skeleton_tool(const Vector<SkeletonTool::Face>& faces, const Vector<int> bones) {

	for(int b_idx = 0 ; b_idx < bones.size() ; b_idx++)
		printf("|%d|",bones[b_idx]);
	printf("\n");

	for(int b_idx = 0 ; b_idx < bones.size() ; b_idx++) {

		if(bones[b_idx] == -1)
			print_bone(bones,b_idx,0,faces);
	}
}

DVector<SurfaceTool> SkeletonTool::commit() {

	print_skeleton_tool(this->faces, this->skeleton);

	this->eliminate_extra_bones(); // eliminates bones from faces with more than 8

	DVector<SurfaceTool> ret;

	// this will point to which SurfaceTool in ret will each face be in
	int* face_positions = memnew_arr(int,this->faces.size()); // holds to which SurfaceTool is each face assigned
	for(int* p = face_positions ; p < face_positions + this->faces.size() ; p++)
		*p = -1;

	for(int i = 0 ; i < this->skeleton.size() ; i++) {
		//Vector<int> faces_idxs = this->get_faces_from_bone(i,face_positions); // get the faces affected by my bone

		for(Vector<int> faces_idxs = this->get_faces_from_bone(i,face_positions); // get the faces affected by my bone
				faces_idxs.size() > 0 ;
				faces_idxs = this->get_faces_from_bone(i,face_positions)) {

			Vector<SkeletonTool::Face> working_faces; // holds the faces affected by this bone
			for(int j = 0 ; j < faces_idxs.size() ; j++) {

				working_faces.push_back(this->faces[faces_idxs[j]]);
			}
			Vector<int> bones;

			// here I create bones by traversing faces
			for(int face_cnt = 0 ; face_cnt < working_faces.size() ; face_cnt++) {

				// this is the index in this->faces
				int face_idx = faces_idxs.get(face_cnt);
				Face& face = this->faces[face_idx];

				// check each bone, and add the ones that matter to this face into bone_idx.
				for(int bone_idx = 0 ; bone_idx < face.affected_bones.size() ; bone_idx++) {

					int bn_id = face.affected_bones[bone_idx];
					if(bones.find(bn_id) < 0)
						bones.push_back(this->faces[face_idx].affected_bones[bone_idx]);
				}
			}

			// now we have all faces used by bone #i and all bones used by those.
			// We need to delete bones until we have 8, now
			while(bones.size() > 8) {

				// the "bone weight" we define here is an int that tells how many
				// other bones are affected by the faces that are affected by a
				// given bone. This is used because we want to delete the bone that
				// will minimize the amount of bones added to the result group
				// including the faces affected by it in the next iterations
				int min_bone_weight = working_faces.size() + 1;	// a given bone cannot affect more
									// bones than what we have
				int min_bone_idx = -1;	// index of the lightest bone in the skeleton
				int min_bone_local_idx = -1;	// index of the lightest bone in "bones"
				for(int bone_cnt = 0 ; bone_cnt < bones.size() ; bone_cnt++) {

					int bone_idx = bones.get(bone_cnt);
					if(bone_idx == i) // do not delete my bone
						continue;

					int bone_weight = 0;
					bone_weight = get_bone_weight(working_faces,bones,bone_cnt);

					if(bone_weight < min_bone_weight) {

						min_bone_weight = bone_weight;
						min_bone_idx = bone_idx;
						min_bone_local_idx = bone_cnt;
					}
				}

				// I have the bone to remove from this group in min_bone_idx now
				for(int face_cnt = 0 ; face_cnt < faces_idxs.size() ; face_cnt++) {

					int face_idx = faces_idxs[face_cnt];
					for(int vertex_idx = 0 ; vertex_idx < 3 ; vertex_idx++) {

						if(this->faces[face_idx].vertices[vertex_idx].weight.get(min_bone_idx) > 0) {

							ERR_CONTINUE(face_idx < 0);
							ERR_CONTINUE(face_idx >= this->faces.size());
							face_positions[face_idx] = -1;
							ERR_CONTINUE(face_cnt < 0);
							faces_idxs.remove(face_cnt--);
							break;
						}
					}
				}
				bones.remove(min_bone_local_idx);
			}

			// Here we have a group of 8 bones or less, and faces that are affected by them
			// now we have to build retval
			//retval.
			if(bones.size() > 0) {

				DVector<SurfaceTool::Vertex> ret_vertices;


				for(int face_cnt = 0 ; face_cnt < faces_idxs.size() ; face_cnt++) {

					int face_idx = faces_idxs.get(face_cnt);
					Face face = this->faces.get(face_idx);
					for(int vertex_idx = 0 ; vertex_idx < 3 ; vertex_idx++) {

						SurfaceTool::Vertex& vertex = face.vertices[vertex_idx];
						for(int bone_idx = 0, bone_cnt = 0 ; bone_idx < vertex.weight.size() ; bone_idx++,bone_cnt++) {

							ERR_CONTINUE(bone_idx < 0);
							ERR_CONTINUE(bone_idx >= vertex.weight.size());
							if(bones.find(bone_cnt) < 0) {

								ERR_CONTINUE(vertex.weight[bone_idx] != 0);
								vertex.weight.remove(bone_idx--);
							}
						}
						ret_vertices.push_back(face.vertices[vertex_idx]);
					}
				}

				// FIX THIS LINE WHEN USING
				int bone_fmt = Surface::FORMAT_ARRAY_WEIGHT_0 << (bones.size()-1);
				SurfaceTool retval(this->surface_tool->get_primitive_type(),
						this->surface_tool->get_format()|bone_fmt,NULL,ret_vertices);
				for(int bone_cnt = 0 ; bone_cnt < bones.size() ; bone_cnt++) {

					int bone_idx = bones[bone_cnt];
					retval.set_weigh_bone_index(bone_cnt,bone_idx);
				}
				ret.push_back(retval);
			}
		}
	}

	ERR_FAIL_COND_V(!this->check_retval(ret),ret);
	for(int i = 0 ; i < this->faces.size() ; i++) {

		printf("face %d is in %d\n",i,face_positions[i]);
		//ERR_FAIL_COND_V(face_positions[i] < 0,ret);
	}

	memdelete_arr(face_positions);
	return ret;
}

bool SkeletonTool::check_retval(const DVector<SurfaceTool>& ret) {

	Vector<bool> vertices;
	vertices.resize(this->faces.size()*3);
	for(int i = 0 ; i < vertices.size() ; i++)
		vertices[i] = false;
	int vertex_idx = 0;

	//printf("checking surfaces\n");
	for(int i = 0 ; i < ret.size() ; i++) {

		//printf("surface %d\n",i);
		DVector<SurfaceTool::Vertex> va = ret[i].get_vertex_array();
		ERR_FAIL_COND_V(va[0].weight.size() > 8,false);
		for(int v = 0 ; v < va.size() ; v++) {

			//printf(" vertex %d\n",v);
			ERR_FAIL_COND_V(va[v].weight.size() != va[0].weight.size(),false);
/*
			bool found = false;
			SurfaceTool::Vertex v1 = va[v];
			for(int f_idx = 0 ; f_idx < this->faces.size() && !found ; f_idx++) {
				printf("  face %d\n",f_idx);
				for(int f_vidx = 0 ; f_vidx < 3 && !found ; f_vidx++){
					SurfaceTool::Vertex v2 = this->faces[f_idx].vertices[f_vidx];
					if(v1.vertex == v2.vertex) {
						if(!vertices[f_idx*3+f_vidx]) {
							vertices[f_idx*3+f_vidx] = found = true;
						}
					}
				}
			}
			ERR_FAIL_COND_V(!found,false);*/
		}
	}
/*
	printf("checking faces\n");
	for(int f_idx = 0 ; f_idx < this->faces.size() ; f_idx++) {
		printf("face %d\n",f_idx);
		for(int f_vidx = 0 ; f_vidx < 3 ; f_vidx++){

			bool found = false;
			SurfaceTool::Vertex v2 = this->faces[f_idx].vertices[f_vidx];

			for(int i = 0 ; i < ret.size() && !found ; i++) {
				DVector<SurfaceTool::Vertex> va = ret[i].get_vertex_array();
				for(int v = 0 ; v < va.size() && !found ; v++) {
					SurfaceTool::Vertex v1 = va[v];
					if(v1.vertex == v2.vertex) {
						if(vertices[f_idx*3+f_vidx]) {
							vertices[f_idx*3+f_vidx] = false;
							found = true;
						}
					}
				}
			}
			ERR_FAIL_COND_V(!found,false);
		}
	}
*/	
	return true;
}

void SkeletonTool::add_weight(int p_vertex_index, int p_weight_index, int p_bone_index, Uint8 weight) {

	int face_idx = p_vertex_index / 3;
	ERR_FAIL_COND(face_idx > this->faces.size());

	int vertex_idx = p_vertex_index % 3;
	Face& face = this->faces[face_idx];
	SurfaceTool::Vertex& vertex = face.vertices[vertex_idx];

	if(vertex.weight.size() <= p_bone_index) {

		vertex.weight.resize(p_bone_index + 1);
	}

	vertex.weight.set(p_bone_index,weight);
	if(face.affected_bones.find(p_bone_index) < 0)
		face.affected_bones.push_back(p_bone_index);
}

void SkeletonTool::set_skeleton( Vector<int> p_skeleton ) {

	this->skeleton = p_skeleton;

	for(int i = 0 ; i < this->faces.size() ; i++) {
		this->faces[i].affected_bones.resize(0);
		for(int j = 0 ; j < 3 ; j++) {

			this->faces[i].vertices[j].weight.resize(p_skeleton.size());
			for(int bone_idx = 0 ; bone_idx < p_skeleton.size() ; bone_idx++)
				this->faces[i].vertices[j].weight.set(bone_idx,0);
		}
	}
}

SkeletonTool::SkeletonTool(const SurfaceTool* st) {

	this->surface_tool = st;
	const DVector< SurfaceTool::Vertex> v_vertices = this->surface_tool->get_vertex_array();
	ERR_FAIL_COND((v_vertices.size() % 3) != 0);

	v_vertices.read_lock();
	const SurfaceTool::Vertex* vertices = v_vertices.read();

	this->faces.resize(v_vertices.size() / 3);

	for(int i = 0 ; i < faces.size() ; i++) {
		Face &face=faces[i];
		face.vertices[0] = vertices[i*3+0];
		face.vertices[1] = vertices[i*3+1];
		face.vertices[2] = vertices[i*3+2];
		for(int v_idx = 0 ; v_idx < 3 ; v_idx++) {

			/*
			for(int bone_idx = 0 ; bone_idx <  face.vertices[v_idx].weight.size() ; bone_idx++) {

				if(face.vertices[v_idx].weight[bone_idx] > 0) {

					if(face.affected_bones.find(bone_idx) < 0) {

						face.affected_bones.push_back(bone_idx);
					}
				}
			}
			*/
		}
		
	}

	v_vertices.read_unlock();

	this->skeleton.resize(0);
}
#endif
