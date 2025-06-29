// simple wavefront .obj + .mtl loader
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
};

class SMPL_Object {
public:
  String name;
  int faceStart;
  int faceEnd;
};

class SMPL_Material {
public:
  Vector diffuse; // Kd
  Vector ambient; // Ka
  Vector specular; // Ks
  Vector emissive; // ke
  float specularPower; //Ns
  float refract; //Ni
  float alpha; //d,Tr
};

class SMPL_File {
public:
  Array<Vector> vertices;
  Array<Vector> normals;
  Array<Vector> texCoords;
  HashMap<int,String> materialNames;
  Array<SMPL_Face> faces;
  Array<SMPL_Object> objs;
  HashMap<String,SMPL_Material> materials;
};

SMPL_File *loadObj(const String &fileName, bool triangulate = false);

#endif // __SMPLOBJL_HPP__
