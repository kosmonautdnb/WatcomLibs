/*
  MIT License

  Copyright (c) 2026 Stefan Mader

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/
// ESBIFT (Extremely Simple Brightness Invariant Feature Tracker)
// currently not in a usable state
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <math.h>
#include <string.h>
#include <i86.h>
#include "ARRAY.HPP"
#include "IMAGE.HPP"
#include "GL.H"

// CONFIG
#define PLAYBACK 1
#define SAVE 0

#define XRES 640
#define YRES 480

const char* loadFileNames = "c:/video/1/a%04d.png";
const char* saveFileNames = "c:/video/2/a%04d.png";

#define FEATURECOUNT 256
#define GLOBAL_MIPSCALE 0.625
#define GLOBAL_IMAGESCALE (1.0/1.0)
#define DESCRIPTORSIZE 5
#define MAXMIPLEVELS 18
#define PAIRSPERMIPLEVEL 64
#define STEPCOUNT 40
#define STEPSIZE 0.25f
#define STEPINTENSITY 3 // may be kept 1
#define MIPRATIO_FRONT (0)
#define MIPRATIO_DISTANCE (0.5f) // this has to be <= 0.5 currently (pictures maybe have to be 1280x720)
#define SCALEINVARIANCE (0.25)
#define ROTATIONINVARIANCE (10.f*3.14159f/180.f)
#define ITERATIVE 1 // 0 or 1 (non iterative not working, yet)
#define LUMAR (0.25)
#define LUMAG (0.6)
#define LUMAB (0.15)
#define FORMULA(__A__) pow(__A__,2.0) // ^2 seems to be better for crisper mipmap filtering
#define OVERSAMPLE 2
#define BORDER 50
#define ALLOWEDVARIANCE 10 // man verliert das feature ausserhalb

#if !ITERATIVE
#undef MIPRATIO_DISTANCE
#define MIPRATIO_DISTANCE 1
#undef STEPCOUNT
#define STEPCOUNT 80
#undef STEPSIZE
#define STEPSIZE (0.002f*5)
#endif

#define SAFEDELETE(__P__) {if ((__P__)!=NULL) {delete (__P__); (__P__) = NULL;}}
#define CLEARPOINTERARRAY(__A__) {for(int i = 0; i < (__A__).size(); i++) {SAFEDELETE((__A__)[i]);} (__A__).clear();}

float _ifrand_(int index, float minf, float maxf) {
  float i = (float)index * 123.4567;
  void *k = (void*)(&i); srand((*((unsigned int*)k))); 
  return (float)rand() / (RAND_MAX-1) * (maxf-minf) + minf;
}

float _frandx_(float minf, float maxf) {return (float)rand() / (RAND_MAX-1) * (maxf-minf) + minf;}
float fsign(float f) {return f < 0 ? -1 : f > 0 ? 1 : 0;}

void setBiosCursor(int x, int y) {
  union REGS regs;
  struct SREGS sregs;
  regs.x.eax = 0x200;
  regs.h.bh = 0;
  regs.h.dl = x;
  regs.h.dh = y;
  int386(0x10, &regs, &regs);
}

// IMAGES

class FeatureMipMap {
public:
  FeatureMipMap() : width(0), height(0) {}
  FeatureMipMap(int w, int h) : width(w), height(h) {lumas.resize(w*h); memset(&lumas[0],0,w*h*sizeof(float));}
  int width, height;
  Array<float> lumas;
  float lookupInterpolated(float x, float y);
  float pairScale();
};

float FeatureMipMap::lookupInterpolated(float x, float y) {
  int xi = (int)x;
  int yi = (int)y;
  if (xi < 0 || yi < 0 || xi >= width-1 || yi >= height-1) return -1;
  float *l = &lumas[xi+yi*width];
  float xf = x - (float)xi;
  float yf = y - (float)yi;
  float p00 = *l;
  float p10 = *(l+1);
  float p11 = *(l+1+width);
  float p01 = *(l+width);
  float up = (p10-p00) * xf + p00;
  float down = (p11-p01) * xf + p01;
  return (down-up)*yf + up;
}

class FeatureImage {
public:
  FeatureImage() : width(0), height(0) {}
  int width, height;
  Array<unsigned int> image;
  Array<FeatureMipMap*> mipMaps;
  FeatureMipMap *mip(int level);
  void generateMipMaps(float mipScale = 0.5f);
};

FeatureMipMap *FeatureImage::mip(int level) {
  if (level >= 0 && level < mipMaps.size()) return mipMaps[level];
  return NULL;
}

void FeatureImage::generateMipMaps(float mipScale) {
  CLEARPOINTERARRAY(mipMaps);
  FeatureMipMap *lastLevel = new FeatureMipMap(width,height); mipMaps.push_back(lastLevel);
  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {
      unsigned int col = image[x+y*width]; int r = col & 255; int g = (col>>8) & 255; int b = (col>>16) & 255; 
      lastLevel->lumas[x+y*width] = FORMULA((r*LUMAR+g*LUMAG+b*LUMAB)/255.f);
    }
  }
  float *vImage = (float*)malloc(width*height*sizeof(float)); unsigned int *wImage = (unsigned int*)malloc(width*height*sizeof(unsigned int));
  while(lastLevel->width>4 && lastLevel->height>4) {
    const int sWidth = lastLevel->width; const int sHeight = lastLevel->height;
    const int dWidth = (int)(sWidth * mipScale); const int dHeight = (int)(sHeight * mipScale);
    FeatureMipMap *level = new FeatureMipMap(dWidth,dHeight); mipMaps.push_back(level);
    for (int i1 = 0; i1 < dWidth*dHeight; i1++) {vImage[i1] = 0; wImage[i1] = 0;}

    float *source = &lastLevel->lumas[0];
    for (int y = 0; y < sHeight; y++) {
      const unsigned int dy = y * dHeight/sHeight;
      for (int x = 0; x < sWidth; x++) {
        const unsigned int dx = x * dWidth/sWidth;
        vImage[dx+dy*dWidth] += source[x+y*sWidth];
        wImage[dx+dy*dWidth]++;
      }
    }

    for (int i2 = 0; i2 < dWidth*dHeight; i2++) {level->lumas[i2] = (wImage[i2]>0) ? vImage[i2] / wImage[i2] : 0;}
    lastLevel = level;
  }
  free(vImage); free(wImage);

}

char text[400] = {0};

FeatureImage *loadImage(const char *fileName, float scale = 1) {
  sprintf(text, "%s", fileName);
  RGBAImage img = RGBAImage::fromFile(fileName);
  if (img.data == NULL) return NULL;
  if (scale != 1) {
    RGBAImage img2 = img.getResized(img.width*scale,img.height*scale);
    img.free();
    img = img2;
  }
  FeatureImage *ret = new FeatureImage();
  ret->width = img.width;
  ret->height = img.height;
  ret->image.resize(img.width * img.height);
  memcpy(&ret->image[0], img.data, img.width * img.height * sizeof(unsigned int));
  img.free();
  ret->generateMipMaps(GLOBAL_MIPSCALE);
  return ret;
}

void saveImage(const char *fileName) {
  RGBAImage s;
  s.data = glFrameBuffer;
  s.width = glFrameBufferWidth;
  s.height = glFrameBufferHeight;
  s.savePNG(fileName);
}

// DESCRIPTORS

class DescriptorPositionPair {public: DescriptorPositionPair() : x0(0), y0(0), x1(0), y1(0), influence(1) {} float x0,y0,x1,y1,influence;};
class DescriptorMipLevelPairs {public:  Array<DescriptorPositionPair> pairs;}; // the x,y pairs for every mip level
class DescriptorMipLevel {public: DescriptorMipLevel() : wasInvalid(false) {}; Array<unsigned int> bits; bool wasInvalid;};
class Descriptor {public: Array<DescriptorMipLevel*> mipLevels;};  // the bits for every mip level
Array<DescriptorMipLevelPairs*> descriptorPairs;

void defaultDescriptorPairs() {
  int randomSeed = 23;
  CLEARPOINTERARRAY(descriptorPairs);
  for (int m = 0; m < MAXMIPLEVELS; m++) {
    DescriptorMipLevelPairs* level = new DescriptorMipLevelPairs();
    descriptorPairs.push_back(level);
    for (int p = 0; p < PAIRSPERMIPLEVEL; p++) {
      DescriptorPositionPair r;
      r.x0 = _ifrand_(randomSeed*1300,-1,1);
      r.y0 = _ifrand_(randomSeed*1512,-1,1);
      r.x1 = _ifrand_(randomSeed*1777,-1,1);
      r.y1 = _ifrand_(randomSeed*1332,-1,1);
      float dx = r.x1 - r.x0;
      float dy = r.y1 - r.y0;
      r.influence = sqrt(dx*dx+dy*dy);
      level->pairs.push_back(r);
      randomSeed += 3;
    }
  }
}

int getBitForPair(FeatureMipMap *mip, float x, float y, DescriptorPositionPair *pair, float pairScale, float pairAngle, float gradientX, float gradientY) {
  float x0 = x + (cos(pairAngle) * pair->x0 - sin(pairAngle) * pair->y0) * pairScale;
  float y0 = y + (sin(pairAngle) * pair->x0 + cos(pairAngle) * pair->y0) * pairScale;
  float x1 = x + (cos(pairAngle) * pair->x1 - sin(pairAngle) * pair->y1) * pairScale;
  float y1 = y + (sin(pairAngle) * pair->x1 + cos(pairAngle) * pair->y1) * pairScale;
  x0 *= mip->width; x1 *= mip->width;
  y0 *= mip->height; y1 *= mip->height;
  x0 += gradientX; y0 += gradientY;
  x1 += gradientX; y1 += gradientY;
  float a = mip->lookupInterpolated(x0,y0);
  float b = mip->lookupInterpolated(x1,y1);
  return (a<0||b<0) ? 2 : (a > b ? 1 : 0);
}

void getGradients(FeatureMipMap *mip, float x, float y, DescriptorPositionPair *pair, float pairScale, float pairAngle, float gradientX, float gradientY, float *result0, float *result1) {
  float x0 = x + (cos(pairAngle) * pair->x0 - sin(pairAngle) * pair->y0) * pairScale;
  float y0 = y + (sin(pairAngle) * pair->x0 + cos(pairAngle) * pair->y0) * pairScale;
  float x1 = x + (cos(pairAngle) * pair->x1 - sin(pairAngle) * pair->y1) * pairScale;
  float y1 = y + (sin(pairAngle) * pair->x1 + cos(pairAngle) * pair->y1) * pairScale;
  x0 *= mip->width; x1 *= mip->width;
  y0 *= mip->height; y1 *= mip->height;
  float grad0a = mip->lookupInterpolated(x0-gradientX,y0-gradientY);
  float grad0b = mip->lookupInterpolated(x0+gradientX,y0+gradientY);
  float grad1a = mip->lookupInterpolated(x1-gradientX,y1-gradientY);
  float grad1b = mip->lookupInterpolated(x1+gradientX,y1+gradientY);
  if (grad0a<0 || grad0b<0) *result0 = 0; else *result0 = grad0a - grad0b;
  if (grad1a<0 || grad1b<0) *result1 = 0; else *result1 = grad1a - grad1b;
}

float getMipLevelScale(FeatureImage *img, int level) {
  int w0 = img->mip(0)->width; // big image
  int wh = img->mip(level)->width; // small image
  return (float)w0/wh;
}

Descriptor *newDescriptor(FeatureImage *img) {
  Descriptor *d = new Descriptor();
  for (int m = 0; m < img->mipMaps.size(); m++) {
    if (m > descriptorPairs.size()) break;
    DescriptorMipLevel *level = new DescriptorMipLevel();
    level->bits.resize((descriptorPairs[m]->pairs.size()+31)/32);
    d->mipLevels.push_back(level);
  }
  return d;
}

// FEATURES

class Feature {
public:
  Feature() : x(0), y(0), scale(0), angle(0), descriptor(NULL), lifeCounter(0) {}
  float x,y;
  float scale;
  float angle;
  int lifeCounter;
  Descriptor *descriptor;
};
Array<Feature> features;

bool sampleDescriptor(FeatureImage *img, Feature *f, Descriptor *d, int onlyMipLevel = -1) {
  bool ret = true;
  int mipCount = d->mipLevels.size();
  int start = ((int)img->mipMaps.size()-1)*MIPRATIO_DISTANCE;
  int end = ((int)img->mipMaps.size()-1)*MIPRATIO_FRONT;

  for (int m = start; m >= end; m--) {
    if (m >= descriptorPairs.size()) break;
    FeatureMipMap *mipLevel = img->mip(m); if (mipLevel == NULL) break;
    DescriptorMipLevelPairs *pairs = descriptorPairs[m]; int pairCount = pairs->pairs.size();
    d->mipLevels[m]->wasInvalid = false;

    float pairScale = getMipLevelScale(img, m) * f->scale;
    float pairAngle = f->angle;

    unsigned int *bits = &d->mipLevels[m]->bits[0];
    unsigned int maxBitCount = d->mipLevels[m]->bits.size()*32;
    if (pairCount > maxBitCount) pairCount = maxBitCount;
    for (unsigned int p = 0; p < pairCount; p++) {
      int bit = getBitForPair(mipLevel, f->x, f->y, &pairs->pairs[p], pairScale, pairAngle, 0, 0);
      if (bit == 2) {bit = rand() & 1; d->mipLevels[m]->wasInvalid = true; ret = false;}
      bits[p>>5] &= ~(1<<(p&31));
      if (bit!=0) bits[p>>5] |= (1<<(p&31));
    }
  }
  return ret;
}

bool refineFeatureOneStep(FeatureImage *img, int mipLevel, Feature *f, Descriptor *d, float scaleMul, float angleAdd, float stepSize) {
  FeatureMipMap *mip = img->mip(mipLevel); if (mip == NULL) return true; if (mipLevel >= descriptorPairs.size()) return true; if (mipLevel > d->mipLevels.size()) return true;
  DescriptorMipLevelPairs *pairs = descriptorPairs[mipLevel]; int pairCount = pairs->pairs.size();
  unsigned int maxBitCount = d->mipLevels[mipLevel]->bits.size()*32;
  if (d->mipLevels[mipLevel]->wasInvalid) return false;
  if (pairCount > maxBitCount) pairCount = maxBitCount;

  unsigned int *bits = &d->mipLevels[mipLevel]->bits[0]; 
  float moveX = 0, moveY = 0;

  float scale = f->scale * scaleMul;
  float angle = f->angle + angleAdd;
  float pairScale = getMipLevelScale(img, mipLevel) * scale;

  int changes = 0, overall = 0;
  float maxDeltaX = 0, minDeltaX = 0, maxDeltaY = 0, minDeltaY = 0;
  for (unsigned int p = 0; p < pairCount; p++) {
    int bitRequired = (bits[p>>5] >> (p & 31)) & 1;
    int bitHere = getBitForPair(mip, f->x, f->y, &pairs->pairs[p], pairScale, angle, 0, 0);
    if (bitHere == 2) continue;
    if (bitHere != bitRequired) {
      float gradientX0, gradientX1; getGradients(mip, f->x, f->y, &pairs->pairs[p], scale, angle, 1, 0, &gradientX0, &gradientX1);
      float gradientY0, gradientY1; getGradients(mip, f->x, f->y, &pairs->pairs[p], scale, angle, 0, 1, &gradientY0, &gradientY1);
      float deltaX = gradientX1 - gradientX0;
      float deltaY = gradientY1 - gradientY0;
      if (bitHere == 0) {deltaX = -deltaX; deltaY = -deltaY;}
      if (deltaX > maxDeltaX) maxDeltaX = deltaX; if (deltaY > maxDeltaY) maxDeltaY = deltaY;
      if (deltaX < minDeltaX) minDeltaX = deltaX; if (deltaY < minDeltaY) minDeltaY = deltaY;
      float l = pairs->pairs[p].influence;
      deltaX = pow(fabs(deltaX), STEPINTENSITY)*fsign(deltaX);
      deltaY = pow(fabs(deltaY), STEPINTENSITY)*fsign(deltaY);
      deltaX *= l; deltaY *= l;
      moveX += deltaX;
      moveY += deltaY;
      changes++;
    }
    overall++;
  }

  // check if the movement was in opposite directions
  minDeltaX = fabs(minDeltaX); minDeltaY = fabs(minDeltaY); maxDeltaX = fabs(maxDeltaX); maxDeltaY = fabs(maxDeltaY);
  float maxX = minDeltaX > maxDeltaX ? minDeltaX : maxDeltaX; if (maxX == 0) maxX = 1;
  float maxY = minDeltaY > maxDeltaY ? minDeltaY : maxDeltaY; if (maxY == 0) maxY = 1;
  float xVariance = fabs(maxDeltaX-minDeltaX)/maxX; // this is 1 if in one direction and 0 if in both directions
  float yVariance = fabs(maxDeltaY-minDeltaY)/maxY;

  float l;
  //l = sqrt(moveX*moveX + moveY*moveY); 
  l = fabs(moveX)+fabs(moveY); // this manhatten distance seems to be better maybe because of x,y variance
  if (l != 0) l = 1.f/l;
  if (overall > 0) l *= (float)changes/overall;
  f->x += moveX * l * stepSize * xVariance;
  f->y += moveY * l * stepSize * yVariance;
  return true;
} 

bool refineFeature(FeatureImage *img, Feature *ft) {
  Feature f[OVERSAMPLE];
  for (int o = 0; o < OVERSAMPLE; o++) {
    f[o] = *ft;
    for (int m = ((int)img->mipMaps.size()-1)*MIPRATIO_DISTANCE; m >= ((int)img->mipMaps.size()-1)*MIPRATIO_FRONT; m--) {
      FeatureMipMap *mip = img->mip(m);  if (mip == NULL) continue;
      float mipMapStepSize = (float)1.f/mip->width;
      for (int s = 0; s < STEPCOUNT; s++) {
        float scaleMul = 1+_frandx_(-SCALEINVARIANCE, +SCALEINVARIANCE);
        float angleAdd = _frandx_(-ROTATIONINVARIANCE, +ROTATIONINVARIANCE);
        refineFeatureOneStep(img, m, &f[o], f[o].descriptor, scaleMul, angleAdd, mipMapStepSize * STEPSIZE);
      }
    }
  }

  ft->x = 0; ft->y = 0;
  for (int o2 = 0; o2 < OVERSAMPLE; o2++) {
    ft->x += f[o2].x;
    ft->y += f[o2].y;
  }
  ft->x /= OVERSAMPLE;
  ft->y /= OVERSAMPLE;

  float maxRad = 0;
  for (int o3 = 0; o3 < OVERSAMPLE; o3++) {
    float dx = f[o3].x - ft->x;
    float dy = f[o3].y - ft->y;
    float d = sqrt(dx*dx+dy*dy);
    if (d > maxRad) maxRad = d;
  }
  if (kbhit()) exit(0);
  return maxRad < (float)ALLOWEDVARIANCE/img->width/2.f;
}

// PROGRAM
void randomFeature(FeatureImage *img, Feature *ft) {
  int k = 10;
  Feature bestFeature = *ft; float lastBestDistance = 10000.f;
  while (k >= 0) {
    ft->x = _frandx_(0,1);
    ft->y = _frandx_(0,1);
    if (sampleDescriptor(img, ft, ft->descriptor)) {
      Feature ft2 = *ft;
      refineFeature(img, &ft2);
      float dx = ft2.x - ft->x;
      float dy = ft2.y - ft->y;
      float d = sqrt(dx*dx+dy*dy);
      if (d < lastBestDistance) {lastBestDistance = d; bestFeature = *ft;}
      break; // currently only the first hit
    }
    k--;
  }
  *ft = bestFeature;
  ft->lifeCounter = 0;
  sampleDescriptor(img, ft, ft->descriptor);
}

void placeRandomFeatures(FeatureImage *img) {
  features.clear();
  int randomSeed = 64;
  for (int i = 0; i < FEATURECOUNT; i++) {
    Feature f;
    f.scale = (float)DESCRIPTORSIZE/img->width; // pixels on base mip level
    f.angle = 0;
    f.descriptor = newDescriptor(img);
    randomFeature(img, &f);
    features.push_back(f);
    if (kbhit()) exit(0);
  }
}

void reSampleDescriptors(FeatureImage *img) {
  for (int f = 0; f < features.size(); f++) {
    Feature *ft = &features[f];
    if (ft->descriptor != NULL && !sampleDescriptor(img, ft, ft->descriptor)) randomFeature(img, ft);
  }
}

void displayImage(FeatureImage *img, int mipLevel = -1) {
  if (mipLevel < 0) {
    unsigned int *imageData = &img->image[0];
    int imageWidth = img->width;
    int imageHeight = img->height;
    for (int y = 0; y < glFrameBufferHeight; y++) {
      for (int x = 0; x < glFrameBufferWidth; x++) {
        unsigned int col = 0x00000000;
        int sx = x * imageWidth / glFrameBufferWidth;
        int sy = y * imageHeight / glFrameBufferHeight;
        col = imageData[sx+sy*imageWidth];
        glFrameBuffer[x+y*glFrameBufferWidth] = col;
      }
    }
  } else {
    FeatureMipMap *m = img->mip(mipLevel);
    float *imageData = &m->lumas[0];
    int imageWidth = m->width;
    int imageHeight = m->height;
    for (int y = 0; y < glFrameBufferHeight; y++) {
      for (int x = 0; x < glFrameBufferWidth; x++) {
        unsigned int col = 0x00000000;
        int sx = x * imageWidth / glFrameBufferWidth;
        int sy = y * imageHeight / glFrameBufferHeight;
        float a = imageData[sx+sy*imageWidth]; int ai = a * 255; if (ai > 255) ai = 255; if (ai < 0) ai = 0;
        col = ai|(ai<<8)|(ai<<16)|(ai<<24);
        glFrameBuffer[x+y*glFrameBufferWidth] = col;
      }
    }
  }
}

void drawPixel(int x, int y, unsigned int col) {
  if ((unsigned int)x < glFrameBufferWidth && (unsigned int)y < glFrameBufferHeight) {
    glFrameBuffer[x+y*glFrameBufferWidth] = col;
  }
}

void drawCircle(float x, float y, float r, unsigned int col) {
  float radx = r;
  float rady = r*16/9*YRES/XRES;
  int RADX = (int)(radx + 1);
  int RADY = (int)(rady + 1);
  for (float ry = -RADY+y; ry <= RADY+y; ry += 1) {
    for (float rx = -RADX+x; rx <= RADX+x; rx += 1) {
      double dx = (rx - x)/radx;
      double dy = (ry - y)/rady;
      double d = sqrt(dx*dx+dy*dy);
      if (d < 1) {
        if (d >= 1 - 1/r) drawPixel(rx,ry,0xff000000); else drawPixel(rx,ry,col);
      }
    }
  }
}

void displayFeatures(FeatureImage *img) {
  for (int f = 0; f < features.size(); f++) {
    srand(f*373);
    float x = features[f].x;
    float y = features[f].y;
    x *= img->width;
    y *= img->height;
    x *= (float)XRES/img->width;
    y *= (float)YRES/img->height;
    if (features[f].lifeCounter >= 2) drawCircle(x,y,3,0xff000000|rand()|rand()*rand());
    features[f].lifeCounter++;
  }
}

bool shouldDelete(FeatureImage *img, Feature *ft) {
  float x = ft->x * img->width;
  float y = ft->y * img->height;
  if (x < BORDER || x > img->width-BORDER || y < BORDER || y > img->height-BORDER) return true;
  return false;
}

void refineFeatures(FeatureImage *img) {
  for (int f = 0; f < features.size(); f++) {
    Feature *ft = &features[f];
    if (!refineFeature(img, ft) || shouldDelete(img,ft)) {
      randomFeature(img, ft);
    }
  }
}

// MAIN

int main(int argc, const char *argv[]) {
  defaultDescriptorPairs(); // required

  if (!glVesa(XRES,YRES,16)) {
    printf("Couldn't initialize graphics mode.\n");
    exit(0);
  }

  if (PLAYBACK) {
    int frame = 0;
    while(!kbhit()) {
      char buffer[200]; sprintf(buffer,saveFileNames,frame); 
      RGBAImage img = RGBAImage::fromFile(buffer);
      if (img.data == NULL) {frame++; if (frame>10) frame = 0; continue;}
      memcpy(glFrameBuffer,img.data,XRES*YRES*sizeof(unsigned int));
      img.free();
      glRefresh();
      setBiosCursor(0,0); printf("Picture:%d\n", frame);
      sprintf(buffer,saveFileNames,frame); 
      frame++;
    }
  } else {
    bool first = true;
    int frame = 0;
    while(!kbhit()) {
      char buffer[200]; sprintf(buffer,loadFileNames,frame); 
      FeatureImage *here = loadImage(buffer,GLOBAL_IMAGESCALE); if (here == NULL) {frame++;continue;}
      if (first) {first = false; placeRandomFeatures(here);}
      if (!ITERATIVE) {for (int i = 0; i < features.size(); i++) {features[i].x=0.5;features[i].y=0.5;}}
      refineFeatures(here);
      //if (ITERATIVE) {reSampleDescriptors(here);}
      displayImage(here);
      displayFeatures(here);
      glRefresh();
      setBiosCursor(0,0); printf("Picture:%d (%dx%d)\n", frame, here->width, here->height);
      sprintf(buffer,saveFileNames,frame); 
      if (SAVE) saveImage(buffer);
      frame++;
    }
  }

  glDone();

  return 0;
}
