#include "GL.H"
#include "textures.hpp"
#include "image.hpp"

HashMap<const char *, unsigned int> textureCache;

unsigned int glLoadTexture(const char *name, int width, int height) {
  if (textureCache.has(name)) 
    return textureCache[name];
  RGBAImage m = RGBAImage::fromFile(name);
  if (width>0||height>0) {
    m = m.getResized(width,height);
  }
  GLuint texture;
  glGenTextures(1,&texture);
  glBindTexture(GL_TEXTURE_2D,texture);
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,m.width,m.height,0,GL_RGBA,GL_UNSIGNED_BYTE,m.data);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  textureCache[name] = texture;
  return texture;
}

void glDeleteTexture(const char *name) {
  if (textureCache.has(name)) {
    glDeleteTextures(1,&textureCache[name]);
    textureCache.erase(name);
  }
}

void glClearTextureCache() {
  Array<const char *> names = textureCache.keys();
  for (int i = 0; i < names.size(); i++)
    glDeleteTextures(1,&textureCache[names[i]]);
}
