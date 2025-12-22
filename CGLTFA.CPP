#include "cgltfa.hpp"
#include "gl.h"
#include "image.hpp"

void GLTFA_Mesh::buildBoundingBox() {
  bool first = true;
  for (int i = 0; i < primitives.size(); i++) {
    GLTFA_Primitive *p = primitives[i];
    Array<Vector> &k = p->positions[0];
    for (int j = 0; j < k.size(); j++) {
      Vector &k2 = k[j];
      if (first||k2.x<boundingBoxMin.x) boundingBoxMin.x=k2.x;
      if (first||k2.y<boundingBoxMin.y) boundingBoxMin.y=k2.y;
      if (first||k2.z<boundingBoxMin.z) boundingBoxMin.z=k2.z;
      if (first||k2.x>boundingBoxMax.x) boundingBoxMax.x=k2.x;
      if (first||k2.y>boundingBoxMax.y) boundingBoxMax.y=k2.y;
      if (first||k2.z>boundingBoxMax.z) boundingBoxMax.z=k2.z;
      first = false;
    }
  }
}

GLTFA_Node::GLTFA_Node() {
  id = 0;
  parent_id = 0;
  matrixUpdateId = 0;
  meshId = 0;
  skinId = 0;
  hasScaling = false;
  hasRotation = false;
  hasTranslation = false;
  hasMatrix = false;
  color = Vector(1,1,1,1);
}


bool GLTFA_File::load(const char *fileName, const float textureScale) { // load .glt or .glb file (glb also features embedded textures etc..)
  cgltf_options options;
  cgltf_data *data=NULL;
  memset(&options,0,sizeof(cgltf_options));
  cgltf_result result = cgltf_parse_file(&options, fileName, &data);
  if (result == cgltf_result_success) {
    result = cgltf_load_buffers(&options, data, fileName);
    if (result == cgltf_result_success) {
      result = cgltf_validate(data);
    }
  }
  if (result == cgltf_result_success) {
    gltfName = fileName;
    defaultScene = (unsigned int)data->scene;
    initMaterials(data->materials,data->materials_count,textureScale);
    initMeshes(data->meshes,data->meshes_count);
    initNodes(data->nodes,data->nodes_count);
    initSkins(data->skins,data->skins_count);
    initScenes(data->scenes, data->scenes_count);
    initAnimations(data->animations, data->animations_count);
    recalculateNodes();
    cgltf_free(data);
    return true;
  }
  cgltf_free(data);
  return false;
}

void GLTFA_File::free() {
#define __FREEMAPPOINTERS__(__a__) {Array<unsigned int> k=(__a__).keys();for (int i = 0; i < k.size(); i++) if ((__a__)[k[i]]!=NULL) delete (__a__)[k[i]]; (__a__).clear();}
#define __FREEARRAYPOINTERS__(__a__) {for (int i = 0; i < (__a__).size(); i++) if ((__a__)[i]!=NULL) delete (__a__)[i]; (__a__).clear();}
  __FREEMAPPOINTERS__(gltfMaterials);
  __FREEMAPPOINTERS__(gltfMeshes);
  __FREEMAPPOINTERS__(gltfNodes);
  __FREEMAPPOINTERS__(gltfSkins);
  __FREEMAPPOINTERS__(gltfScenes);
  __FREEARRAYPOINTERS__(gltfAnimations);
#undef __FREEMAPPOINTERS__
#undef __FREEARRAYPOINTERS__
}

unsigned int GLTFA_File::loadTexture(cgltf_image *v, double texSizeMul) {
  cgltf_buffer_view *view = v->buffer_view;
  const char *mime_type = v->mime_type;
  unsigned int r = 0;
  const uint8_t *data = (uint8_t*)view->buffer->data+view->offset;
  const int size = view->size;
  RGBAImage image = RGBAImage::fromMemory(data, size);
  if (image.data == NULL) {
    return 0;
  }
  image.flipY();
  if (texSizeMul != 1) {
    RGBAImage image2 = image.getResized(image.width*texSizeMul,image.height*texSizeMul);
    image.free();
    image = image2;
  }
  if (textureCallback != NULL) {unsigned int r = textureCallback(&image);image.free();return r;}
  glGenTextures(1, &r);
  glBindTexture(GL_TEXTURE_2D, r);
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,image.width,image.height,0,GL_RGBA,GL_UNSIGNED_BYTE,image.data);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  image.free();
  return r;
}
 
void GLTFA_File::initMaterials(cgltf_material* materials, int count, double texSizeMul)
{
  gltfMaterials.clear();
  for (int i = 0; i < count; i++) {
    GLTFA_Material *material = new GLTFA_Material();
    material->name = materials[i].name==NULL?"":materials[i].name;
    material->materialId = (unsigned int)&materials[i];
    material->texWidth = 0;
    material->texHeight = 0;
    material->texHandle = 0;
    if (materials[i].has_pbr_metallic_roughness) {
      cgltf_pbr_metallic_roughness *r = &materials[i].pbr_metallic_roughness;
      cgltf_texture_view *b = &r->base_color_texture;
      if (b == NULL || b->texture == NULL) b = &materials[i].emissive_texture;
      cgltf_texture* t = b->texture;
      if (t!=NULL) {
        cgltf_image* m = t->image;
        if (m!=NULL) {
          material->texHandle = loadTexture(m, texSizeMul);
          if (material->texHandle!=0) {
            cgltf_sampler* s = t->sampler;
            if (s!=NULL) {
              glBindTexture(GL_TEXTURE_2D,material->texHandle);
              switch(s->wrap_s) {
                case cgltf_wrap_mode_clamp_to_edge:  {glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);} break;
                case cgltf_wrap_mode_mirrored_repeat:  {glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);} break;
                case cgltf_wrap_mode_repeat: {glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);} break;
              }
              switch(s->wrap_t) {
                case cgltf_wrap_mode_clamp_to_edge:  {glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);} break;
                case cgltf_wrap_mode_mirrored_repeat:  {glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);} break;
                case cgltf_wrap_mode_repeat: {glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);} break;
              }
              switch(s->mag_filter) {
                case cgltf_filter_type_nearest: {glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);} break;
                case cgltf_filter_type_linear: {glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);} break;
                case cgltf_filter_type_nearest_mipmap_nearest: {glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);} break;
                case cgltf_filter_type_linear_mipmap_nearest: {glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);} break;
                case cgltf_filter_type_nearest_mipmap_linear: {glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);} break;
                case cgltf_filter_type_linear_mipmap_linear: {glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);} break;
              }
              switch(s->min_filter) {
                case cgltf_filter_type_nearest: {glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);} break;
                case cgltf_filter_type_linear: {glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);} break;
                case cgltf_filter_type_nearest_mipmap_nearest: {glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);} break;
                case cgltf_filter_type_linear_mipmap_nearest: {glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);} break;
                case cgltf_filter_type_nearest_mipmap_linear: {glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);} break;
                case cgltf_filter_type_linear_mipmap_linear: {glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);} break;
              }
            }
            material->texWidth = glGetTextureWidth(material->texHandle);
            material->texHeight = glGetTextureHeight(material->texHandle);
          }
        }
      }
    }
    gltfMaterials[material->materialId] = material;
  }
}
  
void GLTFA_File::initMesh(GLTFA_Mesh *m, cgltf_mesh *v) {
  m->meshId = (unsigned int)v;
  // load primitive data
  m->primitives.clear();
  m->primitives.resize(v->primitives_count);
  m->name = v->name==NULL?"":v->name;
  for (int i = 0; i < m->primitives.size(); i++) {
    cgltf_primitive *p = &v->primitives[i]; 
    GLTFA_Primitive *r = new GLTFA_Primitive(); 
    m->primitives[i] = r;
    // triangles?
    switch(p->type) {
      case cgltf_primitive_type_triangles: {r->primitive_type = GLTFA_Primitive_type_triangle;} break;
      default: {r->primitive_type = GLTFA_Primitive_type_unsupported;} break;
    }
    // indices
    r->indices.resize(p->indices->count);
    r->materialId = (unsigned int)p->material;
    int a = cgltf_accessor_unpack_indices(p->indices, &r->indices[0], 4, p->indices->count); if (a != p->indices->count) {printf("error1\n");exit(0);}
    // attributes
    for (int j = 0; j < p->attributes_count; j++) {
      const cgltf_attribute *attr = &p->attributes[j];
      HashMap<int,Array<Vector> > *dest = NULL;
      bool yflip = false; // todo: strange that this is needed here
      switch(attr->type) {
        case cgltf_attribute_type_position: { dest = &r->positions; } break;
        case cgltf_attribute_type_normal: { dest = &r->normals; } break;
        case cgltf_attribute_type_color: { dest = &r->colors; } break;
        case cgltf_attribute_type_texcoord: { dest = &r->texCoords; yflip=true;  } break;
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
            case 1: {l->y = yflip ? 1-ac[k]:ac[k];} break;
            case 2: {l->z = ac[k];} break;
            case 3: {l->w = ac[k];} break;
          }
        }
        delete[] ac;
      }
    }
  }
}

void GLTFA_File::initMeshes(cgltf_mesh* meshes, int count) {
  gltfMeshes.clear();
  for (int i = 0; i < count; i++) {
    GLTFA_Mesh *mesh = new GLTFA_Mesh();
    initMesh(mesh,&meshes[i]);
    mesh->buildBoundingBox(); // maybe exclude this
    gltfMeshes[mesh->meshId] = mesh;
  }
}

void GLTFA_File::initNodes(cgltf_node *nodes, int count) {
  gltfNodes.clear();
  for (int i = 0; i < count; i++) {
    GLTFA_Node *node = new GLTFA_Node();
    fillNode(node,&nodes[i]);
    gltfNodes[node->id] = node;
  }
}


void GLTFA_File::initSkins(cgltf_skin *skins, int count) {  // initNodes needed before
  gltfSkins.clear();
  for (int o = 0; o < count; o++) {
    cgltf_skin *v = &skins[o];
    GLTFA_Skin *k = new GLTFA_Skin();
    k->id = (unsigned int)v;
    gltfSkins[k->id] = k;
    k->joints.clear();
    k->joints.resize(v->joints_count);
    for (int i = 0; i < k->joints.size(); i++) {
      unsigned int id = (unsigned int)v->joints[i];
      if (gltfNodes.has(id))
        k->joints[i] = gltfNodes[id];
    }
    cgltf_accessor *accessor = v->inverse_bind_matrices;
    int count = accessor->count * 16;
    float *ac = new float[count];
    int a=cgltf_accessor_unpack_floats(accessor,ac,count);
    if (a != count||count!=k->joints.size()*16) {printf("error%d,%d\n",a,count);exit(0);}
    float *read = ac;
    for (int j = 0; j < k->joints.size(); j++) {
      for (int l = 0; l < 16; l++) k->joints[j]->inverseBindMatrix.m[l] = read[(l%4)*1+(l/4)*4];
      read += 16;
    }
    delete[] ac;
  }
}

void GLTFA_File::initScenes(cgltf_scene *scenes, int count) {  // initNodes needed before
  gltfScenes.clear();
  for (int i = 0; i < count; i++) {
    cgltf_scene *v = &scenes[i];
    GLTFA_Scene *k = new GLTFA_Scene;
    k->id = (unsigned int)v;
    gltfScenes[k->id] = k;
    for (int j = 0; j < v->nodes_count; j++) {
      unsigned int id = (unsigned int)v->nodes[j];
      if (gltfNodes.has(id))
        k->nodes.push_back(gltfNodes[id]);
    }
  }
}

void GLTFA_File::initAnimations(cgltf_animation *animations, int count) {
  gltfAnimations.clear();
  for (int o = 0; o < count; o++)
  {
    cgltf_animation *v = &animations[o];
    GLTFA_Animation *k = new GLTFA_Animation;
    gltfAnimations.push_back(k);
    k->channels.clear();
    k->channels.resize(v->channels_count);
    for (int i = 0; i < k->channels.size(); i++) {
      GLTFA_AnimationChannel *l = &k->channels[i];
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
          case cgltf_animation_path_type_translation:{l->type = GLTFA_Animation_type_translation;} break;
          case cgltf_animation_path_type_rotation:{l->type = GLTFA_Animation_type_rotation;} break;
          case cgltf_animation_path_type_scale:{l->type = GLTFA_Animation_type_scaling;} break;
          case cgltf_animation_path_type_weights:{l->type = GLTFA_Animation_type_weights;} break; // not implemented
          default: printf("error7"); exit(0);
        }
      }
      {
        switch(s->interpolation) {
          case cgltf_interpolation_type_linear: {l->interpolation = GLTFA_Interpolation_type_linear;} break;
          case cgltf_interpolation_type_step: {l->interpolation = GLTFA_Interpolation_type_step;} break;
          case cgltf_interpolation_type_cubic_spline: {l->interpolation = GLTFA_Interpolation_type_cubic;glDone();printf("cubic not supported\n");exit(0);} break;
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
}

Matrix GLTFA_File::getMatrixHere(GLTFA_Node *j) const {
  // m = t * r * s
  Matrix matrixHere;
  if (j->hasMatrix) {
    matrixHere = j->matrix;
  } else {
    if (j->hasScaling) {
      matrixHere.scale3(j->scaling);
    }
    if (j->hasRotation) {
      matrixHere = j->rotation.rotationMatrix()*matrixHere;
    }
    if (j->hasTranslation) {
      matrixHere.setColumn(3,j->translation);
    }
  }
  return matrixHere;
}
  
Matrix GLTFA_File::getMatrix(unsigned int nodeId, unsigned int updateCycle) {
  if (gltfNodes.has(nodeId)) {
    GLTFA_Node *j = gltfNodes[nodeId];
    if (j->matrixUpdateId==updateCycle) 
      return j->matrixHereWithParent;
    j->matrixHereWithParent = j->parent_id!=0?(getMatrix(j->parent_id,updateCycle)*getMatrixHere(j)):getMatrixHere(j);
    j->matrixUpdateId = updateCycle;
    return j->matrixHereWithParent;
  }
  return Matrix(); // should never happen
}
  
void GLTFA_File::fillNode(GLTFA_Node *j, cgltf_node *k) const {
  j->name = k->name;
  j->id = (unsigned int)k;
  j->parent_id = (unsigned int)k->parent;
  j->child_ids.resize(k->children_count);
  for (int l = 0; l < j->child_ids.size(); l++) {
    j->child_ids[l] = (unsigned int)k->children[l];
  }
  j->meshId = (unsigned int)k->mesh;
  j->skinId = (unsigned int)k->skin;
  j->hasScaling = k->has_scale; j->scaling = Vector(k->scale[0],k->scale[1],k->scale[2]);
  j->hasRotation = k->has_rotation; j->rotation = Quaternion(k->rotation[0],k->rotation[1],k->rotation[2],k->rotation[3]);
  j->hasTranslation = k->has_translation; j->translation = Vector(k->translation[0],k->translation[1],k->translation[2]);
  j->hasMatrix = k->has_matrix; for (int m = 0; m < 16; m++) j->matrix.m[m] = k->matrix[m];
}

void GLTFA_File::recalculateNodes() {
  currentAnimCycle++;
  Array<unsigned int> k = gltfNodes.keys();
  for (int j = 0; j < k.size(); j++) {
    gltfNodes[k[j]]->matrixUpdateId = 0;
  }
  for (int i = 0; i < k.size(); i++) {
    GLTFA_Node *j = gltfNodes[k[i]];
    j->finalMatrix = getMatrix(j->id,1)*j->inverseBindMatrix; // inverseBindMatrix is identity for normal nodes
    j->finalNormalMatrix = transpose(inverse(j->finalMatrix));
  }
}

void GLTFA_File::applyAnimation(float time, GLTFA_Animation *a) {
  for (int i = 0; i < a->channels.size(); i++) {
    GLTFA_AnimationChannel *c = &a->channels[i];
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
          if (!gltfNodes.has(c->node_id)) {continue;}
          GLTFA_Node *d = gltfNodes[c->node_id];
          switch(c->interpolation) {
            case GLTFA_Interpolation_type_step: {
              switch(c->type) {
                case GLTFA_Animation_type_scaling: {d->scaling = *p0;} break;
                case GLTFA_Animation_type_rotation: {d->rotation = Quaternion(*p0);} break;
                case GLTFA_Animation_type_translation: {d->translation = *p0;} break;
              }
            } break;
            case GLTFA_Interpolation_type_cubic:
            case GLTFA_Interpolation_type_linear: {
              switch(c->type) {
                case GLTFA_Animation_type_scaling: {d->scaling = lerp(*p0,*p1,n);} break;
                case GLTFA_Animation_type_rotation: {d->rotation = slerp(Quaternion(*p0),Quaternion(*p1),n);} break;
                case GLTFA_Animation_type_translation: {d->translation = lerp(*p0,*p1,n);} break;
              }
            } break;
          }
        }
      }
    }
  }
  recalculateNodes();
}

int gltfaPaintedTriangles = 0;
bool gltfaForceNoSkinning = false;

void GLTFA_File::drawPrimitive(GLTFA_Primitive *a, GLTFA_Skin *b, const Vector &color) {
  glColor4dv(&color.x);

  Vector *colorsV = NULL;
  Vector *texCoordsV = NULL;
  Vector *normalsV = NULL;
  Vector *positionsV = NULL;
  Vector *weightsV = NULL;
  Vector *jointsV = NULL;

  if (a->colors.has(0)) colorsV = &a->colors[0][0];
  if (a->texCoords.has(0)) texCoordsV = &a->texCoords[0][0];
  if (a->normals.has(0)) {normalsV = &a->normals[0][0];}
  if (a->positions.has(0)) positionsV = &a->positions[0][0];
  if (a->weights.has(0)) weightsV = &a->weights[0][0];
  if (a->joints.has(0)) jointsV = &a->joints[0][0];

  if (b == NULL) {
    switch(a->primitive_type) {
      case GLTFA_Primitive_type_triangle: {
        glBegin(GL_TRIANGLES);
        for (int i = 0; i < a->indices.size(); i++) {
          int j = a->indices[i];
          if (colorsV) glColor4d(colorsV[j].x*color.x,colorsV[j].y*color.y,colorsV[j].z*color.z,colorsV[j].w*color.w);
          if (texCoordsV) glTexCoord2dv(&texCoordsV[j].x);
          if (normalsV) glNormal3dv(&normalsV[j].x);
          if (positionsV) glVertex3dv(&positionsV[j].x);
        }
        glEnd();
      } break;
    }
  } else {
    Array<Vector> *normals0 = &a->normals0;
    Array<Vector> *positions0 = &a->positions0;
    Vector k; k.x = currentAnimCycle;
    switch(a->primitive_type) {
      case GLTFA_Primitive_type_triangle: {
        glBegin(GL_TRIANGLES);
        for (int i = 0; i < a->indices.size(); i++) {
          int j = a->indices[i];
          if (colorsV) glColor4d(colorsV[j].x*color.x,colorsV[j].y*color.y,colorsV[j].z*color.z,colorsV[j].w*color.w);
          if (texCoordsV) glTexCoord2dv(&texCoordsV[j].x);
          if (normalsV) {
            if (j >= normals0->size()) normals0->resize(j+1);
            Vector *z = &(*normals0)[j];
            if (z->w != k.x) {
              *z = b->transformNormal(normalsV[j], jointsV[j], weightsV[j]); 
              z->w = k.x;
            }
            glNormal3dv(&z->x);
          }
          if (positionsV) {
            if (j >= positions0->size()) positions0->resize(j+1);
            Vector *z = &(*positions0)[j];
            if (z->w != k.x) {
              *z = b->transformPosition(positionsV[j], jointsV[j], weightsV[j]);
              z->w = k.x;
            }
            glVertex3dv(&z->x);
          }
        }
        glEnd();
      } break;
    }
  }

  gltfaPaintedTriangles += a->indices.size()/3;
}
  
void GLTFA_File::drawMesh(GLTFA_Mesh *a) {
  for (int i = 0; i < a->primitives.size(); i++) drawPrimitive(a->primitives[i]);
}
  
void GLTFA_File::drawMesh(GLTFA_Mesh *a, GLTFA_Skin *b) {
  for (int i = 0; i < a->primitives.size(); i++) drawPrimitive(a->primitives[i],b);
}

void GLTFA_File::drawMesh(unsigned int meshId, double *matrix, unsigned int skinId, const Vector &color) {
  if (!gltfMeshes.has(meshId)) return;
  if (skinId != 0 && (!gltfSkins.has(skinId))) return;
  GLTFA_Skin *s = NULL; if (skinId != 0) s = gltfSkins[skinId];
  GLTFA_Mesh *m = gltfMeshes[meshId];
  for (int i = 0; i < m->primitives.size(); i++) {
    if (gltfMaterials.has(m->primitives[i]->materialId)) {
      GLTFA_Material *a = gltfMaterials[m->primitives[i]->materialId];
      if (a->texHandle!=0) {
        glEnable(GL_TEXTURE_2D);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,a->texHandle);
        glPushMatrix();
        glMultMatrixd(matrix);
        drawPrimitive(m->primitives[i],s,color);
        glPopMatrix();
        glDisable(GL_TEXTURE_2D);
      } 
      else {
        if (displayWithoutTexture) {
          glPushMatrix(); // must also be painted, just doesn't have valid base texture
          glMultMatrixd(matrix);
          drawPrimitive(m->primitives[i],s);
          glPopMatrix();
        }
      }
    } else {
      glPushMatrix();
      glMultMatrixd(matrix);
      drawPrimitive(m->primitives[i],s,color);
      glPopMatrix();
    }
  }
}

void GLTFA_File::drawNode(unsigned int nodeId) {
  // somehow recursive functions don't work in watcom c here
  Array<unsigned int> stack;
  stack.push_back(nodeId);
  while(!stack.empty()) {
    nodeId = stack.back();
    stack.pop_back();
    if (gltfNodes.has(nodeId)) {
      GLTFA_Node *n = gltfNodes[nodeId];
      bool found = false;
      for (int j = 0; j < skipNodeNames.size(); j++) {
        if (n->name == skipNodeNames[j]) {
          found = true;
          break;
        }
      }
      if (!found) {
        if (n->meshId != 0) {
          if (n->skinId == 0 || gltfaForceNoSkinning) {
            drawMesh(n->meshId, n->finalMatrix.m,0,n->color);
          } else {
            Matrix identity; // not sure, but this is already in the skinning matrices
            drawMesh(n->meshId, identity.m,n->skinId,n->color);
          }
        }
        for (int i = n->child_ids.size()-1; i >= 0; i--) {
          stack.push_back(n->child_ids[i]);
        }
      }
    }
  }
}

void GLTFA_File::drawScene(unsigned int sceneId) {
  if (!gltfScenes.has(sceneId)) 
    return;
  GLTFA_Scene *s = gltfScenes[sceneId];
  for (int i = 0; i < s->nodes.size(); i++) {
    drawNode(s->nodes[i]->id);
  }
}

unsigned int GLTFA_File::hitTestNode(unsigned int nodeId, HITTESTFUNCTION t, Vector &nodeHitPoint, double &lastHitDistance) {
  // somehow recursive functions don't work in watcom c here
  Array<unsigned int> stack;
  stack.push_back(nodeId);
  unsigned int nodeHit = 0;
  while(!stack.empty()) {
    nodeId = stack.back();
    stack.pop_back();
    if (gltfNodes.has(nodeId)) {
      GLTFA_Node *n = gltfNodes[nodeId];
      bool found = false;
      for (int j = 0; j < skipNodeNames.size(); j++) {
        if (n->name == skipNodeNames[j]) {
          found = true;
          break;
        }
      }
      if (!found) {
        if (n->meshId != 0) {
          if (n->skinId == 0) {
            if (gltfMeshes.has(n->meshId)) {
              GLTFA_Mesh *m = gltfMeshes[n->meshId];
              Vector &boundingBoxMin = m->boundingBoxMin;
              Vector &boundingBoxMax = m->boundingBoxMax;
              Vector boundingBoxCenter = (boundingBoxMin+boundingBoxMax)/2.0;
              Vector boundingBoxScale = (boundingBoxCenter-boundingBoxMin);
              Matrix m1; m1*=n->finalMatrix;m1.translate(boundingBoxCenter);m1.scale3(boundingBoxScale);
              Vector hitPoint; double hitDistance;
              if (t(m1,hitPoint,hitDistance,*n)) {
                if (hitDistance>=0&&hitDistance<lastHitDistance) {
                  lastHitDistance=hitDistance;
                  nodeHit = nodeId;
                  nodeHitPoint = hitPoint;
                }
              }
            }
          } else {
          }
        }
      }
      for (int i = n->child_ids.size()-1; i >= 0; i--) {
        stack.push_back(n->child_ids[i]);
      }
    }
  }
  return nodeHit;
}

unsigned int GLTFA_File::hitTestForScene(unsigned int sceneId, HITTESTFUNCTION t, Vector &hitPoint) {
  if (!gltfScenes.has(sceneId)) 
    return 0;
  GLTFA_Scene *s = gltfScenes[sceneId];
  double lastHitDistance = 10000000000.0;
  unsigned int nodeHit = 0;
  for (int i = 0; i < s->nodes.size(); i++) {
    unsigned int node = hitTestNode(s->nodes[i]->id, t, hitPoint, lastHitDistance);
    if (node != 0) nodeHit = node;
  }
  return nodeHit;
}

