#include "image.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stbimgwr.hpp"
#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_SIMD
#include "stbimage.hpp"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stbimgrs.hpp"


RGBAImage RGBAImage::fromFile(const char *fileName) {
  RGBAImage r;
  int width, height, channels;
  r.data = (uint32_t*)stbi_load(fileName, &width, &height, &channels, 4);
  r.width = r.data == NULL ? -1 : width;
  r.height = r.data == NULL ? -1 : height;
  return r;
}

RGBAImage RGBAImage::fromMemory(const uint8_t *fileData, uint32_t fileLength) {
  RGBAImage r;
  int width, height, channels;
  r.data = (uint32_t*)stbi_load_from_memory(fileData, fileLength, &width, &height, &channels, 4);
  r.width = r.data == NULL ? -1 : width;
  r.height = r.data == NULL ? -1 : height;
  return r;
}

bool RGBAImage::savePNG(const char *fileName) {
  return stbi_write_png(fileName, width, height, 4, data, width * 4) != 0;
}

bool RGBAImage::saveJPEG(const char *fileName, int quality) {
  return stbi_write_jpg(fileName, width, height, 4, data, quality) != 0;
}

bool RGBAImage::saveTGA(const char *fileName) {
  return stbi_write_tga(fileName, width, height, 4, data) != 0;
}

bool RGBAImage::saveBMP(const char *fileName) {
  return stbi_write_bmp(fileName, width, height, 4, data) != 0;
}

RGBAImage RGBAImage::getResized(int width, int height) {
  RGBAImage r;
  r.width = width;
  r.height = height;
  r.data = (uint32_t*)stbir_resize_uint8_linear( (uint8_t*)data,  this->width,  this->height, this->width * 4, NULL, r.width, r.height, r.width * 4, STBIR_RGBA );
  return r;
}

void RGBAImage::flipY() {
  unsigned int *flipped = new unsigned int[width*height];
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      flipped[x+(height-1-y)*width] = data[x+y*width];
    }
  }
  delete[] data;
  data = flipped;
}

void RGBAImage::setAlpha(int alpha) {
  unsigned int alpha2 = alpha << 24;
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      data[x+y*width]=(data[x+y*width]&0x00ffffff)|alpha2;
    }
  }
}
