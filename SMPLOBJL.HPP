// simple wavefront .obj + .mtl loader
// simple stanford .ply (ASCII) loader
#ifndef __SMPLOBJL_HPP__
#define __SMPLOBJL_HPP__

#include "vector.hpp"
#include "string.hpp"
#include "hashmap.hpp"

#define SMPL_MAXV 4
#define SMPL_MAXV_LOAD 24

class SMPL_Face {
public:
  int v[SMPL_MAXV]; // vertex index
  int n[SMPL_MAXV]; // normal index
  int t[SMPL_MAXV]; // texturecoord index
  int m; // material id
  int c; // vertex count
  bool vertexColors; //can be painted by just using vertex colors
};

class SMPL_Object {
public:
  String name;
  int faceStart;
  int faceEnd;
};

class SMPL_Texture {
public:
  SMPL_Texture() : used(false), glHandle(0) {}
  bool used;
  unsigned int glHandle;
  String fileName;
};

class SMPL_Material {
public:
  int materialId;
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
typedef unsigned int(*TextureLoad_t)(const String &fileName, const String &type);
#define SMPL_LOADTEXTURE \
unsigned int SMPL_loadTexture(const String &fileName, const String &type) {\
  unsigned int texture = 0;\
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
  Array<unsigned int> colors; // per vertex color .obj property / empty
  Array<Vector> vertices;
  Array<Vector> normals;
  Array<Vector> texCoords;
  HashMap<int,String> materialNames;
  Array<SMPL_Face> faces;
  Array<SMPL_Object> objs;
  HashMap<String,SMPL_Material> materials;
  void loadTextures(TextureLoad_t functor);
  void genVertexColors(const Array<String> &vertexColorMaterials);
};

class SMPL_Ply {
public:
  SMPL_Ply() : vertexStride(0), xProperty(0), yProperty(0), zProperty(0), vertexCount(0), faceCount(0) {
  }
  Array<float> vertices;
  Array<String> vertexProperties;
  Array<int> faceIndices;
  Array<int> faceIndexCount;
  Array<int> faceStartIndex;
  int vertexStride;
  int xProperty;
  int yProperty;
  int zProperty;
  int vertexCount;
  int faceCount;
  Vector vertex(const int index) {const int z = index*vertexStride;return Vector(vertices[z+xProperty],vertices[z+yProperty],vertices[z+zProperty]);}
  int *face(const int index) {return &faceIndices[faceStartIndex[index]];}
  int faceVertexCount(const int index) {return faceIndexCount[index];}
};

SMPL_File *loadObj(const String &fileName, bool triangulate = false);
SMPL_Ply *loadPly(const String &fileName);

#endif // __SMPLOBJL_HPP__
