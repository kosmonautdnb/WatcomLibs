// Harris Noble corner detection maybe like in GoodFeaturesToTrack by Shi/Tomasi
// maybe not final, just found it on my hard drive in that state
#ifndef __CORNERS_HPP__
#define __CORNERS_HPP__

#include "ARRAY.HPP"
#include <math.h>

struct CornerPoint {
  double x,y;
};

class Corners {
public:

  int width;
  int height;
  double *values;
#define CORNER_VAL(__x__,__y__) values[(x+(__x__))+(y+(__y__))*width]

  bool subPixels;
  bool subPixelsQuadratic;
  double threshold;


  Corners(int width, int height) {
    this->width = width;
    this->height = height;
    values = new double[width*height];
    subPixels = false;
    subPixelsQuadratic = false;
    threshold = 0.00015;
  }

  ~Corners() {
    delete[] values;
  }

  double polyPeak(double lower, double middle, double upper) {
    double a = 0.5*lower - middle + 0.5*upper;
    if (fabs(a) < 1.192093e-06) return 0.0;
    double b = 0.5*upper - 0.5*lower;
    return -b/(2.0*a);
  }
  
  void subPixelPeakLinear(int x, int y, double *xf, double *yf) {
    if (xf != NULL) *xf = x+polyPeak(CORNER_VAL(-1, 0), CORNER_VAL(0, 0), CORNER_VAL(1, 0));
    if (yf != NULL) *yf = y+polyPeak(CORNER_VAL(0, -1), CORNER_VAL(0, 0), CORNER_VAL(0, 1));
  }
  
  void subPixelPeakQuadratic(int x, int y, double *xf, double *yf) {
    double bmm = CORNER_VAL(-1,-1);
    double b0m = CORNER_VAL(0,-1);
    double bpm = CORNER_VAL(1,-1);
    double bm0 = CORNER_VAL(-1,0);
    double b00 = CORNER_VAL(0,0);
    double bp0 = CORNER_VAL(1,0);
    double bmp = CORNER_VAL(-1,1);
    double b0p = CORNER_VAL(0,1);
    double bpp = CORNER_VAL(1,1);
    double A = (bmm - 2.0*b0m + bpm + bm0 - 2.0*b00 + bp0 + bmp - 2.0*b0p + bpp) / 6.0;
    double B = (bmm - bpm - bmp + bpp) / 4.0;
    double C = (bmm + b0m + bpm - 2.0*bm0 - 2.0*b00 - 2.0*bp0 + bmp + b0p + bpp) / 6.0;
    double D = (-bmm + bpm - bm0 + bp0 - bmp + bpp) / 6.0;
    double E = (-bmm - b0m - bpm + bmp + b0p + bpp) / 6.0;
    //double F = (-bmm + 2.0*b0m - bpm + 2.0*bm0 + 5.0*b00 + 2.0*bp0 - bmp + 2.0*b0p-bpp) / 9.0; // maybe this is somehow missing
    double term = (4*A*C-B*B);
    if (fabs(term)<1.192093e-06) {
      if (xf != NULL) *xf = x+0;
      if (yf != NULL) *yf = y+0;
      return;
    }
    if (xf != NULL) *xf = x+(B*E-2.0*C*D) / term;
    if (yf != NULL) *yf = y+(B*D-2.0*A*E) / term;
  }

  inline double luma(unsigned int rgba) {
    return (double)((rgba & 255)*25+((rgba>>8) & 255)*60+((rgba>>16) & 255)*15)/100.0/255.0;
  }
#define CORNER_LUMA(__x__,__y__) luma(rgba[(x+(__x__))+(y+(__y__))*width])

  void cornerIntensities(unsigned int *rgba) {
    // harris corner measure
    for (int y = 1; y < height-1; y++) {
      for (int x = 1; x < width-1; x++) {
        double h = (double)((CORNER_LUMA(1,-1)+CORNER_LUMA(1,0)+CORNER_LUMA(1,1)) - (CORNER_LUMA(-1,-1)+CORNER_LUMA(-1,0)+CORNER_LUMA(-1,1)))/6.0;
        double v = (double)((CORNER_LUMA(-1,1)+CORNER_LUMA(0,1)+CORNER_LUMA(1,1)) - (CORNER_LUMA(-1,-1)+CORNER_LUMA(0,-1)+CORNER_LUMA(1,-1)))/6.0;
        double hh = h*h;
        double vv = v*v;
        double vh = v*h;
        double det = hh*vv-vh*vh;
        double tr = hh+vv;
        //double originalHarris = (det - k*tr*tr)/tr - threshold;
        double harrisNoble = det / (tr+0.0001) - threshold;
        values[x+y*width] = harrisNoble; // this can get numerical instable if just using floats
      }
    }
  }

  void collectCorners(Array<CornerPoint> *points) {
    // non maximum supression
    for (int y = 2; y < height-2; y++) {
      for (int x = 2; x < width-2; x++) {
        float p0 = CORNER_VAL(0,0);
        if (CORNER_VAL(-1,0)>=p0||CORNER_VAL(+1,0)>=p0||CORNER_VAL(0,-1)>=p0||CORNER_VAL(0,+1)>=p0) continue;
        CornerPoint k;
        if (subPixels) {
          if (subPixelsQuadratic) 
            subPixelPeakQuadratic(x,y,&k.x,&k.y);
          else
            subPixelPeakLinear(x,y,&k.x,&k.y);
        } else {
          k.x = x;
          k.y = y;
        }
        points->push_back(k);
      }
    }
  }

  void process(unsigned int *rgba, Array<CornerPoint> *points) {
    cornerIntensities(rgba);
    collectCorners(points);
  }
};

#endif //__CORNERS_HPP__
