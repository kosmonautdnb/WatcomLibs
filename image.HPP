#ifndef __IMAGE_HPP__
#define __IMAGE_HPP__

#include "types.hpp"

#include "stbimgwr.hpp"
#include "stbimage.hpp"
#include "stbimgrs.hpp"

class RGBAImage {
public:
  int32_t width;
  int32_t height;
  uint32_t *data;

  RGBAImage(int width = -1, int height = -1, uint32_t *data = 0) {
    this->width = width;
    this->height = height;
    this->data = data;
  }

  void free() {
    delete[] data;
    data = 0;
    width = -1;
    height = -1;
  }

  // JPG baseline & progressive (12 bpc/arithmetic not supported, same as stock IJG lib)
  // PNG 1/2/4/8/16-bit-per-channel
  // TGA (not sure what subset, if a subset)
  // BMP non-1bpp, non-RLE
  // PSD (composited view only, no extra channels, 8/16 bit-per-channel)
  // GIF (*comp always reports as 4-channel)
  // PIC (Softimage PIC)
  // PNM (PPM and PGM binary only)
  // HDR
  static RGBAImage fromFile(const char *fileName);
  static RGBAImage fromMemory(uint8_t *fileData, uint32_t fileLength);

  bool savePNG(const char *fileName);
  bool saveJPEG(const char *fileName, int quality = 100);
  bool saveTGA(const char *fileName);
  bool saveBMP(const char *fileName);

  RGBAImage getResized(int width, int height);
};

#endif //__IMAGE_HPP__