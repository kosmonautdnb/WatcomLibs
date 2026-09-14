#ifndef __OCTREE_HPP__
#define __OCTREE_HPP__

// here you need -D__GLSOMEDEPTHPASSED__=1 as compiler switch with glimpl.cpp to enable Occlusion Query/Culling in WatcomGL
// many many polys on the more or less exact place can be problematic

#include "ARRAY.HPP"
#include "VECTOR.HPP"

// Octree structures
// -----------
class OctreeEntity {
public:
  OctreeEntity() : faceColor(0), texHandle(0), paintFrame(0) {memset(pos, 0, sizeof(pos)); memset(tex, 0, sizeof(tex));}
  unsigned int pos[3];
  unsigned int tex[3];
  unsigned int faceColor;
  unsigned int texHandle;
  unsigned char paintFrame; // counter to tell if the entity was already drawn this frame
};

class OctreeData {
public:
  OctreeData() : entityCount(0), entityIds(NULL), dynamicEntityCount(0), dynamicEntityIds(NULL) {;}
  void freeIt() {if (entityIds != NULL) {delete[] entityIds; entityIds = NULL;} entityCount = 0; if (dynamicEntityIds != NULL) {delete[] dynamicEntityIds; dynamicEntityIds = NULL;} dynamicEntityCount = 0;}
  int entityCount;
  unsigned int *entityIds;
  int dynamicEntityCount;
  unsigned int *dynamicEntityIds;
};

class OctreeNode {
public:
  OctreeNode() : data(0), color(0), bbBoxMinXYZ10(0), bbBoxMaxXYZ10(0) {memset(childs, 0, sizeof(childs));}
  unsigned int childs[8];
  unsigned int data;
  unsigned short color;
  unsigned int bbBoxMinXYZ10; // 10 bit (0..1023) x,y,z interpolation of the bounding box for enclosing the content
  unsigned int bbBoxMaxXYZ10;
};

class OctreeBase {
public:
  OctreeBase() : minPolysPerNode(32), minChildSize(0.001,0.001,0.001), root(NULL), octreeEntities2(NULL), octreeDatas2(NULL), octreeNodes2(NULL), octreePos2(NULL), octreeTex2(NULL), paintFrame(0) {;}
  int minPolysPerNode; // if a child would have less polys the parent is taken instead and no child created
  Vector minChildSize; // the childs get not smaller than this, (it's important to be bigger 0)
  Vector bbBoxMin; // min of octrees axis aligned bounding box (AABB)
  Vector bbBoxMax; // max of  octrees axis aligned bounding box (AABB)
  OctreeNode *root; // the root node of the octree (using the complete AABB)
  Array<OctreeEntity> octreeEntities;
  Array<OctreeData> octreeDatas;
  Array<OctreeNode> octreeNodes;
  Array<Vector> octreePos; // positions used
  Array<Vector> octreeTex; // texture coordinates used
  OctreeEntity *octreeEntities2; // direct pointer for faster access (WatcomC)
  OctreeData *octreeDatas2; // direct pointer for faster access (WatcomC)
  OctreeNode *octreeNodes2; // direct pointer for faster access (WatcomC)
  Vector *octreePos2; // direct pointer for faster access (WatcomC)
  Vector *octreeTex2; // direct pointer for faster access (WatcomC)
  unsigned char paintFrame; // counter to tell if the entity was already drawn this frame

  void add_OBJ_Entities(class SMPL_File *obj); // add one or more WaveFront .obj files (to be done before generating the octree)
  void generate(); // generates the octree 

  void generateOctreeChildBB(int child, const Vector &parentMin, const Vector &parentMax, Vector &destMin, Vector &destMax); // helper
  // private, creates a new node(id) with all these entities and creates it's fine granular bounding box and generates an average color of the node
  unsigned int buildOctreeNodeFromEntityIds(const Array<unsigned int> &entityIds, const Vector &bbMin, const Vector &bbMax);
  // private, used to free the octreeData associated with this node, if the childs contain now the data
  void eraseOctreeData(unsigned int index);
  // private, generates the 8 childs if they contain anything of the parent and clears the parent
  void generateOctreeChilds(unsigned int parent, const Vector &parentMin, const Vector &parentMax);
};

// Frustum for painting
// -----------
class OctreeFrustum {
public:
  Vector planePoint[6];
  Vector planeNormal[6];
  Vector center;
  Vector points[8];
  double minmax[3*2]; // maybe used later again

  OctreeFrustum();
  void update(); // to be called before usage after points where updated
  void frustumFromOpenGL(double nearPlane, double farPlane); // call this to create a frustum from the current opengl settings (Matrices)

  bool isPointInside(const Vector &point);
  bool isCubeFullyOutside(const Vector &minp, const Vector &maxp);
  double nearPlaneSide(const Vector &point);
  bool isCubeSomeBehindNearPlane(const Vector &minp, const Vector &maxp);
  bool isCubeSomeBehindFarPlane(const Vector &minp, const Vector &maxp);
  void cubeNearest(const Vector &minp, const Vector &maxp, double &nearest);
};

// painting the visible nodes of the octree
void paintOctree(OctreeFrustum *frustum, OctreeBase *base, int maxPolysBeforeCubesArePainted);

#endif // __OCTREE_HPP__
