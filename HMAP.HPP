// you have to define __HMAP_IMPLEMENTATION__ in exactly one of your sourcefiles that includes this file.
#ifndef __HMAP_HPP__
#define __HMAP_HPP__

#include "MATRIX.HPP"
// a perspective matrix (dest = m * src; dest.xy/=dest.w) to transform the source polygon with 4 2d points to the dest polygon with 4 2d points
Matrix perspectiveHomography(double src[4*2], double dst[4*2]); // src = x1,y1,x2,y2,x3,y3,x4,y4; dst = x1,y1,x2,y2,x3,y3,x4,y4 (clockwise or counterclockwise)

#ifdef __HMAP_IMPLEMENTATION__

#include <math.h>
#include <string.h>

/*
  https://github.com/sloisel/numeric

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/
// hopefully no padding problems
static void LUsolve(double lu[8][8], const int p[8], const double b[4*2], double *ret) {

  int i, j; int n = 8;
  double x[4*2];
  int Pi;
  double *LUi;
  double tmp;

  for (i = n-1; i != -1; i--) x[i] = b[i];

  for (i = 0; i < n; i++) {
    Pi = p[i];
    if (p[i] != i) {
      tmp = x[i];
      x[i] = x[Pi];
      x[Pi] = tmp;
    }

    LUi = lu[i];
    for (j = 0; j < i; j++) {
      x[i] -= x[j] * LUi[j];
    }
  }

  for (i = n - 1; i >= 0; i--) {
    LUi = lu[i];
    for (j = i + 1; j < n; j++) {
      x[i] -= x[j] * LUi[j];
    }
    if (LUi[i] != 0) x[i] /= LUi[i];
  }

  memcpy(ret, x, sizeof(x));
}

static void LU(double a[8][8], double *lu, int *p) {

  double *A[8];
  int i, j, k, Pk;
  double absAjk, Akk;
  double *Ak, *Ai;
  double maxv;
  int n = 8, n1 = n-1;
  int P[8];

  for (i = 0; i < n; i++) A[i] = a[i];

  for (k = 0; k < n; k++) {
    Pk = k;
    Ak = A[k];
    maxv = fabs(Ak[k]);
    for (j = k + 1; j < n; j++) {
      absAjk = fabs(A[j][k]);
      if (maxv < absAjk) {
        maxv = absAjk;
        Pk = j;
      }
    }
    P[k] = Pk;

    if (Pk != k) {
      A[k] = A[Pk];
      A[Pk] = Ak;
      Ak = A[k];
    }

    Akk = Ak[k];

    for (i = k + 1; i < n; i++) {
      if (Akk != 0) A[i][k] /= Akk;
    }

    for (i = k + 1; i < n; i++) {
      Ai = A[i];
      for (j = k + 1; j < n1; j++) {
        Ai[j] -= Ai[k] * Ak[j];
        ++j;
        Ai[j] -= Ai[k] * Ak[j];
      }
      if(j == n1) Ai[j] -= Ai[k] * Ak[j];
    }
  }
  for (int o = 0; o < 8; o++) memcpy(&lu[o*8],A[o],sizeof(double)*8);
  memcpy(p,P,sizeof(P));
}

static void solve(double a[8][8], double b[4*2], double *ret) {
  double lu[8][8]; int p[8]; 
  LU(a, &(lu[0][0]), p);
  LUsolve(lu, p, b, ret);
}

/*
  MIT License
  
  Copyright (c) 2021 Eric Canas
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
  
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/
static Matrix toTransformationMatrix(double *l) {
  int i = 0;
  int m = 0;
  Matrix ret;
  for (int dy = 0; dy < 4; dy++) {
    for (int dx = 0; dx < 4; dx++) {
      if ((dy == 2 && dx == 2) || (dy == 3 && dx == 3)) {
        ret.m[m++] = 1;
      } else {
        if( dy == 2 || dx == 2) {
          ret.m[m++] = 0;
        } else {
          ret.m[m++] = l[((i++)*3)%8];
        }
      }
    }
  }
  return ret;
}

// src = x1,y1,x2,y2,x3,y3,x4,y4
// dst = x1,y1,x2,y2,x3,y3,x4,y4
Matrix perspectiveHomography(double src[4*2], double dst[4*2]) {
  double a[8][8] = {
    {src[0],src[1],1,0,0,0,-dst[0]*src[0],-dst[0]*src[1]},
    {0,0,0,src[0],src[1],1,-dst[1]*src[0],-dst[1]*src[1]},
    {src[2],src[3],1,0,0,0,-dst[2]*src[2],-dst[2]*src[3]},
    {0,0,0,src[2],src[3],1,-dst[3]*src[2],-dst[3]*src[3]},
    {src[4],src[5],1,0,0,0,-dst[4]*src[4],-dst[4]*src[5]},
    {0,0,0,src[4],src[5],1,-dst[5]*src[4],-dst[5]*src[5]},
    {src[6],src[7],1,0,0,0,-dst[6]*src[6],-dst[6]*src[7]},
    {0,0,0,src[6],src[7],1,-dst[7]*src[6],-dst[7]*src[7]}
  };
  double ret[8];
  solve(a, dst, ret);
  return toTransformationMatrix(ret);
}

#endif // __HMAP_IMPLEMENTATION__
#endif // __HMAP_HPP__
