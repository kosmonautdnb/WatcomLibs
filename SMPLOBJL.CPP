// simple wavefront ".obj" loader no materials supported just texture coordinates,normals and vertices
// somehow works with blender exports here..
//----
#include "smplobjl.hpp"

#define SMPL_BUFFSIZE 1024
#define SMPL_ERROR {printf("%d\n",__LINE__);getchar();delete ret; return NULL;}
static char buffer[SMPL_BUFFSIZE];
static char buffer2[SMPL_BUFFSIZE];

SMPL_File *loadObj(const String &fileName, bool triangulate) {
  SMPL_File *ret = NULL;

  FILE *in = fopen(fileName.c_str(),"r");
  if (in == NULL) return ret;
  ret = new SMPL_File();
  float x,y,z;
  HashMap<String,int> materialIds;
  int materialId = 0;
  int currentMaterial = 0;
  SMPL_Object *currentGroup = NULL;

  while(fgets(buffer,SMPL_BUFFSIZE,in)) {
    char *s = buffer;
    while(*s==' '||*s=='\t') s++;
    if (*s == '#') continue;
    if (*s == 0x0d || *s == 0x0a) continue;
    int mode = 0;
    if (*s == 'v') {
      if (*(s+1)=='t') mode = 1;
      if (*(s+1)=='n') mode = 2;
      if (*(s+1)==' '||*(s+1)=='\t') mode = 3;
    }
    if (*s == 'o') mode = 4;
    if (*s == 'l') mode = 5;
    if (*s == 's') mode = 6;
    if (*s == 'f') mode = 7;
    if (mode == 0) {
      if (sscanf(s,"usemtl %s",buffer2)==1) {
        String materialName = buffer2;
        if (materialIds.has(materialName)) 
          currentMaterial = materialIds[materialName];
        else {
          currentMaterial = materialId;
          materialIds[materialName] = materialId;
          ret->materialNames[materialId] = materialName;
          materialId++;
        }
        continue;
      }
      if (sscanf(s,"mtllib %s",buffer2)==1) {
        // not used currently.
        continue;
      }
    } else {
      switch(mode) {
        case 1: {
          if (sscanf(s,"vt %f %f",&x,&y)!=2) SMPL_ERROR
          ret->texCoords.push_back(Vector(x,y));
        } break;
        case 2: {
          if (sscanf(s,"vn %f %f %f",&x,&y,&z)!=3) SMPL_ERROR
          ret->normals.push_back(Vector(x,y,z));
        } break;
        case 3: {
          if (sscanf(s,"v %f %f %f",&x,&y,&z)!=3) SMPL_ERROR
          ret->vertices.push_back(Vector(x,y,z));
        } break;
        case 4: {
          if (sscanf(s,"o %s",buffer2)!=1) SMPL_ERROR
          if (currentGroup!=NULL) 
            currentGroup->faceEnd = ret->faces.size();
          ret->objs.push_back(SMPL_Object());
          currentGroup = &ret->objs.back();
          currentGroup->name = buffer2;
          currentGroup->faceStart = ret->faces.size();
          currentGroup->faceEnd = -1;
        } break;
        case 7: {
          char *s2 = s+1;
          int value = 0;
          int vertexId = 0;
          int componentId = 0;
          int helper[SMPL_MAXV][4];
          memset(helper,0,sizeof(helper));
          while(*s2==' '||*s2=='\t') s2++;
          while(*s2 != 0x00) {
            char c = *s2;
            if (c == '#') break;
            if (c >= '0' && c <= '9') {value*=10;value+=c-'0';}
            if (c=='/') {helper[vertexId][componentId]=value;componentId++;value=0; if (componentId>=4) SMPL_ERROR}
            if (c==' '||c=='\t'||c=='\n') {helper[vertexId][componentId]=value;value=0;componentId=0;vertexId++;while(*s2==' '||*s2=='\t'||*s2=='\n') s2++;s2--;if (vertexId>=SMPL_MAXV) SMPL_ERROR;}
            s2++;
          }
          if (!triangulate) {
            ret->faces.push_back(SMPL_Face());
            SMPL_Face *f = &ret->faces.back();
            f->m = currentMaterial;
            f->c = vertexId;
            for (int i = 0; i < vertexId; i++) {
              f->v[i] = helper[i][0]-1;
              f->t[i] = helper[i][1]-1;
              f->n[i] = helper[i][2]-1;
            }
          } else {
            for (int j = 0; j < vertexId-2; j++) {
              ret->faces.push_back(SMPL_Face());
              SMPL_Face *f = &ret->faces.back();
              f->m = currentMaterial;
              f->c = 3;
              f->v[0] = helper[0][0]-1;
              f->t[0] = helper[0][1]-1;
              f->n[0] = helper[0][2]-1;
              for (int i = 1; i < 3; i++) {
                f->v[i] = helper[i+j][0]-1;
                f->t[i] = helper[i+j][1]-1;
                f->n[i] = helper[i+j][2]-1;
              }
            }
          }
        } break;
      }
    }
  }
  if (currentGroup!=NULL) 
    currentGroup->faceEnd = ret->faces.size();
  fclose(in);

  return ret;
}
