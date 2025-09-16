#ifndef __CGLTFA_HPP__
#define __CGLTFA_HPP__

#include "cgltf.hpp" // needs #define CGLTF_IMPLEMENTATION
#include "array.hpp"
#include "hashmap.hpp"
#include "vector.hpp"
#include "matrix.hpp"
#include "quaternn.hpp"
#include "string.hpp"

// this is a tiny fast coded cgltf mesh loader and displayer, including skinning and animations

class GLTFA_Material {
public:
  String name;
  unsigned int materialId;
  unsigned int texWidth;
  unsigned int texHeight;
  unsigned int texHandle;
};

enum GLTFA_Primitive_type {
  GLTFA_Primitive_type_unsupported = 0,
  GLTFA_Primitive_type_triangle = 1,
};

class GLTFA_Primitive {
public:
  GLTFA_Primitive_type primitive_type;
  unsigned int materialId;
  Array<int> indices;
  HashMap<int,Array<Vector> > positions;
  HashMap<int,Array<Vector> > colors;
  HashMap<int,Array<Vector> > normals;
  HashMap<int,Array<Vector> > texCoords;
  HashMap<int,Array<Vector> > joints; // actually ints
  HashMap<int,Array<Vector> > weights;
};

class GLTFA_Mesh {
public:
  unsigned int meshId;
  String name;
  Array<GLTFA_Primitive*> primitives;
  Vector boundingBoxMin;
  Vector boundingBoxMax;
  void buildBoundingBox();
};

class GLTFA_Node {
public:
  GLTFA_Node();
  unsigned int id;
  unsigned int parent_id;
  int matrixUpdateId;
  unsigned int meshId;
  unsigned int skinId;
  bool hasScaling;
  bool hasRotation;
  bool hasTranslation;
  bool hasMatrix;
  Array<unsigned int> child_ids;
  Vector scaling;
  Quaternion rotation;
  Vector translation;
  Matrix matrix;
  Matrix matrixHereWithParent;
  Matrix inverseBindMatrix;
  Matrix finalMatrix;
  Matrix finalNormalMatrix;
  Vector color;
};

class GLTFA_Skin {
public:
  Array<GLTFA_Node*> joints;
  unsigned int id;

  __inline Vector transformPosition(const Vector &p, const Vector &joints4, const Vector &weights4) const {
    Vector r,k; double w;const GLTFA_Node *l;
    w = weights4.x; if (w!=0.0) {
      l = joints[(int)joints4.x];
      k = l->finalMatrix * p;
      r.x += k.x*w;
      r.y += k.y*w;
      r.z += k.z*w;
    }
    w = weights4.y; if (w!=0.0) {
      l = joints[(int)joints4.y];
      k = l->finalMatrix * p;
      r.x += k.x*w;
      r.y += k.y*w;
      r.z += k.z*w;
    }
    w = weights4.z; if (w!=0.0) {  
      l = joints[(int)joints4.z];
      k = l->finalMatrix * p;
      r.x += k.x*w;
      r.y += k.y*w;
      r.z += k.z*w;
    }
    w = weights4.w; if (w!=0.0) {
      l = joints[(int)joints4.w];
      k = l->finalMatrix * p;
      r.x += k.x*w;
      r.y += k.y*w;
      r.z += k.z*w;
    }
    r.w = 1;
    return r;
  }
  
  __inline Vector transformNormal(const Vector &n, const Vector &joints4, const Vector &weights4) const {
    Vector r,k; double w;const GLTFA_Node *l;
    w = weights4.x; if (w!=0.0) {
      l = joints[(int)joints4.x];
      k = l->finalNormalMatrix * n;
      r.x += k.x*w;
      r.y += k.y*w;
      r.z += k.z*w;
    }
    w = weights4.y; if (w!=0.0) {
      l = joints[(int)joints4.y];
      k = l->finalNormalMatrix * n;
      r.x += k.x*w;
      r.y += k.y*w;
      r.z += k.z*w;
    }
    w = weights4.z; if (w!=0.0) {  
      l = joints[(int)joints4.z];
      k = l->finalNormalMatrix * n;
      r.x += k.x*w;
      r.y += k.y*w;
      r.z += k.z*w;
    }
    w = weights4.w; if (w!=0.0) {
      l = joints[(int)joints4.w];
      k = l->finalNormalMatrix * n;
      r.x += k.x*w;
      r.y += k.y*w;
      r.z += k.z*w;
    }
    return r;
  }
};

enum GLTFA_Animation_type {
   GLTFA_Animation_type_scaling = 0,
   GLTFA_Animation_type_rotation = 1,
   GLTFA_Animation_type_translation = 2,
};

enum GLTFA_Interpolation_type {
   GLTFA_Interpolation_type_linear = 0,
   GLTFA_Interpolation_type_step = 1,
   GLTFA_Interpolation_type_cubic = 2,
};

class GLTFA_AnimationChannel {
public:
  Array<float> times;
  Array<Vector> data;
  GLTFA_Animation_type type;
  GLTFA_Interpolation_type interpolation;  
  float timeMin;
  float timeMax;
  unsigned int node_id;
};

class GLTFA_Animation {
public:
  Array<GLTFA_AnimationChannel> channels;
  void init(cgltf_animation *v);
};


class GLTFA_Scene {
public:
  Array<GLTFA_Node *> nodes;
  unsigned int id;
};

typedef bool (*HITTESTFUNCTION)(const Matrix &cubeMatrix, Vector &hitPoint, double &hitDistance, const GLTFA_Node &node);
class GLTFA_File {
public:

  HashMap<unsigned int, GLTFA_Material *> gltfMaterials;
  HashMap<unsigned int, GLTFA_Mesh *> gltfMeshes;
  HashMap<unsigned int, GLTFA_Node *> gltfNodes;
  HashMap<unsigned int, GLTFA_Skin *> gltfSkins;
  HashMap<unsigned int, GLTFA_Scene *> gltfScenes;
  Array<GLTFA_Animation*> gltfAnimations;
  unsigned int defaultScene;

  bool load(const char *fileName, const float textureScale); // load .glt or .glb file (glb also features embedded textures etc..)
  void free();
  unsigned int loadTexture(cgltf_image *v, double texSizeMul);
  void initMaterials(cgltf_material* materials, int count, double texSizeMul);
  void initMesh(GLTFA_Mesh *m, cgltf_mesh *v);
  void initMeshes(cgltf_mesh* meshes, int count);
  void initNodes(cgltf_node *nodes, int count);
  void initSkins(cgltf_skin *skins, int count);
  void initScenes(cgltf_scene *scenes, int count);
  void initAnimations(cgltf_animation *animations, int count);
  Matrix getMatrixHere(GLTFA_Node *j) const;
  Matrix getMatrix(unsigned int nodeId, unsigned int updateCycle);
  void fillNode(GLTFA_Node *j, cgltf_node *k) const;
  void recalculateNodes();
  void applyAnimation(float time, GLTFA_Animation *a);
  void drawPrimitive(GLTFA_Primitive *a, GLTFA_Skin *b = NULL, const Vector &color = Vector(1,1,1,1));
  void drawMesh(GLTFA_Mesh *a);
  void drawMesh(GLTFA_Mesh *a, GLTFA_Skin *b);
  void drawMesh(unsigned int meshId, double *matrix, unsigned int skinId, const Vector &color = Vector(1,1,1,1));
  void drawNode(unsigned int nodeId);
  void drawScene(unsigned int sceneId);
  unsigned int hitTestNode(unsigned int nodeId, HITTESTFUNCTION t, Vector &nodeHitPoint, double &lastHitDistance);
  unsigned int hitTestForScene(unsigned int sceneId, HITTESTFUNCTION t, Vector &hitPoint);
};

/* Basic Hit Test
Vector l0,l1,norm;
bool hitTest(const Matrix &cubeMatrix, Vector &hitPoint, double &hitDistance, const GLTFA_Node &node) {
  bool hit = CLCube::collided(cubeMatrix,l0,l1,hitPoint,false);
  hitDistance = dot(hitPoint-l0,norm);
  return hit;
}
void hitTestScene(GLTFA_File &f, int sceneId) {
  Array<unsigned int> k = f.gltfNodes.keys();
  for (int j = 0; j < k.size(); j++) {
    f.gltfNodes[k[j]]->color = Vector(1,1,1,1);
  }
  int viewPort[4];
  glGetIntegerv(GL_VIEWPORT, viewPort);
  double modelView[16], projection[16];
  glGetDoublev(GL_MODELVIEW_MATRIX, modelView);
  glGetDoublev(GL_PROJECTION_MATRIX, projection);
  gluUnProject(mouseX,glFrameBufferHeight-mouseY,0,modelView,projection,viewPort,&l0.x,&l0.y,&l0.z);
  gluUnProject(mouseX,glFrameBufferHeight-mouseY,1,modelView,projection,viewPort,&l1.x,&l1.y,&l1.z);
  norm = normalize(l1-l0);
  Vector hitPoint;
  unsigned int nodeId = f.hitTestForScene(sceneId,hitTest,hitPoint);
  if (nodeId != 0) {
    if (f.gltfNodes.has(nodeId)) {
      GLTFA_Node *j = f.gltfNodes[nodeId];
      j->color = Vector(1,0,0,1);
    }
  }
}
*/

#endif // __CGLTFA_HPP__
