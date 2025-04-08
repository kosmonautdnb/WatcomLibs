#ifndef __CHARSET_HPP__
#define __CHARSET_HPP__

#include "types.hpp"

typedef struct Letter {
    int32_t letter, width, height, postAddX, postAddY, preAddX, preAddY;
    const char* bitmap;
} Letter;

Letter* getLetter(char s);

void glDrawText(bool clip, float xp, float yp, float zp, const float scale, const char *text, uint32_t color, float anchorX, float anchorY);

#endif //__CHARSET_HPP__