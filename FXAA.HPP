// Post processing Anti Aliasing (FXAA) // from glsl at geeks3d.com
// currently much too slow for realtime
//---------
#ifndef __FXAA_HPP__
#define __FXAA_HPP__

void doFXAA(unsigned int *sourceBuffer, unsigned int *tempBuffer, const int width, const int height); // sourceBuffer != destBuffer

#endif //__FXAA_HPP__
