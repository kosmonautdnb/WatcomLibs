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
  double *gradients;
  double *blurredGradients;
#define CORNER_VAL(__x__,__y__) values[(x+(__x__))+(y+(__y__))*width]

  bool subPixels;
  bool subPixelsQuadratic;
  double k;
  int shiftingWindowSize;
  int gaussKernelSize;


  Corners(int width, int height) {
    this->width = width;
    this->height = height;
    values = new double[width*height];
    gradients = new double[width*height*3];
    blurredGradients = new double[width*height*3];
    subPixels = false;
    subPixelsQuadratic = false;
    k = 0.04;
    gaussKernelSize = 4;
    shiftingWindowSize = 4;
  }

  ~Corners() {
    delete[] values;
    delete[] gradients;
    delete[] blurredGradients;
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

  void collectGradients(unsigned int *rgba) {
    for (int y = 1; y < height-1; y++) {
      for (int x = 1; x < width-1; x++) {
        double h = (double)((CORNER_LUMA(1,-1)+CORNER_LUMA(1,0)+CORNER_LUMA(1,1)) - (CORNER_LUMA(-1,-1)+CORNER_LUMA(-1,0)+CORNER_LUMA(-1,1)))/3.0;
        double v = (double)((CORNER_LUMA(-1,1)+CORNER_LUMA(0,1)+CORNER_LUMA(1,1)) - (CORNER_LUMA(-1,-1)+CORNER_LUMA(0,-1)+CORNER_LUMA(1,-1)))/3.0;
        double *gr = &gradients[(x+y*width)*3+0];
        gr[0] = h*h;
        gr[1] = v*v;
        gr[2] = v*h;
      }
    }
  }

  void blurGradients() {
    // actually you need a gaussian blur here, currently it's a box blur
    for (int y = 1; y < height-1; y++) {
      for (int x = 1; x < width-1; x++) {
        double hh = 0;
        double vv = 0;
        double vh = 0;
        double w = 0;
        for (int y2 = -gaussKernelSize; y2 <= gaussKernelSize; y2++) {
          for (int x2 = -gaussKernelSize; x2 <= gaussKernelSize; x2++) {
            int xp = x + x2;
            int yp = y + y2;
            if (xp>0&&xp<width-1&&yp>0&&yp<height-1) {
              double *gr = &gradients[(xp+yp*width)*3+0];
              hh += gr[0];
              vv += gr[1];
              vh += gr[2];
              w += 1.0;
            }
          }
        }
        if (w != 0) {
          hh /= w;
          vv /= w;
          vh /= w;
        }
        double *gr = &blurredGradients[(x+y*width)*3+0];
        gr[0] = hh;        
        gr[1] = vv;        
        gr[2] = vh;        
      }
    }
  }

  double harris() {
    double max_ = 0;
    for (int y = 1+shiftingWindowSize; y < height-1-shiftingWindowSize; y++) {
      for (int x = 1+shiftingWindowSize; x < width-1-shiftingWindowSize; x++) {
        double hh = 0;
        double vv = 0;
        double vh = 0;
        for (int y2 = -shiftingWindowSize; y2 <= shiftingWindowSize; y2++) {
          for (int x2 = -shiftingWindowSize; x2 <= shiftingWindowSize; x2++) {
            int xp = x + x2;
            int yp = y + y2;
            if (xp>0&&xp<width-1&&yp>0&&yp<height-1) {
              double *gr = &blurredGradients[(xp+yp*width)*3+0];
              hh += gr[0];
              vv += gr[1];
              vh += gr[2];
            }
          }
        }
        double determinant = hh*vv-vh*vh;
        double trace = hh+vv;
        double harris = determinant - k*trace*trace; // harris
        //double harris = trace == 0 ? 0 : determinant/trace; // harris noble
        values[x+y*width] = harris;
        if (harris>max_) max_ = harris;
      }
    }
    return max_;
  }

  void collectCorners(Array<CornerPoint> *points, double threshold) {
    // non-maximum supression
    for (int y = 1+shiftingWindowSize; y < height-1-shiftingWindowSize; y++) {
      for (int x = 1+shiftingWindowSize; x < width-1-shiftingWindowSize; x++) {
        double p0 = CORNER_VAL(0,0);
        if (p0<threshold) continue;
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
    collectGradients(rgba);
    blurGradients();
    double max_ = harris();
    collectCorners(points, max_ * k);
  }
};

#endif //__CORNERS_HPP__
