#ifndef __CHARSET_HPP__
#define __CHARSET_HPP__

typedef struct Letter {
    int letter, width, height, postAddX, postAddY, preAddX, preAddY;
    const char* bitmap;
} Letter;

Letter* getLetter(char s);

void glDrawText(bool clip,float xp, float yp, float zp, const float scale, const char *text, unsigned int color, float anchorX, float anchorY);

#endif //__CHARSET_HPP__