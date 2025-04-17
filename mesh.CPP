#include "C:\_OPENGL_\GL.H"
#include "mesh.hpp"
#include "textures.hpp"

#define TINYOBJ_LOADER_C_IMPLEMENTATION
#include "tinyobjl.hpp"

static void loadFile(void *ctx, const char * filename, int is_mtl, const char *obj_filename, char ** buffer, size_t * len)
{
  long string_size = 0, read_size = 0;
  FILE * handler = fopen(filename, "rb");
  
  if (handler) {
    fseek(handler, 0, SEEK_END);
    string_size = ftell(handler);
    rewind(handler);
    *buffer = (char *) malloc(sizeof(char) * (string_size + 1));
    read_size = fread(*buffer, sizeof(char), (size_t) string_size, handler);
    (*buffer)[string_size] = '\0';
    if (string_size != read_size) {
      free(*buffer);
      *buffer = NULL;
    }
    fclose(handler);
  }
  
  *len = read_size;
}

static String texName(const String &folder, const String &name) {
  int p = name.findLast("/");
  int p2 = name.findLast("\\");
  if (p2 > p) p = p2;
  return folder + "/" + name.substr(p+1);
}

bool Obj3D::loadOBJ(const char *fileName, const char *textureFolder) {

  tinyobj_shape_t * shape = NULL;
  tinyobj_material_t * material = NULL;
  tinyobj_attrib_t attrib;
  
  size_t num_shapes;
  size_t num_materials;
  
  tinyobj_attrib_init(&attrib);
  
  int result = tinyobj_parse_obj(&attrib, &shape, &num_shapes, &material, &num_materials, fileName, loadFile, NULL, TINYOBJ_FLAG_TRIANGULATE);
  if (result != TINYOBJ_SUCCESS)
    return false;

  vertices.resize(attrib.num_vertices);
  if (!vertices.empty()) {
    for (int i = 0; i < attrib.num_vertices; i++)
      vertices[i] = Vector(attrib.vertices[i*3+0],attrib.vertices[i*3+1],attrib.vertices[i*3+2]);
  }
  uvs.resize(attrib.num_texcoords);
  if (!uvs.empty()) {
    for (int i = 0; i < attrib.num_texcoords; i++)
      uvs[i] = Vector(attrib.texcoords[i*2+0],attrib.texcoords[i*2+1]);
  }
  normals.resize(attrib.num_normals);
  if (!normals.empty()) {
    for (int i = 0; i < attrib.num_normals; i++)
      normals[i] = Vector(attrib.normals[i*3+0],attrib.normals[i*3+1],attrib.normals[i*3+2]);
  }
  faces.resize(attrib.num_face_num_verts);
  if(!faces.empty()) {
    tinyobj_vertex_index_t *v = attrib.faces;
    for (int i = 0; i < attrib.num_face_num_verts; i++) {
      for (int j = 0; j < 3; j++) {
        faces[i].v[j] = v->v_idx;
        faces[i].t[j] = v->vt_idx;
        faces[i].n[j] = v->vn_idx;
        faces[i].c[j] = -1;
        v++;
      }
      faces[i].m = attrib.material_ids[i];
    }
  }
  materials.clear();
  if (num_materials != 0) {
    for (int i = 0; i < num_materials; i++) {
      const tinyobj_material_t &s = material[i];
      Material m;
      m.materialName = s.name;
      m.illumModel = s.illum;
      m.ambient_texname = s.ambient_texname;
      m.diffuse_texname = s.diffuse_texname;
      m.specular_texname = s.specular_texname;
      m.specular_highlight_texname = s.specular_highlight_texname;
      m.bump_texname = s.bump_texname;
      m.displacement_texname = s.displacement_texname;
      m.alpha_texname = s.alpha_texname;
      m.ambient.r = s.ambient[0];
      m.ambient.g = s.ambient[1];
      m.ambient.b = s.ambient[2];
      m.diffuse.r = s.diffuse[0];
      m.diffuse.g = s.diffuse[1];
      m.diffuse.b = s.diffuse[2];
      m.specular.r = s.specular[0];
      m.specular.g = s.specular[1];
      m.specular.b = s.specular[2];
      m.transmittance.r = s.transmittance[0];
      m.transmittance.g = s.transmittance[1];
      m.transmittance.b = s.transmittance[2];
      m.emission.r = s.emission[0];
      m.emission.g = s.emission[1];
      m.emission.b = s.emission[2];
      m.shininess = s.shininess;
      m.ior = s.ior;
      m.alpha = s.dissolve;

      m.diffuse_glhandle = 0;
      if (textureFolder != NULL) {
        if (!m.diffuse_texname.empty()) m.diffuse_glhandle = glLoadTexture(texName(textureFolder,m.diffuse_texname.c_str()).c_str());
      }
      materials[i] = m;
    }
  }
  tinyobj_attrib_free(&attrib);
  tinyobj_shapes_free(shape, num_shapes);
  tinyobj_materials_free(material, num_materials);

  return true;
}

void Obj3D::display() {
  glEnable(GL_TEXTURE_2D);
  glVertex4f(0,0,0,1);
  glColor4f(1,1,1,1);
  glBindTexture(GL_TEXTURE_2D,0);
  glEnable(GL_ALPHA_TEST);
  glAlphaFunc(GL_GREATER,230.f/255.f);
  glBegin(GL_TRIANGLES);
  for (int i = 0; i < faces.size(); i++) {
    const Triangle &f = faces[i];
    Material &m = materials[f.m];
    glBindTexture(GL_TEXTURE_2D,m.diffuse_glhandle);
    for (int j = 0; j < 3; j++) {
      if (!normals.empty()) {const Vector &n = normals[f.n[j]]; glNormal3f(n.x,n.y,n.z);}
      if (!uvs.empty()) {const Vector &t = uvs[f.t[j]]; glTexCoord3f(t.x,t.y,t.z);}
      if (!vertices.empty()) {const Vector &v = vertices[f.v[j]]; glVertex3f(v.x,v.y,v.z);}
    }
  }
  glEnd();
}
