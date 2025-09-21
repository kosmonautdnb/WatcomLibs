// simple wavefront .obj + .mtl loader
// simple stanford .ply (ASCII) loader
#ifndef __SMPLOBJL_HPP__
#define __SMPLOBJL_HPP__

#include "types.hpp"
#include "vector.hpp"
#include "string.hpp"
#include "hashmap.hpp"
#include <string.h>

#define SMPL_MAXV 4
#define SMPL_MAXV_LOAD 24

class SMPL_Face {
public:
  SMPL_Face() {m=-1;c=0;vertexColors=false;memset(v,0,sizeof(v));memset(n,0,sizeof(n));memset(t,0,sizeof(t));}
  int32_t v[SMPL_MAXV]; // vertex index
  int32_t n[SMPL_MAXV]; // normal index
  int32_t t[SMPL_MAXV]; // texturecoord index
  int32_t m; // material id
  int32_t c; // vertex count
  bool vertexColors; //can be painted by just using vertex colors
};

class SMPL_Object {
public:
  String name;
  int32_t faceStart;
  int32_t faceEnd;
};

class SMPL_Texture {
public:
  SMPL_Texture() : used(false), glHandle(0) {}
  bool used;
  uint32_t glHandle;
  String fileName;
};

class SMPL_Material {
public:
  int32_t materialId;
  Vector diffuse; // Kd
  Vector ambient; // Ka
  Vector specular; // Ks
  Vector emissive; // ke
  float specularPower; //Ns
  float refract; //Ni
  float alpha; //d,Tr
  SMPL_Texture mapDiffuse; // map_Kd
  SMPL_Texture mapAmbient; // map_Ka
  SMPL_Texture mapSpecular; // map_Ks
  SMPL_Texture mapEmissive; // map_Ke
  SMPL_Texture mapSpecularPower; // map_Ns
  SMPL_Texture mapRefract; // map_Ni
  SMPL_Texture mapAlpha; // map_d
  SMPL_Texture mapTransparency; // map_Tr
  SMPL_Texture mapBump; // map_bump
  SMPL_Texture mapReflectivity; // map_refl
};

// ...->loadTextures(SMPL_loadTexture);
typedef uint32_t(*TextureLoad_t)(const String &fileName, const String &type);
#define SMPL_LOADTEXTURE \
unsigned int SMPL_loadTexture(const String &fileName, const String &type) {\
  uint32_t texture = 0;\
  if (type != "map_Kd") return texture;\
  char *buffer = new char[MAX_LFN_PATH];\
  if (doslfnShortPath(buffer, fileName.c_str())) {\
    RGBAImage image = RGBAImage::fromFile(buffer);\
    glGenTextures(1, &texture);\
    glBindTexture(GL_TEXTURE_2D, texture);\
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,image.width,image.height,0,GL_RGBA,GL_UNSIGNED_BYTE,image.data);\
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);\
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);\
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);\
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);\
    image.free();\
  }\
  delete[] buffer;\
  return texture;\
}

class SMPL_File {
public:
  Array<uint32_t> colors; // per vertex color .obj property / empty
  Array<Vector> vertices;
  Array<Vector> normals;
  Array<Vector> texCoords;
  HashMap<int32_t,String> materialNames;
  Array<SMPL_Face> faces;
  Array<SMPL_Object> objs;
  HashMap<String,SMPL_Material> materials;
  Array<SMPL_Material*> materialsById;
  void loadTextures(TextureLoad_t functor);
  void genVertexColors(const Array<String> &vertexColorMaterials);
  bool makeFrontFacing(bool ccw);
};

class SMPL_Ply {
public:
  SMPL_Ply() : vertexStride(0), xProperty(0), yProperty(0), zProperty(0), vertexCount(0), faceCount(0) {
  }
  Array<float> vertices;
  Array<String> vertexProperties;
  Array<int32_t> faceIndices;
  Array<int32_t> faceIndexCount;
  Array<int32_t> faceStartIndex;
  int32_t vertexStride;
  int32_t xProperty;
  int32_t yProperty;
  int32_t zProperty;
  int32_t vertexCount;
  int32_t faceCount;
  Vector vertex(const int index) {const int32_t z = index*vertexStride;return Vector(vertices[z+xProperty],vertices[z+yProperty],vertices[z+zProperty]);}
  int32_t *face(const int32_t index) {return &faceIndices[faceStartIndex[index]];}
  int32_t faceVertexCount(const int32_t index) {return faceIndexCount[index];}
};

SMPL_File *loadObj(const String &fileName, bool triangulate = false);
SMPL_Ply *loadPly(const String &fileName);

#endif // __SMPLOBJL_HPP__
