#ifndef __VALVETXT_HPP__
#define __VALVETXT_HPP__

// this takes a big monochrome bitmap (fonts or similar)
// and makes a more pixel perfect downscale by retaining the edges
// invented by Valve somehow in the early 2000s for Half Life

void generateDistanceFieldBitmap(const unsigned int *src, int srcWidth, int srcHeight, unsigned int srcColor, unsigned char *dst, int dstWidth, int dstHeight, int box);

#endif //__VALVETXT_HPP__
