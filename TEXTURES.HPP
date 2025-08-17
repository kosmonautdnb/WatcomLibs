#ifndef __TEXTURES_HPP__
#define __TEXTURES_HPP__

#include "hashmap.hpp"

unsigned int glLoadTexture(const char *name, int width = -1, int height = -1); // returns gl handle
void glDeleteTexture(const char *name);
void glClearTextureCache();

extern HashMap<const char *, unsigned int> textureCache;

#endif //__TEXTURES_HPP__