#include "OCTREE.HPP"
#include "SMPLOBJL.HPP"
#include "GL.H"
#include "MATRIX.HPP"

// OCTREE GENERATION
//---------------
void OctreeBase::add_OBJ_Entities(SMPL_File *obj) {
  int posStart = (int)octreePos.size();
  int texStart = (int)octreeTex.size();
  int entityStart = (int)octreeEntities.size();
  octreePos.resize(posStart+obj->vertices.size());
  octreeTex.resize(texStart+obj->texCoords.size());
  octreeEntities.resize(entityStart + obj->faces.size());
  {for (int i = 0; i < (int)obj->vertices.size(); i++) octreePos[posStart+i] = obj->vertices[i];}
  {for (int i = 0; i < (int)obj->texCoords.size(); i++) octreeTex[texStart+i] = obj->texCoords[i];}
  for (int i = 0; i < (int)obj->faces.size(); i++) {
    SMPL_Face &f = obj->faces[i];
    int m = f.m; SMPL_Material *m_ = m >= 0 && m < (int)obj->materialsById.size() ? obj->materialsById[m] : NULL;
    OctreeEntity &e = octreeEntities[i + entityStart];
    e.texHandle = m_ != NULL ? m_->mapDiffuse.glHandle : 0;
    for (int j = 0; j < 3; j++) {
      if (j == 0) e.faceColor = obj->colors[f.v[j]];
      e.pos[j] = f.v[j] + posStart;
      e.tex[j] = f.t[j] + texStart;
    }
  }
}

void OctreeBase::generateOctreeChildBB(int child, const Vector &parentMin, const Vector &parentMax, Vector &destMin, Vector &destMax) {
  Vector center = (parentMin + parentMax) * 0.5; // unweighted octree
  Vector boxMin = parentMin, boxMax = parentMax;
  switch(child) {
    case 0: {boxMax = center;} break;
    case 1: {boxMin.x = center.x; boxMax.y = center.y; boxMax.z = center.z;} break;
    case 2: {boxMin.y = center.y; boxMax.x = center.x; boxMax.z = center.z;} break;
    case 3: {boxMin.z = center.z; boxMax.x = center.x; boxMax.y = center.y;} break;
    case 4: {boxMin = center;} break;
    case 5: {boxMax.x = center.x; boxMin.y = center.y; boxMin.z = center.z;} break;
    case 6: {boxMax.y = center.y; boxMin.x = center.x; boxMin.z = center.z;} break;
    case 7: {boxMax.z = center.z; boxMin.x = center.x; boxMin.y = center.y;} break;
  }
  destMin = boxMin; destMax = boxMax;
}

unsigned int OctreeBase::buildOctreeNodeFromEntityIds(const Array<unsigned int> &entityIds, const Vector &bbMin, const Vector &bbMax) {
  unsigned int nodeIndex = octreeNodes.size(); octreeNodes.resize(nodeIndex + 1); OctreeNode *node = &octreeNodes[nodeIndex];
  if (!entityIds.empty()) {
    unsigned int dataIndex = octreeDatas.size(); octreeDatas.resize(dataIndex + 1); OctreeData *data = &octreeDatas[dataIndex];
    node->data = dataIndex;
    data->entityCount = entityIds.size(); data->entityIds = new unsigned int[entityIds.size()];
    double r=0, g=0, b=0; bool first = true; Vector minBox, maxBox;
    for (int i = 0; i < (int)entityIds.size(); i++) {
      data->entityIds[i] = entityIds[i];
      OctreeEntity &e = octreeEntities[entityIds[i]];
      r += e.faceColor & 255; g += (e.faceColor>>8) & 255; b += (e.faceColor>>16) & 255;
      for (int j = 0; j < 3; j++) {
        Vector &k = octreePos[e.pos[j]];
        if (first || k.x < minBox.x) minBox.x = k.x;
        if (first || k.y < minBox.y) minBox.y = k.y;
        if (first || k.z < minBox.z) minBox.z = k.z;
        if (first || k.x > maxBox.x) maxBox.x = k.x;
        if (first || k.y > maxBox.y) maxBox.y = k.y;
        if (first || k.z > maxBox.z) maxBox.z = k.z;
        first = false;
      }
    }
    if (minBox.x < bbMin.x) minBox.x = bbMin.x;
    if (minBox.y < bbMin.y) minBox.y = bbMin.y;
    if (minBox.z < bbMin.z) minBox.z = bbMin.z;
    if (maxBox.x < bbMin.x) maxBox.x = bbMin.x;
    if (maxBox.y < bbMin.y) maxBox.y = bbMin.y;
    if (maxBox.z < bbMin.z) maxBox.z = bbMin.z;
    if (minBox.x > bbMax.x) minBox.x = bbMax.x;
    if (minBox.y > bbMax.y) minBox.y = bbMax.y;
    if (minBox.z > bbMax.z) minBox.z = bbMax.z;
    if (maxBox.x > bbMax.x) maxBox.x = bbMax.x;
    if (maxBox.y > bbMax.y) maxBox.y = bbMax.y;
    if (maxBox.z > bbMax.z) maxBox.z = bbMax.z;
    unsigned int minx = (bbMax.x - bbMin.x) != 0 ? (minBox.x - bbMin.x) * 1023 / (bbMax.x - bbMin.x) : 0;
    unsigned int miny = (bbMax.y - bbMin.y) != 0 ? (minBox.y - bbMin.y) * 1023 / (bbMax.y - bbMin.y) : 0;
    unsigned int minz = (bbMax.z - bbMin.z) != 0 ? (minBox.z - bbMin.z) * 1023 / (bbMax.z - bbMin.z) : 0;
    unsigned int maxx = (bbMax.x - bbMin.x) != 0 ? (maxBox.x - bbMin.x) * 1023 / (bbMax.x - bbMin.x) + 1 : 0;
    unsigned int maxy = (bbMax.y - bbMin.y) != 0 ? (maxBox.y - bbMin.y) * 1023 / (bbMax.y - bbMin.y) + 1 : 0;
    unsigned int maxz = (bbMax.z - bbMin.z) != 0 ? (maxBox.z - bbMin.z) * 1023 / (bbMax.z - bbMin.z) + 1 : 0;
    if (maxx > 1023) maxx = 1023; 
    if (maxy > 1023) maxy = 1023; 
    if (maxz > 1023) maxz = 1023;
    node->bbBoxMinXYZ10 = minx | (miny << 10) | (minz << 20); node->bbBoxMaxXYZ10 = maxx | (maxy << 10) | (maxz << 20);
    r /= entityIds.size(); g /= entityIds.size(); b /= entityIds.size();
    int ri = (int)(r*31.0/255.0); if (ri < 0) ri = 0; if (ri > 31) ri = 31;
    int gi = (int)(g*63.0/255.0); if (gi < 0) gi = 0; if (gi > 63) gi = 63;
    int bi = (int)(b*31.0/255.0); if (bi < 0) bi = 0; if (bi > 31) bi = 31;
    node->color = (unsigned short)(ri|(gi<<5)|(bi<<11));
  } else node->data = 0;
  return nodeIndex;
}

void OctreeBase::eraseOctreeData(unsigned int index) {
  if (index == 0) return;
  OctreeData *data = &octreeDatas[index]; data->freeIt();
  octreeDatas.erase(index,1);
  for (int i = 0; i < (int)octreeNodes.size(); i++) {if (octreeNodes[i].data >= index) octreeNodes[i].data--;}
}

void OctreeBase::generateOctreeChilds(unsigned int parent, const Vector &parentMin, const Vector &parentMax) {
  if ((parentMax.x-parentMin.x)<minChildSize.x||(parentMax.y-parentMin.y)<minChildSize.y||(parentMax.z-parentMin.z)<minChildSize.z) return;
  if (parent == 0) {return;}
  OctreeNode *parent2 = &octreeNodes[parent]; // this is a bit problematic since pointers may change on octreeNodes.push_back();
  if (parent2 == 0 || parent2->data == 0) return;
  if (octreeDatas[parent2->data].entityCount < minPolysPerNode) return;
  static Array<unsigned int> sides[8];
  Vector boxMin[8], boxMax[8];
  for (int child = 0; child < 8; child++) { sides[child].clear();
    generateOctreeChildBB(child, parentMin, parentMax, boxMin[child], boxMax[child]); Vector boxMin2 = boxMin[child], boxMax2 = boxMax[child];
    for (int i = 0; i < (int)octreeDatas[parent2->data].entityCount; i++) {
      unsigned int id = octreeDatas[parent2->data].entityIds[i];
      OctreeEntity *e = &octreeEntities[id];
      int clipFlags = (1<<6)-1;
      // if all points are outside of one cube side it is not colliding
      for (int j = 0; j < 3; j++) {
        Vector *p = &octreePos[e->pos[j]];
        if (p->x > boxMin2.x) clipFlags &= ~(1<<0);
        if (p->x < boxMax2.x) clipFlags &= ~(1<<1);
        if (p->y > boxMin2.y) clipFlags &= ~(1<<2);
        if (p->y < boxMax2.y) clipFlags &= ~(1<<3);
        if (p->z > boxMin2.z) clipFlags &= ~(1<<4);
        if (p->z < boxMax2.z) clipFlags &= ~(1<<5);
      }
      if (clipFlags == 0) { // clipFlags is 0 if atleast one side had a point in
        sides[child].push_back(id);
      }
    }
  }
  int sidesWithTooMuch = 0;
  for (int k = 0; k < 8; k++) {if ((int)sides[k].size() >= octreeDatas[parent2->data].entityCount) sidesWithTooMuch++;}
  if (sidesWithTooMuch<2) {
    bool processed = false;
    for (int c = 0; c < 8; c++) {
      if (!sides[c].empty()) {
        unsigned int child = buildOctreeNodeFromEntityIds(sides[c], boxMin[c], boxMax[c]); // not combined with line below because of memory inconsistencies with DJGPP optimizations
        octreeNodes[parent].childs[c] = child;
        processed = true;
      }
    }
    if (processed) {eraseOctreeData(octreeNodes[parent].data); octreeNodes[parent].data = 0;}
  }
}

void OctreeBase::generate() {
  for (int i = 0; i < (int)octreePos.size(); i++) {
    if (i == 0 || octreePos[i].x < bbBoxMin.x) bbBoxMin.x = octreePos[i].x;
    if (i == 0 || octreePos[i].y < bbBoxMin.y) bbBoxMin.y = octreePos[i].y;
    if (i == 0 || octreePos[i].z < bbBoxMin.z) bbBoxMin.z = octreePos[i].z;
    if (i == 0 || octreePos[i].x > bbBoxMax.x) bbBoxMax.x = octreePos[i].x;
    if (i == 0 || octreePos[i].y > bbBoxMax.y) bbBoxMax.y = octreePos[i].y;
    if (i == 0 || octreePos[i].z > bbBoxMax.z) bbBoxMax.z = octreePos[i].z;
  }
  Array<unsigned int> entityIds; entityIds.resize(octreeEntities.size()); {for (int i = 0; i < (int)entityIds.size(); i++) entityIds[i] = i;}
  octreeNodes.resize(1); octreeDatas.resize(1); // first (0) is terminator
  unsigned int rootid = buildOctreeNodeFromEntityIds(entityIds, bbBoxMin, bbBoxMax);
  Array<unsigned int> stack1; stack1.push_back(rootid); Array<Vector> stack2; stack2.push_back(bbBoxMin); Array<Vector> stack3; stack3.push_back(bbBoxMax);
  while(!stack1.empty()) {
    unsigned int here = stack1.back(); Vector minb = stack2.back(); Vector maxb = stack3.back(); stack1.pop_back(); stack2.pop_back(); stack3.pop_back();
    if (here == 0) continue;
    generateOctreeChilds(here, minb, maxb);
    bool somethingDone = false;
    for (int i = 0; i < 8; i++) {
      if (octreeNodes[here].childs[i] != 0) {
        stack1.push_back(octreeNodes[here].childs[i]);
        Vector boxMin, boxMax; generateOctreeChildBB(i,  minb, maxb, boxMin, boxMax);
        stack2.push_back(boxMin);
        stack3.push_back(boxMax);
        somethingDone = true;
      }
    }
    if (!somethingDone) {
      if ((rand() & 1023)==0) {printf("."); fflush(0);}
    }
  }
  octreePos2 = &octreePos[0]; // don't update the arrays after that
  octreeTex2 = &octreeTex[0]; // don't update the arrays after that
  octreeEntities2 = &octreeEntities[0]; // don't update the arrays after that
  octreeNodes2 = &octreeNodes[0]; // don't update the arrays after that
  octreeDatas2 = &octreeDatas[0]; // don't update the arrays after that
  root = &octreeNodes2[rootid];
}
//---------------

// OCTREE FRUSTUM
//---------------
OctreeFrustum::OctreeFrustum() {
  memset(minmax, 0, sizeof(minmax));
}

static void planeNormalIt(const Vector &p0, const Vector &p1, const Vector &p2, const Vector &p3, Vector &p, Vector &n) {
  (void)(p2);
  p = p0; n = normalize(cross(normalize(p1-p), normalize(p3-p)));
}

void OctreeFrustum::update() {
  for (int a = 0; a < 3; a++) {
    double min_d,max_d;
    for (int p = 0; p < 8; p++) {
      double v = 0;
      switch(a) {
        case 0: {v = points[p].x;} break;
        case 1: {v = points[p].y;} break;
        case 2: {v = points[p].z;} break;
      }
      if (p == 0 || v < min_d) min_d = v;
      if (p == 0 || v > max_d) max_d = v;
    }
    minmax[a*2+0] = min_d;
    minmax[a*2+1] = max_d;
  }
#define XYZ(__x__, __y__, __z__) points[(__x__) + (__y__)*2 + (__z__)*4]
  planeNormalIt(XYZ(0,0,1),XYZ(0,0,0),XYZ(0,1,0),XYZ(0,1,1), planePoint[0], planeNormal[0]); // left
  planeNormalIt(XYZ(1,1,1),XYZ(1,1,0),XYZ(1,0,0),XYZ(1,0,1), planePoint[1], planeNormal[1]); // right
  planeNormalIt(XYZ(1,0,0),XYZ(0,0,0),XYZ(0,0,1),XYZ(1,0,1), planePoint[2], planeNormal[2]); // bottom
  planeNormalIt(XYZ(1,1,1),XYZ(0,1,1),XYZ(0,1,0),XYZ(1,1,0), planePoint[3], planeNormal[3]); // top
  planeNormalIt(XYZ(1,1,0),XYZ(0,1,0),XYZ(0,0,0),XYZ(1,0,0), planePoint[4], planeNormal[4]); // near
  planeNormalIt(XYZ(1,0,1),XYZ(0,0,1),XYZ(0,1,1),XYZ(1,1,1), planePoint[5], planeNormal[5]); // far
#undef XYZ
  center = Vector(0,0,0); for (int i = 0; i < 8; i++) center += points[i]; center /= 8; //if (!isPointInside(center)) exit(0);
  for (int j = 0; j < 6; j++) {if (dot(center-planePoint[j],planeNormal[j])>0) planeNormal[j] *= -1;} // e.g. if we mirrored the world (chirality)
}

void OctreeFrustum::frustumFromOpenGL(double nearPlane, double farPlane) {
  Matrix mv; glGetDoublev(GL_MODELVIEW_MATRIX, mv.m);
  Matrix pr; glGetDoublev(GL_PROJECTION_MATRIX, pr.m); 
  Vector zn(0, 0, -nearPlane); zn = pr * zn; if (zn.w != 0) zn /= zn.w;
  Vector zf(0, 0, -farPlane); zf = pr * zf; if (zf.w != 0) zf /= zf.w;
  Matrix inv = inverse(pr * mv);
  for (int t = 0; t < 8; t++) {
    Vector k; double s = 1.0; k.x = (t & 1) ? s : -s; k.y = (t & 2) ? s : -s; k.z = (t & 4) ? zf.z : zn.z;
    k = inv * k; if (k.w != 0) k /= k.w;
    points[t] = k;
  }
  update();
}

bool OctreeFrustum::isPointInside(const Vector &point) {
  for (int i = 0; i < 6; i++) {
    if (dot(point-planePoint[i],planeNormal[i])>0) return false;
  }
  return true;
}

bool OctreeFrustum::isCubeFullyOutside(const Vector &minp, const Vector &maxp) {
  for (int j = 0; j < 6; j++) {
    bool found = true; Vector p = planePoint[j]; Vector n = planeNormal[j];
    double px = p.x; double py = p.y; double pz = p.z;
    double nx = n.x; double ny = n.y; double nz = n.z;
    for (int i = 0; i < 8; i++) {
      double x,y,z;
      x = (i & 1) ? maxp.x : minp.x;
      y = (i & 2) ? maxp.y : minp.y;
      z = (i & 4) ? maxp.z : minp.z;
      if ((x-px)*nx+(y-py)*ny+(z-pz)*nz<=0)  {found = false; break;} // if one inside this plane then not fully outside
    }
    if (found) return true;
  }
  return false;
}

double OctreeFrustum::nearPlaneSide(const Vector &point) {
  return (point.x-planePoint[4].x)*planeNormal[4].x+(point.y-planePoint[4].y)*planeNormal[4].y+(point.z-planePoint[4].z)*planeNormal[4].z;
}

bool OctreeFrustum::isCubeSomeBehindNearPlane(const Vector &minp, const Vector &maxp) {
  for (int i = 0; i < 8; i++) { Vector point; point.x = (i & 1) ? maxp.x : minp.x; point.y = (i & 2) ? maxp.y : minp.y; point.z = (i & 4) ? maxp.z : minp.z;
    if (dot(point-planePoint[4],planeNormal[4])>0) return true;
  }
  return false;
}

bool OctreeFrustum::isCubeSomeBehindFarPlane(const Vector &minp, const Vector &maxp) {
  for (int i = 0; i < 8; i++) { Vector point; point.x = (i & 1) ? maxp.x : minp.x; point.y = (i & 2) ? maxp.y : minp.y; point.z = (i & 4) ? maxp.z : minp.z;
    if (dot(point-planePoint[5],planeNormal[5])>0) return true;
  }
  return false;
}

void OctreeFrustum::cubeNearest(const Vector &minp, const Vector &maxp, double &nearest) {
  const Vector &p = planePoint[4]; const Vector& n = planeNormal[4];
  double px = p.x; double py = p.y; double pz = p.z; double nx = -n.x; double ny = -n.y; double nz = -n.z; // negated because of easier handling
  for (int i = 0; i < 8; i++) {
    double x,y,z;
    x = (i & 1) ? maxp.x : minp.x;
    y = (i & 2) ? maxp.y : minp.y;
    z = (i & 4) ? maxp.z : minp.z;
    double dist = (x-px)*nx + (y-py)*ny + (z-pz)*nz;
    if (i == 0 || dist < nearest) nearest = dist;
  }
}
//---------------

// OCTREE TRAVERSE / PAINT
//---------------

static void quad(const Vector &p0, const Vector &p1, const Vector &p2, const Vector &p3) {
  glVertex3d(p3.x,p3.y,p3.z);
  glVertex3d(p2.x,p2.y,p2.z);
  glVertex3d(p1.x,p1.y,p1.z);
  glVertex3d(p0.x,p0.y,p0.z);
}

static void displayColoredBoundingBox(unsigned short color, const Vector &bbMin, const Vector &bbMax) {
  float r = (float)(color & 31)/31.f;
  float g = (float)((color>>5) & 63)/63.f;
  float b = (float)((color>>11) & 31)/31.f;
  glBegin(GL_QUADS);
  glColor4f(r,g,b,1);
#define XYZ(__x__, __y__, __z__) Vector((__x__) ? bbMax.x : bbMin.x, (__y__) ? bbMax.y : bbMin.y, (__z__) ? bbMax.z : bbMin.z)
  quad(XYZ(0,0,1),XYZ(0,0,0),XYZ(0,1,0),XYZ(0,1,1));
  quad(XYZ(1,1,1),XYZ(1,1,0),XYZ(1,0,0),XYZ(1,0,1));
  quad(XYZ(1,0,0),XYZ(0,0,0),XYZ(0,0,1),XYZ(1,0,1));
  quad(XYZ(1,1,1),XYZ(0,1,1),XYZ(0,1,0),XYZ(1,1,0));
  quad(XYZ(1,1,0),XYZ(0,1,0),XYZ(0,0,0),XYZ(1,0,0));
  quad(XYZ(1,0,1),XYZ(0,0,1),XYZ(0,1,1),XYZ(1,1,1));
#undef XYZ
  glColor4f(1,1,1,1);
  glEnd();
}

static void displayPolys(OctreeBase *base, OctreeData *data) {
  for (int i = 0; i < (int)data->entityCount; i++) {
    OctreeEntity *e = &base->octreeEntities2[data->entityIds[i]]; if (e->paintFrame == base->paintFrame) continue;
    e->paintFrame = base->paintFrame;
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, e->texHandle);
    glBegin(GL_TRIANGLES);
    Vector *t, *p;
    t = &base->octreeTex2[e->tex[0]]; glTexCoord2d(t->x,t->y); p = &base->octreePos2[e->pos[0]]; glVertex3d(p->x,p->y,p->z);
    t = &base->octreeTex2[e->tex[1]]; glTexCoord2d(t->x,t->y); p = &base->octreePos2[e->pos[1]]; glVertex3d(p->x,p->y,p->z);
    t = &base->octreeTex2[e->tex[2]]; glTexCoord2d(t->x,t->y); p = &base->octreePos2[e->pos[2]]; glVertex3d(p->x,p->y,p->z);
    glEnd();
    glDisable(GL_TEXTURE_2D);
  }
}

static void ocQuad(const Vector &p0, const Vector &p1, const Vector &p2, const Vector &p3) {
  glVertex3d(p3.x,p2.y,p3.z); 
  glVertex3d(p2.x,p2.y,p2.z);
  glVertex3d(p1.x,p1.y,p1.z);
  glVertex3d(p0.x,p0.y,p0.z);
}

static bool isOccluded(OctreeFrustum *frustum, const Vector &bbMin, const Vector &bbMax) {
  // you need -D__GLSOMEDEPTHPASSED__=1 as compiler switch to enable Occlusion Query/Culling in WatcomGL
  for (int i = 0; i < 8; i++) {
    Vector p((i & 1)?bbMax.x:bbMin.x,(i & 2)?bbMax.y:bbMin.y,(i & 4)?bbMax.z:bbMin.z);
    if (frustum->nearPlaneSide(p)>0) return false; // if one point is behind the nearplane (outside)
  }
  glSomeDepthPassed = GL_FALSE; glBreakOnSomeDepthPassed = GL_TRUE;
  // face culling is enabled
  GLboolean clipOn[GLMAXCLIPPLANES]; for(int a = 0; a < GLMAXCLIPPLANES; a++) {clipOn[a] = glIsEnabled(GL_CLIP_PLANE0+a); glDisable(GL_CLIP_PLANE0+a);}
  glBegin(GL_QUADS);
#define XYZ_(__x__, __y__, __z__) Vector((__x__) ? bbMax.x : bbMin.x, (__y__) ? bbMax.y : bbMin.y, (__z__) ? bbMax.z : bbMin.z)
  ocQuad(XYZ_(0,0,1),XYZ_(0,0,0),XYZ_(0,1,0),XYZ_(0,1,1)); // left
  ocQuad(XYZ_(1,1,1),XYZ_(1,1,0),XYZ_(1,0,0),XYZ_(1,0,1)); // right
  ocQuad(XYZ_(1,0,0),XYZ_(0,0,0),XYZ_(0,0,1),XYZ_(1,0,1)); // bottom
  ocQuad(XYZ_(1,1,1),XYZ_(0,1,1),XYZ_(0,1,0),XYZ_(1,1,0)); // top
  ocQuad(XYZ_(1,1,0),XYZ_(0,1,0),XYZ_(0,0,0),XYZ_(1,0,0)); // near
  ocQuad(XYZ_(1,0,1),XYZ_(0,0,1),XYZ_(0,1,1),XYZ_(1,1,1)); // far
  glEnd();
  for(int b = 0; b < GLMAXCLIPPLANES; b++) {if (clipOn[b]) glEnable(GL_CLIP_PLANE0+b);}
  bool yes = glSomeDepthPassed ? true : false; glBreakOnSomeDepthPassed = GL_FALSE; glSomeDepthPassed = GL_FALSE;
  return !yes; // if no pixel was painted this is occluded
}

class OctreeTraverse {
public:
  OctreeTraverse() : node(NULL),minFromNearPlane(0),used(false) {;}
  OctreeTraverse(OctreeNode *node, const Vector &bbMin, const Vector &bbMax) : node(node), bbMin(bbMin), bbMax(bbMax), minFromNearPlane(0),used(false) {;}
  OctreeNode *node;
  Vector bbMin,bbMax;
  double minFromNearPlane;
  bool used;
};

static void nodeContentBounds(const OctreeNode *node, Vector &minb, Vector &maxb) {
  unsigned int min10 = node->bbBoxMinXYZ10; unsigned int max10 = node->bbBoxMaxXYZ10;
  double minx = (min10 & 1023) / 1023.0; double miny = ((min10>>10) & 1023) / 1023.0; double minz = ((min10>>20) & 1023) / 1023.0;
  double maxx = (max10 & 1023) / 1023.0; double maxy = ((max10>>10) & 1023) / 1023.0; double maxz = ((max10>>20) & 1023) / 1023.0;
  Vector siz = maxb - minb; Vector origin = minb;
  minb.x = origin.x + siz.x * minx; minb.y = origin.y + siz.y * miny; minb.z = origin.z + siz.z * minz;
  maxb.x = origin.x + siz.x * maxx; maxb.y = origin.y + siz.y * maxy; maxb.z = origin.z + siz.z * maxz;
}

void paintOctree(OctreeFrustum *frustum, OctreeBase *base, int maxPolysBeforeCubesArePainted) {
  base->paintFrame = (unsigned char)((base->paintFrame + 1) & 255);
  frustum->update();
  static Array<OctreeTraverse> traverseNodes;  static Array<int> traverseFreeed; traverseNodes.clear(); traverseFreeed.clear();
  OctreeTraverse trv; trv.node = base->root; trv.bbMin = base->bbBoxMin; trv.bbMax = base->bbBoxMax; frustum->cubeNearest(trv.bbMin, trv.bbMax, trv.minFromNearPlane); trv.used = true;
  traverseNodes.push_back(trv);
  int polyCount = 0;
  while(!traverseNodes.empty()) {
    // take the front most child to process next (because of Occlusion Culling (and early Z with WatcomGL))
    double d; int found = -1; {for (int i = 0; i < (int)traverseNodes.size(); i++) {const OctreeTraverse &node = traverseNodes[i]; if (node.used && (found < 0 || node.minFromNearPlane < d)) {d = node.minFromNearPlane; found = i;}}} if (found < 0) break;
    OctreeTraverse &n = traverseNodes[found]; n.used = false; traverseFreeed.push_back(found);
    if (n.node == NULL) continue;
    if (frustum->isCubeFullyOutside(n.bbMin, n.bbMax)) continue;

    double cubeSize = length(n.bbMax - n.bbMin); Vector cubeCenter = (n.bbMax + n.bbMin)*0.5; double cubeDistNearPlane = -frustum->nearPlaneSide(cubeCenter);
    bool isCubeMode = polyCount > maxPolysBeforeCubesArePainted;
    Vector minb = n.bbMin, maxb = n.bbMax; nodeContentBounds(n.node, minb, maxb);
    if (isCubeMode && cubeSize * 6 < cubeDistNearPlane) {displayColoredBoundingBox(n.node->color, minb, maxb); continue;}
    if (isOccluded(frustum, minb, maxb)) continue;
    if (n.node->data != 0) {
      int a = glDrawnTrianglesFrame;
      displayPolys(base, &base->octreeDatas2[n.node->data]);
      polyCount += glDrawnTrianglesFrame - a;
      continue;
    }
    OctreeTraverse n2 = n; // no reference allowed
    {for (int i = 0; i < 8; i++) {
      unsigned int child = n2.node->childs[i];
      if (child != 0) {
        Vector boxMin, boxMax; base->generateOctreeChildBB(i, n2.bbMin, n2.bbMax, boxMin, boxMax);
        trv.node = &base->octreeNodes2[child]; trv.bbMin = boxMin; trv.bbMax = boxMax; frustum->cubeNearest(boxMin, boxMax, trv.minFromNearPlane);
        trv.used = true;
        if (traverseFreeed.empty()) 
          traverseNodes.push_back(trv);
        else {
          unsigned int found = traverseFreeed.back(); traverseFreeed.pop_back();
          traverseNodes[found] = trv;
        }
      }
    }}
  }
}
//---------------
