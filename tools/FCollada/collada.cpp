#ifdef COLLADA_ENABLED
#include "collada.h"
#include "iapi_db.h"
#include "iapi_persist.h"
#include "os/dir_access.h"

#include "tools/FCollada/FCollada.h"
#include "tools/FCollada/FCDocument/FCDocument.h"
#include "tools/FCollada/FCDocument/FCDSceneNode.h"
#include "tools/FCollada/FCDocument/FCDSceneNodeTools.h"
#include "tools/FCollada/FCDocument/FCDocumentTools.h"
#include "tools/FCollada/FCDocument/FCDController.h"
#include "tools/FCollada/FCDocument/FCDSkinController.h"
#include "tools/FCollada/FCDocument/FCDControllerInstance.h"
#include "tools/FCollada/FCDocument/FCDMaterial.h"
#include "tools/FCollada/FCDocument/FCDMaterialInstance.h"
#include "tools/FCollada/FCDocument/FCDEffect.h"
#include "tools/FCollada/FCDocument/FCDGeometry.h"
#include "tools/FCollada/FCDocument/FCDGeometryMesh.h"
#include "tools/FCollada/FCDocument/FCDGeometryPolygons.h"
#include "tools/FCollada/FCDocument/FCDGeometrySource.h"
#include "tools/FCollada/FCDocument/FCDGeometryPolygonsInput.h"
#include "tools/FCollada/FCDocument/FCDGeometryPolygonsTools.h"
#include "tools/FCollada/FCDocument/FCDEffectStandard.h"
#include "tools/FCollada/FCDocument/FCDTexture.h"
#include "tools/FCollada/FCDocument/FCDImage.h"
#include "render/surface_tool.h"


#include "types/math/matrix4.h"
//#include "animation/animation.h"

//#include "main/main.h"
#include "render/renderer.h"

#include "scene/nodes/node.h"
#include "scene/nodes/dummy_node.h"
#include "scene/nodes/skeleton_node.h"
#include "scene/nodes/scene_node.h"
#include "scene/nodes/mesh_node.h"

static bool is_init = false;

static Matrix4 transform_matrix(Matrix4 mat) {

	//*
	for (int col=0; col<4; col++) {
	
		double tmp = mat.get(col, 1);
		mat.set(col, 1, mat.get(col, 2));
		mat.set(col, 2, tmp);
	};

	for (int row = 0; row<4; row++) {

		double tmp = mat.get(1, row);
		mat.set(1,row, mat.get(2, row));
		mat.set(2, row, tmp);
	};

	mat.set(2, 0, -mat.get(2, 0));
	mat.set(2, 1, -mat.get(2, 1));
	mat.set(2, 3, -mat.get(2, 3));
	mat.set(0, 2, -mat.get(0, 2));
	mat.set(1, 2, -mat.get(1, 2));
	
	double aux = mat.get(3, 0);
	mat.set(3, 0, mat.get(3, 2));
	mat.set(3, 2, -aux);
	
	//	*/

	return mat;
};

static Matrix4 to_matrix4(FMMatrix44 p_mat) {

	Matrix4 ret;
	
	/*
	//p_mat = p_mat.Transposed();
	
	for (int col = 0; col<4; col++) {
		
		for (int row = 0; row < 4; row++) {
			
			ret.set(row, col, p_mat[row][col]);
		};
	};
	//	*/

	real_t* d = (real_t*)ret.elements;
	float* f = (float*)&p_mat.m;
	for (int i=0; i<16; i++) {
	
		d[i] = f[i];
	};

	/* // from dae_import
	for (int col = 0; col < 4; col++) {
		
		double tmp = ret.get(col, 1);
		ret.set(col, 1, ret.get(col, 2));
		ret.set(col, 2, tmp);
	};

	for (int row = 0; row < 4; row++) {
		
		double tmp = ret.get(1, row);
		ret.set(1, row, ret.get(2, row));
		ret.set(2, row, tmp);
	};

	ret.set(2, 0, -ret.get(2, 0));
	ret.set(2, 1, -ret.get(2, 1));
	ret.set(2, 3, -ret.get(2, 3));
	ret.set(0, 2, -ret.get(0, 2));
	ret.set(1, 2, -ret.get(1, 2));
	ret.set(3, 2, -ret.get(3, 2));

	//	*/
	
	return ret;
};

static Vector3 transform_vector3(Vector3 p_vec) {

	return p_vec;
	
	Vector3 ret;
	ret.y = p_vec.z;
	ret.z = -p_vec.y;
	ret.x = p_vec.x;

	return ret;
};

static bool get_matrix_from_root(FCDSceneNode* p_scene, FCDSceneNode* p_parent, FMMatrix44& mat) {
	
	for (int i=0; i<(int)p_parent->GetChildrenCount(); i++) {
		
		FCDSceneNode* child = p_parent->GetChild(i);
		FMMatrix44 cmat = mat * child->ToMatrix();
		if (child == p_scene) {

			mat = cmat;
			return true;
		} else {
			
			bool ret = get_matrix_from_root(p_scene, child, cmat);
			if (ret) {
				mat = cmat;
				return true;
			};
		};
	};
	
	return false;
};

static Matrix4 get_matrix_from_root(FCDSceneNode* p_scene) {
	
	FCDSceneNode* root = p_scene->GetDocument()->GetVisualSceneInstance();

	FMMatrix44 mat = root->ToMatrix();
	bool ret = get_matrix_from_root(p_scene, root, mat);
	
	ERR_FAIL_COND_V(!ret, Matrix4());
	
	return to_matrix4(mat);
};


static Matrix4 get_global_matrix(FCDSceneNode* p_scene) {

	//return get_matrix_from_root(p_scene);
	
	return to_matrix4(p_scene->CalculateWorldTransform());
	
	/*
	Matrix4 mat;// = to_matrix4(p_scene->CalculateWorldTransform());
	for (unsigned int i=0; i<p_scene->GetTransformCount(); i++) {
		
		FCDTransform* t;
		t = p_scene->GetTransform(i);
		if (t->GetType() == FCDTransform::MATRIX) {
			FCDTMatrix* tmat = SAFE_CAST<FCDTMatrix*>(t);
			mat = mat * to_matrix4(tmat->ToMatrix());
		} else printf("****** transformation is not matrix\n");
	};
	//	*/

	/*
	Matrix4 mat = to_matrix4(p_scene->ToMatrix());
	printf("parent count is %i\n", p_scene->GetParentCount());
	FCDSceneNode* parent = p_scene->GetParent(p_scene->GetParentCount()?p_scene->GetParentCount()-1:0);
	if (parent) {
		mat = get_global_matrix(parent) * mat;
	};
	
	return mat;
	//	*/
	
	/*
	FMMatrix44 gmat = p_scene->CalculateLocalTransform();
	FCDSceneNode* parent = p_scene->GetParent();
	while (parent) {
		
		gmat = parent->ToMatrix() * gmat;
		parent = parent->GetParent();
	};
	
	return to_matrix4(gmat);
	*/
};

struct Bone {
	
	String name;
	Matrix4 matrix;
	Matrix4 mat_global;
	int parent;
};

static FCDSkinController* find_controller(FCDSceneNode* p_scene, FCDSceneNode* p_branch) {

	for (unsigned int i=0; i<p_branch->GetInstanceCount(); i++) {

		FCDEntityInstance* instance = p_branch->GetInstance(i);
		if (instance->GetType() == FCDEntityInstance::CONTROLLER) {

			FCDControllerInstance* cont = (FCDControllerInstance*)instance;

			for (unsigned int j=0; j<cont->GetJointCount(); j++) {

				if (cont->GetJoint(j) == p_scene) {
					
					return ((FCDController*)instance->GetEntity())->GetSkinController();
				};
			};
		};
	};
	
	int c = p_branch->GetChildrenCount();
	for (int i=0; i<c; i++) {
		
		FCDSkinController* scont = find_controller(p_scene, p_branch->GetChild(i));
		if (scont) return scont;
	};
	
	return NULL;
};

static FCDSkinControllerJoint* find_joint(FCDSceneNode* p_scene, FCDSceneNode* p_branch) {

	for (unsigned int i=0; i<p_branch->GetInstanceCount(); i++) {

		FCDEntityInstance* instance = p_branch->GetInstance(i);
		if (instance->GetType() == FCDEntityInstance::CONTROLLER) {

			FCDControllerInstance* cont = (FCDControllerInstance*)instance;

			for (unsigned int j=0; j<cont->GetJointCount(); j++) {

				if (cont->GetJoint(j) == p_scene) {
					
					return ((FCDController*)instance->GetEntity())->GetSkinController()->GetJoint(j);
				};
			};
		};
	};
	
	int c = p_branch->GetChildrenCount();
	for (int i=0; i<c; i++) {
		
		FCDSkinControllerJoint* j = find_joint(p_scene, p_branch->GetChild(i));
		if (j) return j;
	};

	return NULL;
};

static FMMatrix44 find_rest(FCDSceneNode* p_scene, bool& is_joint) {
	
	FCDocument* doc = p_scene->GetDocument();
	FCDSceneNode* root = doc->GetVisualSceneInstance();
	FCDSkinControllerJoint* joint = find_joint(p_scene, root);
	is_joint = joint?true:false;
	if (joint) {
		return joint->GetBindPoseInverse().Inverted();
	} else {
		return p_scene->CalculateWorldTransform();
	};
};

static void add_childs(String parent_name, FCDControllerInstance* cont, DVector<Bone>& bones) {

	FCDEntity* ent = cont->GetEntity();
	FCDController* inst = SAFE_CAST<FCDController*>(ent);
	FCDSkinController* scont = inst->GetSkinController();
	Matrix4 shape = to_matrix4(scont->GetBindShapeTransform());
	
	int parent = bones.size()-1;
	bool dummy;
	for (unsigned int i=0; i<cont->GetJointCount(); i++) {

		FCDSceneNode* joint = cont->GetJoint(i);
		String pname = joint->GetParent()->GetName().c_str();
		if (pname == parent_name) {
			
			Bone b;
			b.name = joint->GetName().c_str();
			b.mat_global = to_matrix4(find_rest(joint, dummy));
			b.matrix = b.mat_global;
			b.parent = parent;

			if (b.parent != -1) {
				b.matrix = bones[b.parent].mat_global.inverse() * b.mat_global;
			};
			
			bones.push_back(b);
			
			add_childs(b.name, cont, bones);
		};
	};
};

static DVector<Bone> sort_bones(FCDControllerInstance* cont) {

	FCDEntity* ent = cont->GetEntity();
	FCDController* inst = SAFE_CAST<FCDController*>(ent);
	FCDSkinController* scont = inst->GetSkinController();
	Matrix4 shape; // = to_matrix4(scont->GetBindShapeTransform());
	
	DVector<Bone> ret;
	Table<String, String> joints;

	for (unsigned int i=0; i<cont->GetJointCount(); i++) {
		
		FCDSceneNode* joint = cont->GetJoint(i);
		String name = joint->GetName().c_str();
		String pname = joint->GetParent()->GetName().c_str();

		joints.set(name, pname);
	};

	bool dummy;
	for (unsigned int i=0; i<cont->GetJointCount(); i++) {
	
		FCDSceneNode* joint = cont->GetJoint(i);
		if (!joints.has(joint->GetParent()->GetName().c_str())) {

			Bone b;
			b.name = joint->GetName().c_str();
			b.mat_global = to_matrix4(find_rest(joint, dummy));
			b.matrix = to_matrix4(find_rest(joint, dummy)) * shape.inverse();
			b.parent = -1;
			
			ret.push_back(b);

			add_childs(joint->GetName().c_str(), cont, ret);
		};
	};
	
	return ret;
};

/*
static DVector<Bone> sort_bones(FCDControllerInstance* cont) {
	
	DVector<Bone> ret;
	Table<String, String> joints;
	Table<String, BoneLevel> levels;
	
	FCDEntity* ent = cont->GetEntity();
	FCDController* inst = SAFE_CAST<FCDController*>(ent);
	FCDSkinController* scont = inst->GetSkinController();
	
	for (unsigned int i=0; i<cont->GetJointCount(); i++) {
		
		FCDSceneNode* joint = cont->GetJoint(i);
		String name = joint->GetName().c_str();
		String pname = joint->GetParent()->GetName().c_str();

		joints.set(name, pname);
	};

	for (unsigned int i=0; i<cont->GetJointCount(); i++) {
		
		String name = cont->GetJoint(i)->GetName().c_str();
		String pname = name;
		int level = -1;
		while (joints.has(pname)) {
			++level;
			pname = joints[pname];
		};
		BoneLevel l;
		l.joint_id = i;
		l.level = level;
		l.name = name;
		l.parent = joints[name];
		bool b;
		//l.matrix = to_matrix4(scont->GetJoint(i)->GetBindPoseInverse().Inverted());
		l.matrix = to_matrix4(find_rest(cont->GetJoint(i), b));
		levels[name] = l;
	};
	
	Table<String, int> ordered;
	for (unsigned int i=0; i<=cont->GetJointCount(); i++) {
		
		const String* I = levels.next(NULL);
		while (I) {

			if (levels[*I].level == (int)i) {

				ordered.set(*I, ret.size());
				Bone b;
				b.name = *I;
				Matrix4 pmat;
				if (ordered.has(levels[*I].parent)) {
					b.parent = ordered[levels[*I].parent];
					if (levels.has(levels[*I].parent)) {
						pmat = levels[levels[*I].parent].matrix;
						//printf("%ls has pmat: %ls\n", b.name.c_str(), Variant(pmat).get_string().c_str());
					};
				} else {
					b.parent = -1;
				};

				//b.matrix = pmat.inverse() * levels[*I].matrix * to_matrix4(scont->GetBindShapeTransform().Inverted());
				b.matrix = pmat.inverse() * levels[*I].matrix * to_matrix4(scont->GetBindShapeTransform().Inverted());
				ret.push_back(b);
			};
			
			I = levels.next(I);
		};
	};
	
	return ret;
};
*/

static IRef<Texture> instance_texture(FCDTexture* p_tex, Table<String, IRef<Texture> >& p_tex_cache) {

	FCDImage* img = p_tex->GetImage();
	ERR_FAIL_COND_V(!img, IRef<Texture>());

	String dae_id = img->GetDaeId().c_str();
	if (p_tex_cache.has(dae_id)) {
		return p_tex_cache.get(dae_id);
	};

	
	Renderer* r = Renderer::get_singleton();

	String path = img->GetFilename().c_str();
	printf("texture path is %ls\n", path.c_str());
	String file = path;
	if (file.find_last("\\")!=-1) {
		
		file = file.right( file.find_last("\\") );
	} else if (file.find_last("/")!=-1) {
		
		file = file.right( file.find_last("/") );
	}
		
	
	IAPI_DB * db = IAPI_DB::get_singleton();
	
	if (db && db->get_ID("textures/"+file)!=OBJ_INVALID_ID) {
		/* texture is cached in database */	
		IRef<Texture> tex = IAPI_Persist::get_singleton()->load("textures/"+file);
		
		ERR_FAIL_COND_V(tex.is_null(),tex);
		return tex;		
	}

	String ext = file.substr( file.size() - 4, 4);
	if (ext.nocasecmp_to(".png") != 0) {
		ERR_PRINT("Not a png file.");
		return IRef<Texture>();
	};
	
	DirAccess *da = DirAccess::create();

	Error err = da->copy(path,"tex://"+file);
	memdelete(da);
	ERR_FAIL_COND_V(err,IRef<Texture>()); // coudln't copy to texture path	
	
	IRef<Texture> tex = r->create_texture();
	tex->load("tex://"+file);
	//IAPI_Persist::get_singleton()->save(tex,"textures/"+file);

	p_tex_cache.set(dae_id, tex);
	
	return tex;
};

static IRef<Material> instance_material(FCDMaterialInstance* matins, Table<String, IRef<Texture> >& p_tex_cache) {

	Renderer* r = Renderer::get_singleton();

	FCDMaterial* mat = matins->GetMaterial();
	FCDEffect* eff = mat->GetEffect();
	
	FCDEffectProfile* profile = eff->FindProfile(FUDaeProfileType::COMMON);
	
	if (profile==NULL) {
		return IRef<Material>();
	}

	FCDEffectStandard* prof=SAFE_CAST<FCDEffectStandard*>(profile);
	ERR_FAIL_COND_V( !prof, IRef<Material>() );
	
	IRef<Material> mat_iref = r->create_material();

	// lights
	Color c;
	c.r = (Uint8)(prof->GetAmbientColor().x * 255.0);
	c.g = (Uint8)(prof->GetAmbientColor().y * 255.0);
	c.b = (Uint8)(prof->GetAmbientColor().z * 255.0);
	c.a = (Uint8)(prof->GetAmbientColor().w * 255.0);
	mat_iref->set("color/ambient", c);

	c.r = (Uint8)(prof->GetDiffuseColor().x * 255.0);
	c.g = (Uint8)(prof->GetDiffuseColor().y * 255.0);
	c.b = (Uint8)(prof->GetDiffuseColor().z * 255.0);
	c.a = (Uint8)(prof->GetDiffuseColor().w * 255.0);
	mat_iref->set("color/diffuse", c);
	
	c.r = (Uint8)(prof->GetSpecularColor().x * 255.0);
	c.g = (Uint8)(prof->GetSpecularColor().y * 255.0);
	c.b = (Uint8)(prof->GetSpecularColor().z * 255.0);
	c.a = (Uint8)(prof->GetSpecularColor().w * 255.0);
	mat_iref->set("color/specular", c);

	if (prof->IsEmissionFactor()) {
		
		c.r = c.g = c.b = c.a = (Uint8)(prof->GetEmissionFactor() * 255.0);
	} else {
	
		float f = prof->GetEmissionFactor() * 255;
		
		c.r = (Uint8)(prof->GetEmissionColor().x * f);
		c.g = (Uint8)(prof->GetEmissionColor().y * f);
		c.b = (Uint8)(prof->GetEmissionColor().z * f);
		c.a = (Uint8)(prof->GetEmissionColor().w * f);
		
	};
	mat_iref->set("color/emission", c);

	// no flags

	// vars

	FCDEffectStandard::TransparencyMode m = prof->GetTransparencyMode();
	if (m == FCDEffectStandard::A_ONE) {

		mat_iref->set("vars/transparency", 1.0 - prof->GetTranslucencyColor().w * prof->GetTranslucencyFactor());
	} else {

		// average r,g,b ?
		float av = (prof->GetTranslucencyColor().x + prof->GetTranslucencyColor().y + prof->GetTranslucencyColor().z) / 3.0;
		mat_iref->set("vars/transparency", av * prof->GetTranslucencyFactor());
	};

	mat_iref->set("vars/shininess", prof->GetShininess());

	// textures
	if (prof->GetTextureCount(FUDaeTextureChannel::DIFFUSE)>0) {
		
		IRef<Texture> tex = instance_texture(prof->GetTexture(FUDaeTextureChannel::DIFFUSE,0), p_tex_cache);
		mat_iref->set("textures/diffuse", IObjRef(tex));
	};

	if (prof->GetTextureCount(FUDaeTextureChannel::REFLECTION)>0) {

		IRef<Texture> tex = instance_texture(prof->GetTexture(FUDaeTextureChannel::REFLECTION,0), p_tex_cache);
		mat_iref->set("textures/reflection", IObjRef(tex));
	};

	if (prof->GetTextureCount(FUDaeTextureChannel::SPECULAR)>0) {
		
		IRef<Texture> tex = instance_texture(prof->GetTexture(FUDaeTextureChannel::SPECULAR,0), p_tex_cache);
		mat_iref->set("textures/specular", IObjRef(tex));
	};

	if (prof->GetTextureCount(FUDaeTextureChannel::SPECULAR_LEVEL)>0) {
		
		IRef<Texture> tex = instance_texture(prof->GetTexture(FUDaeTextureChannel::SPECULAR_LEVEL,0), p_tex_cache);
		mat_iref->set("textures/specular", IObjRef(tex));
	};

	if (prof->GetTextureCount(FUDaeTextureChannel::BUMP)>0) {
		
		IRef<Texture> tex = instance_texture(prof->GetTexture(FUDaeTextureChannel::BUMP,0), p_tex_cache);
		mat_iref->set("textures/normalmap", IObjRef(tex));
	};

	if (prof->GetTextureCount(FUDaeTextureChannel::EMISSION)>0) {

		IRef<Texture> tex = instance_texture(prof->GetTexture(FUDaeTextureChannel::EMISSION,0), p_tex_cache);
		mat_iref->set("textures/emission", IObjRef(tex));
	};
	
	return mat_iref;
};

static int face_order[] = { 0, 1, -1 };

void instance_geometry(FCDGeometryInstance* geo, IRef<MeshNode> p_node, FCDControllerInstance* cont, Table<String, IRef<Texture> >& p_tex_cache) {
	
/*	
	FCDGeometry* egeo = SAFE_CAST<FCDGeometry*>(geo->GetEntity());
	ERR_FAIL_COND(!mesh);
	
	if (!mesh->IsTriangles()) {
		
		FCDGeometryPolygonsTools::Triangulat	e(mesh);
	};
*/	

	FCDGeometry* egeo = static_cast<FCDGeometry*>(geo->GetEntity());
	FCDGeometryMesh* mesh = egeo->GetMesh();
	ERR_FAIL_COND(!egeo->IsMesh());

	printf("SCENE: TRIANGLES %i?\n",mesh->IsTriangles());
	//if (!mesh->IsTriangles()) {


		FCDGeometryPolygonsTools::Triangulate(mesh);
	//};

	Renderer* r = Renderer::get_singleton();
	IRef<Mesh> mesh_iref = r->create_mesh();

	for (unsigned int surf_id=0; surf_id<geo->GetMaterialInstanceCount(); surf_id++) {
		
		FCDMaterialInstance* matins = geo->GetMaterialInstance(surf_id);
		FCDGeometryPolygons* pol = SAFE_CAST<FCDGeometryPolygons*>(matins->GetGeometryTarget());

		ERR_CONTINUE(!pol);
		ERR_CONTINUE(!pol->GetParent());
		//fstring material_semantic = pol->GetMaterialSemantic();
		//FCDMaterialInstance* matins = geo->FindMaterialInstance(material_semantic);
		
		/** Set up format arrays **/
		
		int format = Surface::FORMAT_ARRAY_VERTEX;
		
		FCDGeometrySource* src_vertex_data=pol->GetParent()->FindSourceByType(FUDaeGeometryInput::POSITION);
		FCDGeometrySource* src_normal_data=NULL;
		FCDGeometrySource* src_color_data=NULL;
		FCDGeometrySource* src_uv_data=NULL;
		FCDGeometrySource* src_tangent_data=NULL;
		FCDGeometrySource* src_binormal_data=NULL;
		
		if (pol->FindInput(FUDaeGeometryInput::NORMAL) && 
		    (src_normal_data=pol->GetParent()->FindSourceByType(FUDaeGeometryInput::NORMAL))) {
			format |= Surface::FORMAT_ARRAY_NORMAL;			
		};
		if (pol->FindInput(FUDaeGeometryInput::TEXCOORD) &&
		    (src_uv_data=pol->GetParent()->FindSourceByType(FUDaeGeometryInput::TEXCOORD))) {
		
			FCDGeometryPolygonsInputList inputs;
			pol->FindInputs(FUDaeGeometryInput::TEXCOORD, inputs);
		
			format |= Surface::FORMAT_ARRAY_TEX_UV_0;
		};
		if (pol->FindInput(FUDaeGeometryInput::COLOR) &&
		     (src_color_data = pol->GetParent()->FindSourceByType(FUDaeGeometryInput::COLOR))) {
			format |= Surface::FORMAT_ARRAY_COLOR;
		};
		if (pol->FindInput(FUDaeGeometryInput::TEXTANGENT) &&
			(src_tangent_data = pol->GetParent()->FindSourceByType(FUDaeGeometryInput::TEXTANGENT))) {
			format |= Surface::FORMAT_ARRAY_TANGENT;
		};
		if (pol->FindInput(FUDaeGeometryInput::TEXBINORMAL) &&
			(src_binormal_data = pol->GetParent()->FindSourceByType(FUDaeGeometryInput::TEXBINORMAL))) {
			format |= Surface::FORMAT_ARRAY_BINORMAL;
		};
		FCDGeometryPolygonsInput* input=NULL;
		
		int vertex_count = pol->FindInput(FUDaeGeometryInput::POSITION)->GetIndexCount();
		int stride = src_vertex_data->GetStride();
		ERR_FAIL_COND( stride != 3 );
				
		Vector< Vector<Uint8 > > bone_array;
		
		if (cont) { // check wether there's skeleton
			
			bone_array.resize( vertex_count );
			format|=Surface::FORMAT_ARRAY_WEIGHTS;
			DVector<Bone> bones = sort_bones(cont);
									
			Table<int, int> bone_ids;
			for (unsigned int i=0; i<cont->GetJointCount(); i++) {
				
				String name = cont->GetJoint(i)->GetName().c_str();
				for (int j=0; j<bones.size(); j++) {
					
					if (bones[j].name == name) {
						
						bone_ids[i] = j;
					};
				};
			};

			
			FCDGeometryPolygonsInput* input = pol->FindInput(FUDaeGeometryInput::POSITION);
			uint32* indices = input->GetIndices();
			
			FCDEntity* ent = cont->GetEntity();
			FCDController* inst = SAFE_CAST<FCDController*>(ent);
			FCDSkinController* scont = inst->GetSkinController();

			for (int i=0; i<vertex_count; i++) {

				
				const FCDSkinControllerVertex* pSCV = scont->GetVertexInfluence( indices[i+ face_order[i%3]] );
				Vector<Uint8 > &varr=bone_array[i];
				
				
				size_t wcount = pSCV->GetPairCount();

				Vector<int> widx;
				Vector<float> wamount;
				/* find WEIGHTS */
				for( uint32 p=0; p<wcount; p++ ) {
					
					const FCDJointWeightPair* pPair = pSCV->GetPair( p );
					// i # vertex, 
					// p # weight
					// bone_ids[pPair->jointIndex] # mat
					// (Uint8)(pPair->weight * 255) # weight
		//			sktool.add_weight(i, p, bone_ids[pPair->jointIndex], (Uint8)(pPair->weight * 255));
		
			
					widx.push_back(bone_ids[pPair->jointIndex]);
					float wg=pPair->weight;
					
					if (wg<0)
						wg=0;
					if (wg>1.0)
						wg=1.0;
					wamount.push_back(wg);
					
					
				}
				
				// sort
				
				while (true) {
				
					bool exchanges=false;
					
					for (int i=0;i<(widx.size()-1);i++) {
					
						if (wamount[i]<wamount[i+1]) {
						
							SWAP(wamount[i],wamount[i+1]);
							SWAP(widx[i],widx[i+1]);
							exchanges=true;
						}
					}
					if (!exchanges)
						break;
				}
				
				// 
				if (widx.size()==0 ) {
					WARN_PRINT("vertex without weights\n");
					widx.push_back(0);
					wamount.push_back(0);
				} else { 
					
					/* Remove Extra Weights, limit to MAX_WEIGHTS */
					while (widx.size() > Surface::MAX_WEIGHTS ) {
						
						int smallest=-1;
						float smallest_v;
						for (int wi=0;wi<widx.size();wi++) {
							
							if (wi==0 || wamount[wi]<smallest_v) {
								
								smallest=wi;
								smallest_v=wamount[wi];
							}
						}
						
						widx.remove( smallest );
						wamount.remove( smallest );
					}
					
					float wtotal=0;
					for (int wi=0;wi<widx.size();wi++) {
						
						wtotal+=wamount[wi];
					}
					
					if (wtotal<0.999) {
						

						for (int wi=0;wi<widx.size();wi++) {
						
							wamount[wi]/=wtotal;
							
						}				
					}
				}
				
				while(widx.size()<Surface::MAX_WEIGHTS) {
					
					widx.push_back(0);
					wamount.push_back(0);
				}
				
				for (int wi=0;wi<Surface::MAX_WEIGHTS;wi++) {
					
					bone_array[i].push_back(widx[wi]);
					float wgh=wamount[wi]*255.0;
					if (wgh<0)
						wgh=0;
					if (wgh>255.0)
						wgh=255.0;
					bone_array[i].push_back((Uint8)wgh);
					
				
				}
			}	
		}		
		
		
		SurfaceTool surfacet(Surface::PRIMITIVE_TRIANGLES,format);
		
		for (int i=0; i<vertex_count; i++) {
		
			/* Add Normal */
			
			if (src_normal_data) {
				
				uint32* indices = pol->FindInput(FUDaeGeometryInput::NORMAL)->GetIndices();

				Vector3 norm;
				float* p=NULL;
				int stride = src_normal_data->GetStride();
				ERR_FAIL_COND( stride < 3 );			
				p = &src_normal_data->GetData()[indices[i+ face_order[i%3]]*stride];
				norm.x = p[0];
				norm.y = p[1];
				norm.z = p[2];
				norm = transform_vector3(norm);
				norm.normalize();	
				surfacet.add_normal(norm);
			}
			
			/* Add Color */
			
			if (src_color_data) {
				
				uint32* indices = pol->FindInput(FUDaeGeometryInput::COLOR)->GetIndices();
				float* p=NULL;
				
				int color_stride = src_color_data->GetStride();
				ERR_FAIL_COND( color_stride< 3 );
				p = &src_color_data->GetData()[indices[i+ face_order[i%3]] * color_stride];
				Uint32 col;
				col = (Uint32)(p[0] * 255.0);
				col |= ((Uint32)(p[1] * 255.0)) << 8;
				col |= ((Uint32)(p[2] * 255.0)) << 16;
				if (color_stride >= 4) {
					col |= ((Uint32)(p[3] * 255.0)) << 24;
				};
				
				surfacet.add_color( col );
			}
			
			/* Add UV */
			if ( src_uv_data ) {
				
				uint32* indices = pol->FindInput(FUDaeGeometryInput::TEXCOORD)->GetIndices();
				float* p=NULL;
				
				int stride = src_uv_data->GetStride();
				ERR_FAIL_COND( stride < 2 );				
				p = &src_uv_data->GetData()[indices[i+ face_order[i%3]] * stride];
					
				surfacet.add_uv(0, Vector3( p[0], 1 - p[1], 0 ) );
			}

			/* Add binormal */
			
			#if 0
			if ( src_binormal_data ) {
				
				uint32* indices = pol->FindInput(FUDaeGeometryInput::TEXBINORMAL)->GetIndices();

				Vector3 binorm;
				float* p=NULL;
				int stride = src_normal_data->GetStride();
				ERR_FAIL_COND( stride < 3 );			
				p = &src_binormal_data->GetData()[indices[i+ face_order[i%3]]*stride];
				binorm.x = p[0];
				binorm.y = p[1];
				binorm.z = p[2];
				binorm = transform_vector3(binorm);
				//norm.normalize();	
				surfacet.add_binormal(binorm);
			};

			/* Add tangent */
			if ( src_tangent_data ) {
				
				uint32* indices = pol->FindInput(FUDaeGeometryInput::TEXTANGENT)->GetIndices();

				Vector3 tan;
				float* p=NULL;
				int stride = src_normal_data->GetStride();
				ERR_FAIL_COND( stride < 3 );			
				p = &src_binormal_data->GetData()[indices[i+ face_order[i%3]]*stride];
				tan.x = p[0];
				tan.y = p[1];
				tan.z = p[2];
				tan = transform_vector3(tan);
				//norm.normalize();	
				surfacet.add_binormal(tan);
			};
			#endif
			if (bone_array.size())  {
				

				if (bone_array[i].size()==Surface::MAX_WEIGHT_BYTES) {
							
					
					surfacet.add_weight( &bone_array[i][0] );
				}
			}
			
			/* Finally add vertex */
			uint32* indices = pol->FindInput(FUDaeGeometryInput::POSITION)->GetIndices();
			
			Vector3 vert;
			float* p=NULL;
			p = &src_vertex_data->GetData()[indices[i + face_order[i%3]]*stride];
			vert.x = p[0];
			vert.y = p[1];
			vert.z = p[2];
			vert = transform_vector3(vert);
			
			surfacet.add_vertex(vert);
			
	
			
		}
		
		/* make material */
		IRef<Material> mat = instance_material(matins, p_tex_cache);

		/* make surface */
		
		surfacet.index();
		
		IRef<Surface> surf=surfacet.commit();
		ERR_FAIL_COND(surf.is_null());
		
		/* set material */
		surf->set_material(mat);
					
		/* add to mesh */
		mesh_iref->add_surface(surf);
		
		
	};
	p_node->set_mesh(mesh_iref);
};

static void add_skeleton(IRef<SkeletonNode> skel, FCDSceneNode* p_scene, FCDEntityInstance* instance) {

	FCDControllerInstance* cont = (FCDControllerInstance*)instance;
	FCDEntity* ent = cont->GetEntity();

	skel->set_bone_count(cont->GetJointCount());
	Table<String, int> joints;
	
	ERR_FAIL_COND(ent->GetType() != FCDEntity::CONTROLLER);

	DVector<Bone> bones = sort_bones(cont);
	for (int i=0; i<bones.size(); i++) {
		
		skel->add_bone(i, bones[i].name, bones[i].matrix, bones[i].parent);
	};
};

static void add_scene(FCDSceneNode* p_scene, IRef<Node> parent, IRef<Node> existing, IRef<SceneNode> root, Table<String, IRef<Mesh> >& p_cache, Table<String, IRef<Texture> >& p_tex_cache) {

	for (unsigned int i=0; i<p_scene->GetInstanceCount(); i++) {

		FCDEntityInstance* instance = p_scene->GetInstance(i);
		FCDControllerInstance* cont = (FCDControllerInstance*)instance;

		
		switch (instance->GetType()) {
			
			case FCDEntityInstance::CONTROLLER: {
	
				
				IRef<SkeletonNode> skel;
				if (!existing) {
					skel = memnew(SkeletonNode);
				} else {
					ERR_FAIL_COND( !existing->is_type("SkeletonNode") );
					skel = existing;
				};
	
				if (parent) {
					parent->add_child(skel);
					skel->set_owner(root);
				};
				
				skel->set_name(p_scene->GetName().c_str());
	
				skel->set_global_matrix(get_global_matrix(p_scene));
				add_skeleton(skel, p_scene, instance);
				//skel->set_matrix(to_matrix4(p_scene->ToMatrix()));
											
				FCDEntity* ent = cont->GetEntity();
				FCDController* inst = SAFE_CAST<FCDController*>(ent);
				FCDSkinController* scont = inst->GetSkinController();
				
				IRef<MeshNode> mnode = memnew(MeshNode);
				mnode->set_name(p_scene->GetName().c_str());
				mnode->set_use_skeleton_node(true);
				skel->add_child(mnode);
				mnode->set_owner(root);
				
				// load the mesh
				
				String dae_id = cont->GetEntity()->GetDaeId().c_str();
				if (p_cache.has(dae_id)) {
					
					mnode->set_mesh(p_cache.get(dae_id));
				} else {
					
					instance_geometry(cont, mnode, cont, p_tex_cache);
					if (mnode->get_mesh()) {
						p_cache.set(dae_id, mnode->get_mesh());
					};
				};
				
				/*
				Matrix4 mat;
				mat.rotate_y(Math_PI/2.0);
				skel->set_bone_pose(7, mat);
				skel->set_bone_pose(6, mat);
				skel->process();
				*/
	
				parent = mnode;
				
			} break;
				
			case FCDEntityInstance::GEOMETRY: {
				
				
				
				FCDGeometryInstance* geo = static_cast<FCDGeometryInstance*>(instance);
				
				IRef<MeshNode> mnode;
				if (!existing) {
					mnode = memnew(MeshNode);
				} else {
					ERR_FAIL_COND( !existing->is_type("MeshNode") );
					mnode = existing;
				};
				if (parent) {
					parent->add_child(mnode);
				};
				mnode->set_name(p_scene->GetName().c_str());
				mnode->set_owner(root);
				
				mnode->set_global_matrix(get_global_matrix(p_scene));
				//mnode->set_matrix(to_matrix4(p_scene->ToMatrix()));
	
				
				
				String dae_id = geo->GetEntity()->GetDaeId().c_str();
				if (p_cache.has(dae_id)) {
					
					mnode->set_mesh(p_cache.get(dae_id));
				} else {
					
					instance_geometry(geo, mnode, NULL, p_tex_cache);
					if (mnode->get_mesh()) {
						p_cache.set(dae_id, mnode->get_mesh());
					};
				};
				
				parent = mnode;
				
			} break;
			
			default: {
			
			};break;
	
		};
	};
	
	/*
	if (p_scene->GetInstanceCount() == 0) {
	
		Node* dnode = existing;
		if (!dnode) {
			dnode = memnew(DummyNode);
		};
		
		if (parent)
			parent->add_child(dnode);
		
		//dnode->set_matrix(to_matrix4(p_scene->ToMatrix()));
		dnode->set_owner(root);
		dnode->set_name(p_scene->GetName().c_str());
		parent = dnode;
	};
	//	*/
	
	
	for (unsigned int i=0; i<p_scene->GetChildrenCount(); i++) {
		
		add_scene(p_scene->GetChild(i), parent, IRef<Node>(), root, p_cache, p_tex_cache);
	};
};

IRef<SceneNode> Collada::import_scene(String p_fname, IRef<SceneNode> p_root) {

	if (!is_init) {
		FCollada::Initialize();
		is_init = true;
	};

	if (!p_root)
		p_root = memnew( SceneNode );
	
	ERR_FAIL_COND_V(p_root.is_null(),p_root);
	
	FUErrorSimpleHandler errorHandler;
	FUObjectRef<FCDocument> document = FCollada::NewTopDocument();
	bool load_success=FCollada::LoadDocumentFromFile(document, FC(p_fname.ascii().get_data()));
	if (!load_success) {
	
		ERR_PRINT(errorHandler.GetErrorString());
		ERR_FAIL_COND_V(!load_success, IRef<SceneNode>());

	}
	//ERR_FAIL_COND_V(!errorHandler.IsSuccessful(), IRef<SceneNode>());

	FCDocumentTools::StandardizeUpAxisAndLength(document, FMVector3(0, 1, 0), 1);
	
	FCDSceneNode* root = document->GetVisualSceneInstance();

	p_root->set_name(root->GetName().c_str());

	
	Matrix4 global = get_global_matrix(root);
	
	p_root->set_global_matrix(global);

	Table<String, IRef<Mesh> > geometry_cache;
	Table<String, IRef<Texture> > texture_cache;
	
	for (unsigned int i=0; i<root->GetChildrenCount(); i++) {
		
		add_scene(root->GetChild(i), p_root, IRef<Node>(), p_root, geometry_cache, texture_cache);
	};
	
	
	return p_root;
};


static float add_track(IRef<Animation> p_anim, FCDSceneNode* p_scene,String p_base /*, IRef<SkeletonNode> p_skel*/) {
	
	ERR_FAIL_COND_V(p_anim->has_track(p_scene->GetName().c_str()), -1);
	printf("adding track %s\n",p_base.ascii().get_data());
	bool joint;	
	FMMatrix44 inv = FMMatrix44::Identity;
	//*
	inv = find_rest(p_scene, joint);

	//*
	FCDSceneNode* root = p_scene->GetDocument()->GetVisualSceneInstance();
	FCDSkinController* scont = find_controller(p_scene, root);
	FMMatrix44 shape = FMMatrix44::Identity;
	if (scont) {
		shape = scont->GetBindShapeTransform();
		//inv = inv * shape;
	};
	
	//*
	if (p_scene->GetParent()) {
		bool pjoint;
		inv = find_rest(p_scene->GetParent(), pjoint).Inverted() * inv;
		printf("has rest:\n %s\n",Variant(to_matrix4(inv)).get_string().ascii().get_data());
		//if (! p_scene->TestFlag( FCDSceneNode::FLAG_Joint))
		//	inv=FMMatrix44::Identity;
	};
	//	*/

/*
	Matrix4 skel_rest;
	int bid = p_skel->get_bone_id(p_scene->GetName().c_str());
	if (bid != -1) {
		skel_rest = p_skel->get_bone_rest(p_skel->get_bone_id(p_scene->GetName().c_str()));
		printf("-------\n");
		printf("skel_rest is %ls\n", Variant(skel_rest).get_string().c_str());
		printf("scene inv is %ls\n", Variant(to_matrix4(inv)).get_string().c_str());
		printf("rest_shap is %ls\n", Variant(to_matrix4(inv * shape.Inverted())).get_string().c_str());
	};
*/	
	FCDSceneNodeTools::GenerateSampledAnimation(p_scene);
	
	FloatList keylist = FCDSceneNodeTools::GetSampledAnimationKeys();
	FMMatrix44List matlist = FCDSceneNodeTools::GetSampledAnimationMatrices();

	printf("matlist %i, keylist %i\n",keylist.size(),matlist.size());
	
	if (keylist.size() == 0) {
		

		if (joint) {
			
			
			keylist.push_back(0);
			printf("has %i transforms\n",p_scene->GetTransformCount());
			if (p_scene->GetTransformCount()) {
			
				matlist.push_back(  p_scene->GetTransform(0)->ToMatrix());
				printf("transform local %s\n",Variant(to_matrix4(p_scene->GetTransform(0)->ToMatrix())).get_string().ascii().get_data());

			} else {
			
				matlist.push_back( inv ); /// get rid of trck :(
			}
		} else {
		
			return -1;
		};
	};
	String name = p_scene->GetName().c_str();
	int track = p_anim->add_track(p_base);
	
	
	for (unsigned int i=0; i<keylist.size(); i++) {

		FMMatrix44 t;
		t = inv.Inverted() * matlist[i];

		Matrix4 fmat = to_matrix4(t);

		//fmat = skel_rest.inverse() * to_matrix4(matlist[i]);
		p_anim->add_location_key(track,keylist[i],fmat.get_translation());
		p_anim->add_rotation_key(track,keylist[i],Quat(fmat));
	};
	
	return keylist[keylist.size()-1];
};

static float add_scene_tracks(IRef<Animation> p_anim, FCDSceneNode* p_scene,String p_base,bool root) {
	
	float time=0;
	
	
	String scene_name=p_scene->GetName().c_str();
	
	if (!root) 
		p_base=p_base!=""?(p_base+"/"+scene_name):scene_name;
	
	bool is_bone=p_scene->TestFlag( FCDSceneNode::FLAG_Joint);
	
	
	/* Determine wether this instance is a skeleton/bone */
	for (unsigned int i=0; i<p_scene->GetInstanceCount(); i++) {

		FCDEntityInstance* instance = p_scene->GetInstance(i);
		if (instance->GetType() == FCDEntityInstance::CONTROLLER) {
		
			FCDControllerInstance* cont = (FCDControllerInstance*)instance;
			
			for (unsigned int j=0;j<cont->GetJointCount();j++) {
						
				FCDSceneNode *joint = cont->GetJoint(j);
				
				float ttime = add_track(p_anim, joint,p_base+":"+String(joint->GetName().c_str()));
				if (ttime>time)
					time=ttime;
					
				
			}
			break;
			
		}
		//if (instance->GetType() == FCDEntityInstance::CONTROLLER) {
		
	}
	
	
	if (!is_bone && p_scene->GetTransformCount() > 0) {
		
		float ttime = add_track(p_anim, p_scene,p_base);
		if (ttime > time) time = ttime;
	};
	
		
	for (unsigned int i=0; i<p_scene->GetChildrenCount(); i++) {
		
		float ttime = add_scene_tracks(p_anim, p_scene->GetChild(i),p_base,false);
		if (ttime > time) time = ttime;
	};
	
	return time;
};

IRef<Animation> Collada::import_animation(String p_fname) {

	if (!is_init) {
		FCollada::Initialize();
		is_init = true;
	};

	FUErrorSimpleHandler errorHandler;
	FUObjectRef<FCDocument> document = FCollada::NewTopDocument();
	bool load_success=FCollada::LoadDocumentFromFile(document, FC(p_fname.ascii().get_data()));
	if (!load_success) {
	
		ERR_PRINT(errorHandler.GetErrorString());
		ERR_FAIL_COND_V(!load_success, IRef<Animation>());

	}
	
	FCDocumentTools::StandardizeUpAxisAndLength(document, FMVector3(0, 1, 0), 1);
	
	FCDSceneNode* root = document->GetVisualSceneInstance();
	
	ERR_FAIL_COND_V(!root, IRef<Animation>());
	
	IRef<Animation> anim;
	anim.create();
	
	float time = add_scene_tracks(anim, root,"",true);


	anim->set_length(time);
	
	return anim;
};

#endif

