#include <string.h>
#include <stdlib.h>
#include <math.h>
#define CGLTF_IMPLEMENTATION
#include "cgltf.hpp"
#include "array.hpp"
#include "hashmap.hpp"
#include "vector.hpp"
#include "matrix.hpp"
#include "quaternn.hpp"
#include "gl.h"
#include "keymtrix.hpp"
#include "image.hpp"

cgltf_data *loadGLTF(const char *fileName) { // glt/glb
  cgltf_options options;
  cgltf_data *data;
  memset(&options,0,sizeof(cgltf_options));
  cgltf_result result = cgltf_parse_file(&options, fileName, &data);
  if (result == cgltf_result_success) {
    result = cgltf_load_buffers(&options, data, fileName);
    if (result == cgltf_result_success) {
      result = cgltf_validate(data);
    }
  }
  return result == cgltf_result_success ? data : NULL;
}

enum GLTFPrimitive_type {
  GLTFPrimitive_type_unsupported = 0,
  GLTFPrimitive_type_triangle = 1,
};

enum GLTFAnimation_type {
   GLTFAnimation_type_scaling = 0,
   GLTFAnimation_type_rotation = 1,
   GLTFAnimation_type_translation = 2,
};

enum GLTFInterpolation_type {
   GLTFInterpolation_type_linear = 0,
   GLTFInterpolation_type_step = 1,
   GLTFInterpolation_type_cubic = 2,
};

class GLTFPrimitive {
public:
  GLTFPrimitive_type primitive_type;
  Array<int> indices;
  HashMap<int,Array<Vector> > positions;
  HashMap<int,Array<Vector> > colors;
  HashMap<int,Array<Vector> > normals;
  HashMap<int,Array<Vector> > texCoords;
  HashMap<int,Array<Vector> > joints; // actually ints
  HashMap<int,Array<Vector> > weights;
};

class GLTFMesh {
public:
  Array<GLTFPrimitive*> primitives;
  void init(cgltf_mesh *v);
};

class GLTFJoint {
public:
  GLTFJoint() {
    id = 0;
    parent_id = 0;
    matrixUpdateId = 0;
  }
  unsigned int id;
  unsigned int parent_id;
  Array<unsigned int> child_ids;
  bool hasScaling;
  bool hasRotation;
  bool hasTranslation;
  bool hasMatrix;
  Vector scaling;
  Quaternion rotation;
  Vector translation;
  Matrix matrix;
  int matrixUpdateId;
  Matrix matrixHereWithParent;
  Matrix inverseBindMatrix;
  Matrix finalMatrix;
  Matrix finalNormalMatrix;
};

class GLTFSkin {
public:
  Array<GLTFJoint> joints;
  HashMap<int, GLTFJoint*> jointFromId;
  void init(cgltf_skin *v);
  __inline Vector transformPosition(const Vector &p, const Vector &joints, const Vector &weights) const;
  __inline Vector transformNormal(const Vector &n, const Vector &joints, const Vector &weights) const;
  Matrix getMatrix(unsigned int id, unsigned int frameId);
  void recalculateJoints();
};

class GLTFAnimationChannel {
public:
  Array<float> times;
  Array<Vector> data;
  GLTFAnimation_type type;
  GLTFInterpolation_type interpolation;  
  float timeMin;
  float timeMax;
  int node_id;
};

class GLTFAnimation {
public:
  Array<GLTFAnimationChannel> channels;
  void init(cgltf_animation *v);
};

GLTFJoint sceneNode;

__inline Vector GLTFSkin::transformPosition(const Vector &p, const Vector &joints4, const Vector &weights4) const {
  Vector r,k; double w;const GLTFJoint *l;
  w = weights4.x; if (w!=0.0) {
    l = &joints[(int)joints4.x];
    k = l->finalMatrix * p;
    r.x += k.x*w;
    r.y += k.y*w;
    r.z += k.z*w;
  }
  w = weights4.y; if (w!=0.0) {
    l = &joints[(int)joints4.y];
    k = l->finalMatrix * p;
    r.x += k.x*w;
    r.y += k.y*w;
    r.z += k.z*w;
  }
  w = weights4.z; if (w!=0.0) {  
    l = &joints[(int)joints4.z];
    k = l->finalMatrix * p;
    r.x += k.x*w;
    r.y += k.y*w;
    r.z += k.z*w;
  }
  w = weights4.w; if (w!=0.0) {
    l = &joints[(int)joints4.w];
    k = l->finalMatrix * p;
    r.x += k.x*w;
    r.y += k.y*w;
    r.z += k.z*w;
  }
  r.w = 1;
  return r;
}

__inline Vector GLTFSkin::transformNormal(const Vector &n, const Vector &joints4, const Vector &weights4) const {
  Vector r,k; double w;const GLTFJoint *l;
  w = weights4.x; if (w!=0.0) {
    l = &joints[(int)joints4.x];
    k = l->finalNormalMatrix * n;
    r.x += k.x*w;
    r.y += k.y*w;
    r.z += k.z*w;
  }
  w = weights4.y; if (w!=0.0) {
    l = &joints[(int)joints4.y];
    k = l->finalNormalMatrix * n;
    r.x += k.x*w;
    r.y += k.y*w;
    r.z += k.z*w;
  }
  w = weights4.z; if (w!=0.0) {  
    l = &joints[(int)joints4.z];
    k = l->finalNormalMatrix * n;
    r.x += k.x*w;
    r.y += k.y*w;
    r.z += k.z*w;
  }
  w = weights4.w; if (w!=0.0) {
    l = &joints[(int)joints4.w];
    k = l->finalNormalMatrix * n;
    r.x += k.x*w;
    r.y += k.y*w;
    r.z += k.z*w;
  }
  return r;
}

static Matrix getMatrixHere(GLTFJoint *j) {
  // m = t * r * s
  Matrix matrixHere; matrixHere.identity();
  if (j->hasScaling) {matrixHere.scale3(j->scaling);}
  if (j->hasRotation) {matrixHere = j->rotation.rotationMatrix()*matrixHere;}
  if (j->hasTranslation) {matrixHere.setColumn(3,j->translation);}
  if (j->hasMatrix) {matrixHere = j->matrix;}
  return matrixHere;
}

Matrix GLTFSkin::getMatrix(unsigned int id, unsigned int frameId) {
  if (jointFromId.has(id)) {
    GLTFJoint *j = jointFromId[id];
    if (j->matrixUpdateId==frameId) return j->matrixHereWithParent;
    j->matrixHereWithParent = getMatrix(j->parent_id,frameId)*getMatrixHere(j);
    j->matrixUpdateId = frameId;
    return j->matrixHereWithParent;
  }
  // hardcoded default scene node
  return getMatrixHere(&sceneNode);
}

void GLTFSkin::recalculateJoints() {
  {for (int i = 0; i < joints.size(); i++) {
    GLTFJoint *j = &joints[i];
    j->matrixUpdateId = 0;
  }}
  {for (int i = 0; i < joints.size(); i++) {
    GLTFJoint *j = &joints[i];
    j->finalMatrix = getMatrix(j->id,1)*j->inverseBindMatrix;
    j->finalNormalMatrix = transpose(inverse(j->finalMatrix));
  }}
}

void GLTFMesh::init(cgltf_mesh *v) {
  // load primitive data
  {for (int i = 0; i < primitives.size(); i++) delete[] primitives[i];}
  primitives.clear();
  primitives.resize(v->primitives_count);
  for (int i = 0; i < primitives.size(); i++) {
    cgltf_primitive *p = &v->primitives[i]; GLTFPrimitive *r = new GLTFPrimitive(); primitives[i] = r;
    // triangles?
    switch(p->type) {
      case cgltf_primitive_type_triangles: {r->primitive_type = GLTFPrimitive_type_triangle;} break;
      default: {r->primitive_type = GLTFPrimitive_type_unsupported;} break;
    }
    // indices
    r->indices.resize(p->indices->count);
    int a = cgltf_accessor_unpack_indices(p->indices, &r->indices[0], 4, p->indices->count); if (a != p->indices->count) {printf("error1\n");exit(0);}
    // attributes
    for (int j = 0; j < p->attributes_count; j++) {
      const cgltf_attribute *attr = &p->attributes[j];
      HashMap<int,Array<Vector> > *dest = NULL;
      switch(attr->type) {
        case cgltf_attribute_type_position: { dest = &r->positions; } break;
        case cgltf_attribute_type_normal: { dest = &r->normals; } break;
        case cgltf_attribute_type_color: { dest = &r->colors; } break;
        case cgltf_attribute_type_texcoord: { dest = &r->texCoords;  } break;
        case cgltf_attribute_type_joints: { dest = &r->joints;  } break;
        case cgltf_attribute_type_weights: { dest = &r->weights;  } break;
      }
      if (dest != NULL) {
        int index = attr->index;
        Array<Vector> *d = &(*dest)[index];
        const cgltf_accessor *accessor = attr->data;
        int floatCount = cgltf_num_components(accessor->type);
        int overallFloats = attr->data->count*floatCount;
        float *ac = new float[overallFloats];
        cgltf_accessor_unpack_floats(accessor,ac,overallFloats);
        d->resize(overallFloats/floatCount+1);
        for (int k = 0; k < overallFloats; k++) {
          Vector *l = &(*d)[k/floatCount];
          int ki = k % floatCount;
          switch(ki) {         
            case 0: {l->x = ac[k];} break;
            case 1: {l->y = ac[k];} break;
            case 2: {l->z = ac[k];} break;
            case 3: {l->w = ac[k];} break;
          }
        }
        delete[] ac;
      }
    }
  }
}

static void fillJoint(GLTFJoint *j, cgltf_node *k) {
  j->id = (unsigned int)k;
  j->parent_id = (unsigned int)k->parent;
  j->child_ids.resize(k->children_count);
  for (int l = 0; l < j->child_ids.size(); l++) {
    j->child_ids[l] = (unsigned int)k->children[l];
  }
  j->hasScaling = k->has_scale; j->scaling = Vector(k->scale[0],k->scale[1],k->scale[2]);
  j->hasRotation = k->has_rotation; j->rotation = Quaternion(k->rotation[0],k->rotation[1],k->rotation[2],k->rotation[3]);
  j->hasTranslation = k->has_translation; j->translation = Vector(k->translation[0],k->translation[1],k->translation[2]);
  j->hasMatrix = k->has_matrix; for (int m = 0; m < 16; m++) j->matrix.m[m] = k->matrix[m];
}

void GLTFSkin::init(cgltf_skin *v) {
  joints.clear();
  joints.resize(v->joints_count);
  jointFromId.clear();
  for (int i = 0; i < joints.size(); i++) {
    fillJoint(&joints[i],v->joints[i]);
    jointFromId[joints[i].id] = &joints[i];
  }
  cgltf_accessor *accessor = v->inverse_bind_matrices;
  int count = accessor->count * 16;
  float *ac = new float[count];
  int a=cgltf_accessor_unpack_floats(accessor,ac,count);
  if (a != count||count!=joints.size()*16) {printf("error%d,%d\n",a,count);exit(0);}
  float *read = ac;
  for (int j = 0; j < joints.size(); j++) {
    for (int l = 0; l < 16; l++) joints[j].inverseBindMatrix.m[l] = read[(l%4)*1+(l/4)*4];
    read += 16;
  }
  delete[] ac;
  recalculateJoints();
}

void GLTFAnimation::init(cgltf_animation *v) {
  channels.clear();
  channels.resize(v->channels_count);
  for (int i = 0; i < channels.size(); i++) {
    GLTFAnimationChannel *l = &channels[i];
    cgltf_animation_channel *c = &v->channels[i];
    cgltf_animation_sampler *s = c->sampler;
    {
      cgltf_accessor *accessor = s->input;
      int count = accessor->count;
      float *ac = new float[count];
      int a=cgltf_accessor_unpack_floats(accessor,ac,count);
      if (a != count) {printf("error5\n"); exit(0);}
      l->times.resize(count);
      for (int i = 0; i < count; i++) l->times[i] = ac[i];
      delete[] ac;
    }
    {
      cgltf_accessor *accessor = s->output;
      int floatCount = cgltf_num_components(accessor->type);
      int overallFloats = accessor->count*floatCount;
      float *ac = new float[overallFloats];
      int a=cgltf_accessor_unpack_floats(accessor,ac,overallFloats);
      if (a != overallFloats) {printf("error6\n"); exit(0);}
      l->data.resize(accessor->count);
      for (int k = 0; k < overallFloats; k++) {
        Vector *m = &l->data[k/floatCount];
        int ki = k % floatCount;
        switch(ki) {         
          case 0: {m->x = ac[k];} break;
          case 1: {m->y = ac[k];} break;
          case 2: {m->z = ac[k];} break;
          case 3: {m->w = ac[k];} break;
        }
      }
      delete[] ac;
    }
    {
      switch(c->target_path) {
        case cgltf_animation_path_type_translation:{l->type = GLTFAnimation_type_translation;} break;
        case cgltf_animation_path_type_rotation:{l->type = GLTFAnimation_type_rotation;} break;
        case cgltf_animation_path_type_scale:{l->type = GLTFAnimation_type_scaling;} break;
        default: printf("error7"); exit(0);
      }
    }
    {
      switch(s->interpolation) {
        case cgltf_interpolation_type_linear: {l->interpolation = GLTFInterpolation_type_linear;} break;
        case cgltf_interpolation_type_step: {l->interpolation = GLTFInterpolation_type_step;} break;
        case cgltf_interpolation_type_cubic_spline: {l->interpolation = GLTFInterpolation_type_cubic;} break;
        default: printf("error8"); exit(0);
      }
    }
    {
      l->node_id = (unsigned int)c->target_node;
      l->timeMin = s->input->min[0];
      l->timeMax = s->input->max[0];
    }
  }
}

void applyTimePoint(float time, GLTFAnimation *a, GLTFSkin *s) {
  for (int i = 0; i < a->channels.size(); i++) {
    GLTFAnimationChannel *c = &a->channels[i];
    float time2 = time;
    if (time2>=c->timeMin&&time2<c->timeMax) {
      for (int k = 0; k < c->times.size()-1; k++) {
        if (time2>=c->times[k]&&time2<c->times[k+1]) {
          Vector *p0 = &c->data[k];
          Vector *p1 = &c->data[k+1];
          float t0 = c->times[k];
          float t1 = c->times[k+1];
          if (t1-t0==0.0) t1 = t0+0.1;
          double n = (time2-t0)/(t1-t0);
          if (!s->jointFromId.has(c->node_id)) {glDone();exit(0);}
          GLTFJoint *d = s->jointFromId[c->node_id];
          switch(c->interpolation) {
            case GLTFInterpolation_type_step: {
              switch(c->type) {
                case GLTFAnimation_type_scaling: {d->scaling = *p0;} break;
                case GLTFAnimation_type_rotation: {d->rotation = Quaternion(*p0);} break;
                case GLTFAnimation_type_translation: {d->translation = *p0;} break;
              }
            } break;
            case GLTFInterpolation_type_cubic:
            case GLTFInterpolation_type_linear: {
              switch(c->type) {
                case GLTFAnimation_type_scaling: {d->scaling = lerp(*p0,*p1,n);} break;
                case GLTFAnimation_type_rotation: {d->rotation = slerp(Quaternion(*p0),Quaternion(*p1),n);} break;
                case GLTFAnimation_type_translation: {d->translation = lerp(*p0,*p1,n);} break;
              }
            } break;
          }
        }
      }
    }
  }
  s->recalculateJoints();
}

void draw(GLTFPrimitive *a) {
  switch(a->primitive_type) {
    case GLTFPrimitive_type_triangle: {
      glBegin(GL_TRIANGLES);
      for (int i = 0; i < a->indices.size(); i++) {
        int j = a->indices[i];
        if (j<a->colors[0].size()) glColor4dv(&a->colors[0][j].x);
        if (j<a->texCoords[0].size()) glTexCoord2dv(&a->texCoords[0][j].x);
        if (j<a->normals[0].size()) glNormal3dv(&a->normals[0][j].x);
        if (j<a->positions[0].size()) glVertex3dv(&a->positions[0][j].x);
      }
      glEnd();
    } break;
  }
}

void draw(GLTFPrimitive *a, GLTFSkin *b) {
  Array<Vector> *colors = &a->colors[0];
  Array<Vector> *texCoords = &a->texCoords[0];
  Array<Vector> *normals = &a->normals[0];
  Array<Vector> *positions = &a->positions[0];
  Array<Vector> *weights = &a->weights[0];
  Array<Vector> *joints = &a->joints[0];
  Vector translation,normal;
  switch(a->primitive_type) {
    case GLTFPrimitive_type_triangle: {
      glBegin(GL_TRIANGLES);
      for (int i = 0; i < a->indices.size(); i++) {
        int j = a->indices[i];
        if (j<colors->size()) glColor4dv(&(*colors)[j].x);
        if (j<texCoords->size()) glTexCoord2dv(&(*texCoords)[j].x);
        normal = b->transformNormal((*normals)[j], (*joints)[j], (*weights)[j]);
        translation = b->transformPosition((*positions)[j], (*joints)[j], (*weights)[j]);
        glNormal3dv(&normal.x);
        glColor3dv(&normal.x);
        glVertex3dv(&translation.x);
      }
      glEnd();
    } break;
  }
}

void draw(GLTFMesh *a) {
  for (int i = 0; i < a->primitives.size(); i++) draw(a->primitives[i]);
}

void draw(GLTFMesh *a, GLTFSkin *b) {
  for (int i = 0; i < a->primitives.size(); i++) draw(a->primitives[i],b);
}

Vector playerPos=Vector(0,0,4);
double headAngle=3.1415927;
double headUpDown = 0;
double currentTime=0;
Vector viewDir() {
  return Vector(sin(headAngle)*cos(headUpDown),sin(headUpDown),cos(headAngle)*cos(headUpDown));
}

Vector sideDir() {
  return viewDir().zyx()*Vector(1,1,-1);
}

unsigned int cgltfTexture(cgltf_image *v, int width=-1, int height=-1) {
  cgltf_buffer_view *view = v->buffer_view;
  const char *mime_type = v->mime_type;
  unsigned int r = 0;

  const uint8_t *data = (uint8_t*)view->buffer->data+view->offset;
  const int size = view->size;

  RGBAImage image = RGBAImage::fromMemory(data, size);
  image.flipY();
  glGenTextures(1, &r);
  glBindTexture(GL_TEXTURE_2D, r);
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,image.width,image.height,0,GL_RGBA,GL_UNSIGNED_BYTE,image.data);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  image.free();

  return r;
}

int main(int argc, const char *argv[]) {
  const char *fileName = "c:/meshes/fawn/fawn~1.glt";
  cgltf_data *gltf = loadGLTF(fileName);
  if (gltf == NULL) {printf("failed\n"); exit(0);}
  GLTFMesh a;
  GLTFSkin s;
  GLTFAnimation n;
  a.init(&gltf->meshes[0]);
  s.init(&gltf->skins[0]);
  n.init(&gltf->animations[0]);
  fillJoint(&sceneNode,gltf->scene->nodes[0]);

  glVesa(640,480,32);
  //glVGA();
  installKeyboardHandler();

  glDepthFunc(GL_LEQUAL);
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);

  bool quit = false;
  while(!quit) {

    int currentKey = glNextKey();
    if (currentKey == GL_VK_ESCAPE) quit = true;
    glClearColor(0.6,0.8,1.0,0);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    
    static double lastTime = glSeconds();
    currentTime = glSeconds();
    double timeDelta = currentTime-lastTime;
    lastTime = currentTime;
    static double fpsTime = 0;
    static int fpsCounter = 0;
    fpsTime+=timeDelta;
    fpsCounter++;
    static double currentFPS=0;
    if (fpsTime>2.0) {
      fpsTime -= 2.0;
      currentFPS = fpsCounter/2.0;
      fpsCounter = 0;
    }

    double mdx,mdy;
    glNextMouseDelta(&mdx,&mdy);
  
    double w = 2*timeDelta*5;
    double q = 1*timeDelta;
    headAngle += q*mdx*0.1;
    headUpDown += clampd(q*mdy*0.1,-3.0,3.0);
    if (isKeyPressed(SCANCODE_LEFT)) headAngle += q;
    if (isKeyPressed(SCANCODE_RIGHT)) headAngle -= q;
    if (isKeyPressed(SCANCODE_UP)) playerPos += viewDir()*w;
    if (isKeyPressed(SCANCODE_DOWN)) playerPos -= viewDir()*w;
    if (isKeyPressed(SCANCODE_O)) playerPos += sideDir()*w;
    if (isKeyPressed(SCANCODE_P)) playerPos -= sideDir()*w;
    if (isKeyPressed(SCANCODE_Q)) playerPos.y += w;
    if (isKeyPressed(SCANCODE_A)) playerPos.y -= w;
  
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(90,16.0/9.0,0.1,1300.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(playerPos.x,playerPos.y,playerPos.z,playerPos.x+viewDir().x,playerPos.y+viewDir().y,playerPos.z+viewDir().z,0,1,0);
      
    applyTimePoint(fmod(currentTime,18), &n, &s);

    glTranslatef(0,0,0);
    draw(&a,&s);
    glTranslatef(-1,0,0);
    draw(&a,&s);
    glTranslatef(2,0,0);
    draw(&a,&s);

    glRefresh();
  }

  uninstallKeyboardHandler();
  glDone();
  return 0;
}
