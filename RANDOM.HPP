#ifndef __RANDOM_HPP__
#define __RANDOM_HPP__

__inline unsigned short randomLike(const unsigned short index) {
  int b = (int)index ^ (index * 11) ^ (index / 17) ^ (index * 1877) ^ (index * 8332) ^ (index * 173);
  b = b ^ (b << 8) ^ (b * 23);
  b >>= 3;
  return b & 0xffff;
}

__inline unsigned short rand2d(const int x, const int y) {
  return randomLike((x*17)+randomLike(y*31));
}

__inline unsigned short rand3d(const int x, const int y, const int z) {
  return randomLike((x*17)+randomLike(y*31+randomLike(z*5)));
}

__inline float randomLikef(const unsigned short index) {
    return randomLike(index)/(float)0x10000;
}

float rand2df(const float x, const float y);
float rand3df(const float x, const float y, const float z);

#endif // __RANDOM_HPP__
