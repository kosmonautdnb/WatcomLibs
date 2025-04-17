#ifndef __MESH_HPP__
#define __MESH_HPP__

#include "types.hpp"
#include "array.hpp"
#include "vector.hpp"
#include "hashmap.hpp"
#include "string.hpp"

class Color {
public:
  float r,g,b,a;
};

class Triangle {
public:
  int v[3];
  int t[3];
  int n[3];
  int c[3];
  int m;
};

class Material {
public:
  String materialName;
  int illumModel;
  String ambient_texname;            /* map_Ka */
  String diffuse_texname;            /* map_Kd */
  String specular_texname;           /* map_Ks */
  String specular_highlight_texname; /* map_Ns */
  String bump_texname;               /* map_bump, bump */
  String displacement_texname;       /* disp */
  String alpha_texname;              /* map_d */
  Color ambient;
  Color diffuse;
  Color specular;
  Color transmittance;
  Color emission;
  float shininess;
  float ior;
  float alpha;
  unsigned int diffuse_glhandle;
};

class Obj3D {
public:
  Array<Triangle> faces;  
  Array<Vector> vertices;
  Array<Vector> uvs;
  Array<Vector> normals;
  Array<Vector> colors;
  HashMap<int,Material> materials;
  
  bool loadOBJ(const char *fileName, const char *textureFolder = NULL);
  void display();
};

#endif //__MESH_HPP__