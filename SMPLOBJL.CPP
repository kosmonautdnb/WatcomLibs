#include "smplobjl.hpp"

#define SMPL_BUFFSIZE 1024
#define SMPL_ERROR {printf("SMPLOBJL.CPP LINE:%d FILELINE:%d\n",__LINE__,line);getchar();delete ret; return NULL;}
static char buffer[SMPL_BUFFSIZE];
static char buffer2[SMPL_BUFFSIZE];

void addTexture(SMPL_Texture &t, const String &params, const String &materialLibFileName) {
  t.used = true;
  String a = params;
  {
    int i;
    for (i = a.length()-1;i>=0; i--) if (a[i]!=' '&&a[i]!=0x00&&a[i]!=0x0d&&a[i]!=0x0a&&a[i]!='\t') break;
    a.resize(i+1);
    int pre1 = a.findLast("/");
    int pre2 = a.findLast("\\");
    int pre3 = a.findLast(" ");
    int pre4 = a.findLast("\t");
    int k = pre1;
    if (pre2 > k) k = pre2;
    if (pre3 > k) k = pre3;
    if (pre4 > k) k = pre4;
    if (k>=0) a = a.substr(k+1);
  }

  {
    int pre1 = materialLibFileName.findLast("/");
    int pre2 = materialLibFileName.findLast("\\");
    int k = pre1;
    if (pre2 > pre1) k = pre2;
    if (k >= 0) a = materialLibFileName.substr(0,k)+"/"+a;
  }
  t.fileName = a;
}

static bool loadMaterialLib(SMPL_File *f,const String &fileName) {
  String currentMaterial;
  FILE *in = fopen(fileName.c_str(), "r");
  if (in == NULL) return false;
  while(fgets(buffer,SMPL_BUFFSIZE,in)) {
    char *s = buffer;
    while(*s==' '||*s=='\t') s++;
    if (*s == '#') continue;
    if (*s=='m') {
      if (sscanf(s,"map_Kd %s",buffer2)==1) {addTexture(f->materials[currentMaterial].mapDiffuse,String(buffer),fileName); continue;}
      if (sscanf(s,"map_Ka %s",buffer2)==1) {addTexture(f->materials[currentMaterial].mapAmbient,String(buffer),fileName); continue;}
      if (sscanf(s,"map_Ks %s",buffer2)==1) {addTexture(f->materials[currentMaterial].mapSpecular,String(buffer),fileName); continue;}
      if (sscanf(s,"map_Ke %s",buffer2)==1) {addTexture(f->materials[currentMaterial].mapEmissive,String(buffer),fileName); continue;}
      if (sscanf(s,"map_Ns %s",buffer2)==1) {addTexture(f->materials[currentMaterial].mapSpecularPower,String(buffer),fileName); continue;}
      if (sscanf(s,"map_Ni %s",buffer2)==1) {addTexture(f->materials[currentMaterial].mapRefract,String(buffer),fileName); continue;}
      if (sscanf(s,"map_d %s",buffer2)==1) {addTexture(f->materials[currentMaterial].mapAlpha,String(buffer),fileName); continue;}
      if (sscanf(s,"map_Tr %s",buffer2)==1) {addTexture(f->materials[currentMaterial].mapTransparency,String(buffer),fileName); continue;}
      if (sscanf(s,"map_Bump %s",buffer2)==1) {addTexture(f->materials[currentMaterial].mapBump,String(buffer),fileName); continue;}
      if (sscanf(s,"map_refl %s",buffer2)==1) {addTexture(f->materials[currentMaterial].mapReflectivity,String(buffer),fileName); continue;}
      continue;
    }
    if (*s=='n') {
      if (sscanf(s,"newmtl %s",buffer2)==1) {
        currentMaterial = buffer2;
      }
      continue;
    }
    if (*s=='d') {
      float x;
      if (sscanf(s+1,"%f",&x)==1) {
        f->materials[currentMaterial].alpha = x;
      }
      continue;
    }
    if (*s=='T'&&*(s+1)=='r') {
      float x;
      if (sscanf(s+2,"%f",&x)==1) {
        f->materials[currentMaterial].alpha = 1-x;
      }
      continue;
    }
    if (*s=='N'&&(*(s+1)!=0)) {
      float x;
      if (sscanf(s+2,"%f",&x)==1) {
        switch(*(s+1)) {
        case 's': {f->materials[currentMaterial].specularPower=x;} break;
        case 'i': {f->materials[currentMaterial].refract=x;} break;
        }
      }
      continue;
    }
    if (*s=='K'&&(*(s+1)!=0)) {
      float x,y,z;
      if (sscanf(s+2,"%f %f %f",&x,&y,&z)==3) {
        Vector c(x,y,z);
        switch(*(s+1)) {
        case 'a': {f->materials[currentMaterial].ambient = c;} break;
        case 'd': {f->materials[currentMaterial].diffuse = c;} break;
        case 's': {f->materials[currentMaterial].specular = c;} break;
        case 'e': {f->materials[currentMaterial].emissive = c;} break;
        }
      }
      continue;
    }
  }
  fclose(in);
  return true;
}

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

  int line = -1;
  while(fgets(buffer,SMPL_BUFFSIZE,in)) {
    line++;
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
        String a = buffer2;
        int pre1 = fileName.findLast("/");
        int pre2 = fileName.findLast("\\");
        int k = pre1;
        if (pre2 > pre1) k = pre2;
        if (k >= 0) a = fileName.substr(0,k)+"/"+a;
        loadMaterialLib(ret, a);
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
          int helper[SMPL_MAXV_LOAD][4];
          memset(helper,0,sizeof(helper));
          while(*s2==' '||*s2=='\t') s2++;
          while(*s2 != 0x00) {
            char c = *s2;
            if (c == '#') break;
            if (c >= '0' && c <= '9') {value*=10;value+=c-'0';}
            if (c=='/') {helper[vertexId][componentId]=value;componentId++;value=0; if (componentId>=4) SMPL_ERROR}
            if (c==' '||c=='\t'||c=='\n') {helper[vertexId][componentId]=value;value=0;componentId=0;vertexId++;while(*s2==' '||*s2=='\t'||*s2=='\n') s2++;s2--;if (vertexId>=SMPL_MAXV_LOAD) SMPL_ERROR;}
            s2++;
          }
          if (!triangulate) {
            ret->faces.push_back(SMPL_Face());
            SMPL_Face *f = &ret->faces.back();
            f->m = currentMaterial;
            f->c = vertexId;
            if (vertexId>SMPL_MAXV) SMPL_ERROR;
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

void loadTexture(TextureLoad_t functor, SMPL_Texture &t, const String &type) {
  if (t.used && t.glHandle == 0) {
    t.glHandle = functor(t.fileName, type);
  }
}

void SMPL_File::loadTextures(TextureLoad_t functor) {
  Array<String> keys = materials.keys();
  for (int i = 0; i < keys.size(); i++) {
    SMPL_Material &m = materials[keys[i]];
    loadTexture(functor,m.mapDiffuse,"map_Kd");
    loadTexture(functor,m.mapAmbient,"map_Ka");
    loadTexture(functor,m.mapSpecular,"map_Ks");
    loadTexture(functor,m.mapEmissive,"map_Ke");
    loadTexture(functor,m.mapSpecularPower,"map_Ns");
    loadTexture(functor,m.mapRefract,"map_Ni");
    loadTexture(functor,m.mapAlpha,"map_d");
    loadTexture(functor,m.mapTransparency,"map_Tr");
    loadTexture(functor,m.mapBump,"map_Bump");
    loadTexture(functor,m.mapReflectivity,"map_refl");
  }
}

String getPlyWord(char **r) {
  char *s = *r;
  while((*s!=0x00)&&((*s==' ')||(*s=='\t')||(*s==0x0d)||(*s==0x0a))) {
    s++;
  }
  char *start = s;
  while((*s!=0x00)&&(!((*s==' ')||(*s=='\t')||(*s==0x0d)||(*s==0x0a)))) {
    s++;
  }
  char *end = s;
  *r = end;
  String ret;
  ret.resize(end-start);
  memcpy(ret.data,start,ret.size());
  return toLower(ret);
}

SMPL_Ply *loadPly(const String &fileName) {
  SMPL_Ply *ret = NULL;
  FILE *in = fopen(fileName.c_str(), "r");
  if (in == NULL) return ret;
  ret = new SMPL_Ply();
  bool isPly = false;
  bool format = false;
  String formatString;
  String formatVersion;
  while(fgets(buffer,SMPL_BUFFSIZE,in)) {
    char *s = buffer;
    while(*s==' '||*s=='\t') s++;
    String w = getPlyWord(&s);
    if (w=="comment") {
      continue;
    }
    if ((!isPly)&&(w=="ply")) {
        isPly = true;
    }
    if (!isPly) {
      delete[] ret; ret = NULL;
      return ret;
    }
    if ((!format)&&(w=="format")) {
      format = true;
      formatString = getPlyWord(&s);
      formatVersion = getPlyWord(&s);
    }
    if (w=="element") {
      String element = getPlyWord(&s);
      String count = getPlyWord(&s);
      if (element=="vertex") {
        ret->vertexCount = count.toInt();
      }
      if (element=="face") {
        ret->faceCount = count.toInt();
      }
    }
    if (w=="property") {
      String type = getPlyWord(&s); // not used, always float
      if (type == "list") 
        continue; // not evaluated always int and vertexCount,vertex0,vertex1,vertex2,...
      String name = getPlyWord(&s);
      if (name == "x") ret->xProperty=ret->vertexProperties.size();
      if (name == "y") ret->yProperty=ret->vertexProperties.size();
      if (name == "z") ret->zProperty=ret->vertexProperties.size();
      ret->vertexProperties.push_back(name);
      ret->vertexStride = ret->vertexProperties.size();
    }
    if (w=="end_header") {
      break;
    }
  }
  {for (int i = 0; i < ret->vertexCount; i++) {
    if (!fgets(buffer,SMPL_BUFFSIZE,in)) break;
    char *s = buffer;
    for (int j = 0; j < ret->vertexStride; j++) {
      String name = getPlyWord(&s);
      ret->vertices.push_back(name.toFloat());
    }
  }}
  {for (int i = 0; i < ret->faceCount; i++) {
    if (!fgets(buffer,SMPL_BUFFSIZE,in)) break;
    char *s = buffer;
    String name = getPlyWord(&s);
    int vertexCount=-1;
    ret->faceStartIndex.push_back(ret->faceIndices.size());
    while(!name.empty()) {
      if (vertexCount==-1) {
        vertexCount=name.toInt();
        ret->faceIndexCount.push_back(vertexCount);
      } else {
        ret->faceIndices.push_back(name.toInt());
      }
      name = getPlyWord(&s);
    }
  }}
  return ret;
}
