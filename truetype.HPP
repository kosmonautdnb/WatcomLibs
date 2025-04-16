#ifndef __TRUETYPE_HPP__
#define __TRUETYPE_HPP__

#include "stbttf.hpp"
#include "types.hpp"

GLuint stb_initfont(const char *ttf_file_name, float fontHeight = 32.f);
void stb_print(float x, float y, const char *text, uint32_t color, float anchorX, float anchorY);

void glDrawTextTTF(bool clip, float xp, float yp, float zp, const float scale, const char *text, uint32_t color, float anchorX, float anchorY);

#endif //__TRUETYPE_HPP__