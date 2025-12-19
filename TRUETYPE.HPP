#ifndef __TRUETYPE_HPP__
#define __TRUETYPE_HPP__

#include "stbttf.hpp"
#include "types.hpp"

#define MAX_FONTS 4

unsigned int stb_initfont(int fontIndex, const char *ttf_file_name, float fontHeight = 32.f);
void stb_clearfont(int fontIndex);
void stb_print(int fontIndex, float x, float y, const char *text, uint32_t color, float anchorX, float anchorY);

void glDrawTextTTF(int fontIndex, float xp, float yp, float zp, const float scale, const char *text, uint32_t color, float anchorX, float anchorY);
void glDrawText3DTTF(int fontIndex, float xp, float yp, float zp, const float scale, const char *text, uint32_t color, float anchorX, float anchorY);

#endif //__TRUETYPE_HPP__