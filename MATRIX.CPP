#include "matrix.hpp"
#include <math.h>

Matrix &Matrix::zero() {
  for (int i = 0; i < 16; i++) m[i]=0;
  return *this;
}

Matrix &Matrix::identity() {
  zero();
  set(0,0,1);
  set(1,1,1);
  set(2,2,1);
  set(3,3,1);
  return *this;
}  

Matrix &Matrix::set(int x, int y, double v) {
  m[x+y*4]=v;
  return *this;
}

Matrix &Matrix::set(const Matrix &a) {
  for (int i = 0; i < 16; i++) m[i]=a.m[i];
  return *this;
}

double Matrix::get(int x, int y) const {
  return m[x+y*4];
}

Matrix &Matrix::setRow(int y, const Vector &b) {
  set(y,0,b.x);
  set(y,1,b.y);
  set(y,2,b.z);
  set(y,3,b.w);
  return *this;
}

Matrix &Matrix::setRow3(int y, const Vector &b) {
  set(y,0,b.x);
  set(y,1,b.y);
  set(y,2,b.z);
  return *this;
}

Vector Matrix::getRow(int y) const {
  Vector r;
  r.x = get(y,0);
  r.y = get(y,1);
  r.z = get(y,2);
  r.w = get(y,3);
  return r;
}

Matrix &Matrix::setColumn(int x, const Vector &b) {
  set(0,x,b.x);
  set(1,x,b.y);
  set(2,x,b.z);
  set(3,x,b.w);
  return *this;
}

Matrix &Matrix::setColumn3(int x, const Vector &b) {
  set(0,x,b.x);
  set(1,x,b.y);
  set(2,x,b.z);
  return *this;
}

Vector Matrix::getColumn(int x) const {
  Vector r;
  r.x = get(0,x);
  r.y = get(1,x);
  r.z = get(2,x);
  r.w = get(3,x);
  return r;
}

Matrix &Matrix::transpose() {
  Matrix a;
  a.set(*this);
  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 4; x++) {
      set(x,y,a.get(y,x));
    }
  }
  return *this;
}

Matrix &Matrix::transpose3x3() {
  Matrix a;
  a.set(*this);
  for (int y = 0; y < 3; y++) {
    for (int x = 0; x < 3; x++) {
      set(x,y,a.get(y,x));
    }
  }
  return *this;
}

Matrix &Matrix::mul(const Matrix &a) {
  Matrix result;
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      double v = 0.0;
      for (int k = 0; k < 4; k++) {
        v += get(i,k) * a.get(k,j);
      }
      result.set(i,j,v);
    }
  }
  set(result);
  return *this;
}

Matrix &Matrix::mul3x3(const Matrix &a) {
  Matrix result;
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      double v = 0.0;
      for (int k = 0; k < 3; k++) {
        v += get(i,k) * a.get(k,j);
      }
      result.set(i,j,v);
    }
  }
  set(result);
  return *this;
}

Matrix &Matrix::mulTransposed(const Matrix &a) {
  Matrix result;
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      double v = 0.0;
      for (int k = 0; k < 4; k++) {
        v += a.get(i,k) * get(k,j);
      }
      result.set(i,j,v);
    }
  }
  set(result);
  return *this;
}

Matrix &Matrix::mul3x3Transposed(const Matrix &a) {
  Matrix result;
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      double v = 0.0;
      for (int k = 0; k < 3; k++) {
        v += a.get(i,k) * get(k,j);
      }
      result.set(i,j,v);
    }
  }
  set(result);
  return *this;
}

Matrix &Matrix::operator*=(const Matrix &a) {
  mul(a);
  return *this;
}

Matrix &Matrix::operator/=(const Matrix &a) {
  mul(::inverse(a));
  return *this;
}

Matrix Matrix::rotationMatrix3x3() {
  Matrix result;
  result.identity();
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      result.set(i,j,this->get(i,j));
    }
  }  
  result.normalize3x3();
  return result;
}

Matrix Matrix::matrix3x3() {
  Matrix result;
  result.identity();
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      result.set(i,j,this->get(i,j));
    }
  }  
  return result;
}


#define SWAP_ROWS_DOUBLE(a, b) { double *_tmp = a; (a)=(b); (b)=_tmp; }
#define MAT(m,r,c) (m)[(c)*4+(r)]
#define FABSF fabs

bool Matrix::invert() {
  //https://archive.mesa3d.org/older-versions/9.x/9.0/
  //MesaLib-9.0.zip/Mesa-9.0\src\mesa\math\m_matrix.c
  double wtmp[4][8];
  double m0, m1, m2, m3, s;
  double *r0, *r1, *r2, *r3;
  
  r0 = wtmp[0], r1 = wtmp[1], r2 = wtmp[2], r3 = wtmp[3];
  
  r0[0] = MAT(m,0,0), r0[1] = MAT(m,0,1),
  r0[2] = MAT(m,0,2), r0[3] = MAT(m,0,3),
  r0[4] = 1.0, r0[5] = r0[6] = r0[7] = 0.0,
  
  r1[0] = MAT(m,1,0), r1[1] = MAT(m,1,1),
  r1[2] = MAT(m,1,2), r1[3] = MAT(m,1,3),
  r1[5] = 1.0, r1[4] = r1[6] = r1[7] = 0.0,
  
  r2[0] = MAT(m,2,0), r2[1] = MAT(m,2,1),
  r2[2] = MAT(m,2,2), r2[3] = MAT(m,2,3),
  r2[6] = 1.0, r2[4] = r2[5] = r2[7] = 0.0,
  
  r3[0] = MAT(m,3,0), r3[1] = MAT(m,3,1),
  r3[2] = MAT(m,3,2), r3[3] = MAT(m,3,3),
  r3[7] = 1.0, r3[4] = r3[5] = r3[6] = 0.0;
  
  /* choose pivot - or die */
  if (FABSF(r3[0])>FABSF(r2[0])) SWAP_ROWS_DOUBLE(r3, r2);
  if (FABSF(r2[0])>FABSF(r1[0])) SWAP_ROWS_DOUBLE(r2, r1);
  if (FABSF(r1[0])>FABSF(r0[0])) SWAP_ROWS_DOUBLE(r1, r0);
  if (0.0 == r0[0])  return false;
  
  /* eliminate first variable     */
  m1 = r1[0]/r0[0]; m2 = r2[0]/r0[0]; m3 = r3[0]/r0[0];
  s = r0[1]; r1[1] -= m1 * s; r2[1] -= m2 * s; r3[1] -= m3 * s;
  s = r0[2]; r1[2] -= m1 * s; r2[2] -= m2 * s; r3[2] -= m3 * s;
  s = r0[3]; r1[3] -= m1 * s; r2[3] -= m2 * s; r3[3] -= m3 * s;
  s = r0[4];
  if (s != 0.0) { r1[4] -= m1 * s; r2[4] -= m2 * s; r3[4] -= m3 * s; }
  s = r0[5];
  if (s != 0.0) { r1[5] -= m1 * s; r2[5] -= m2 * s; r3[5] -= m3 * s; }
  s = r0[6];
  if (s != 0.0) { r1[6] -= m1 * s; r2[6] -= m2 * s; r3[6] -= m3 * s; }
  s = r0[7];
  if (s != 0.0) { r1[7] -= m1 * s; r2[7] -= m2 * s; r3[7] -= m3 * s; }
  
  /* choose pivot - or die */
  if (FABSF(r3[1])>FABSF(r2[1])) SWAP_ROWS_DOUBLE(r3, r2);
  if (FABSF(r2[1])>FABSF(r1[1])) SWAP_ROWS_DOUBLE(r2, r1);
  if (0.0 == r1[1])  return false;
  
  /* eliminate second variable */
  m2 = r2[1]/r1[1]; m3 = r3[1]/r1[1];
  r2[2] -= m2 * r1[2]; r3[2] -= m3 * r1[2];
  r2[3] -= m2 * r1[3]; r3[3] -= m3 * r1[3];
  s = r1[4]; if (0.0 != s) { r2[4] -= m2 * s; r3[4] -= m3 * s; }
  s = r1[5]; if (0.0 != s) { r2[5] -= m2 * s; r3[5] -= m3 * s; }
  s = r1[6]; if (0.0 != s) { r2[6] -= m2 * s; r3[6] -= m3 * s; }
  s = r1[7]; if (0.0 != s) { r2[7] -= m2 * s; r3[7] -= m3 * s; }
  
  /* choose pivot - or die */
  if (FABSF(r3[2])>FABSF(r2[2])) SWAP_ROWS_DOUBLE(r3, r2);
  if (0.0 == r2[2])  return false;
  
  /* eliminate third variable */
  m3 = r3[2]/r2[2];
  r3[3] -= m3 * r2[3], r3[4] -= m3 * r2[4],
  r3[5] -= m3 * r2[5], r3[6] -= m3 * r2[6],
  r3[7] -= m3 * r2[7];
  
  /* last check */
  if (0.0 == r3[3]) return false;
  
  s = 1.0F/r3[3];             /* now back substitute row 3 */
  r3[4] *= s; r3[5] *= s; r3[6] *= s; r3[7] *= s;
  
  m2 = r2[3];                 /* now back substitute row 2 */
  s  = 1.0F/r2[2];
  r2[4] = s * (r2[4] - r3[4] * m2), r2[5] = s * (r2[5] - r3[5] * m2),
  r2[6] = s * (r2[6] - r3[6] * m2), r2[7] = s * (r2[7] - r3[7] * m2);
  m1 = r1[3];
  r1[4] -= r3[4] * m1, r1[5] -= r3[5] * m1,
  r1[6] -= r3[6] * m1, r1[7] -= r3[7] * m1;
  m0 = r0[3];
  r0[4] -= r3[4] * m0, r0[5] -= r3[5] * m0,
  r0[6] -= r3[6] * m0, r0[7] -= r3[7] * m0;
  
  m1 = r1[2];                 /* now back substitute row 1 */
  s  = 1.0F/r1[1];
  r1[4] = s * (r1[4] - r2[4] * m1), r1[5] = s * (r1[5] - r2[5] * m1),
  r1[6] = s * (r1[6] - r2[6] * m1), r1[7] = s * (r1[7] - r2[7] * m1);
  m0 = r0[2];
  r0[4] -= r2[4] * m0, r0[5] -= r2[5] * m0,
  r0[6] -= r2[6] * m0, r0[7] -= r2[7] * m0;
  
  m0 = r0[1];                 /* now back substitute row 0 */
  s  = 1.0F/r0[0];
  r0[4] = s * (r0[4] - r1[4] * m0), r0[5] = s * (r0[5] - r1[5] * m0),
  r0[6] = s * (r0[6] - r1[6] * m0), r0[7] = s * (r0[7] - r1[7] * m0);

  MAT(m,0,0) = r0[4]; MAT(m,0,1) = r0[5],
  MAT(m,0,2) = r0[6]; MAT(m,0,3) = r0[7],
  MAT(m,1,0) = r1[4]; MAT(m,1,1) = r1[5],
  MAT(m,1,2) = r1[6]; MAT(m,1,3) = r1[7],
  MAT(m,2,0) = r2[4]; MAT(m,2,1) = r2[5],
  MAT(m,2,2) = r2[6]; MAT(m,2,3) = r2[7],
  MAT(m,3,0) = r3[4]; MAT(m,3,1) = r3[5],
  MAT(m,3,2) = r3[6]; MAT(m,3,3) = r3[7];
 
  return true;
}

void Matrix::normalize3x3() {
  setColumn3(0,normalize(getColumn(0)));
  setColumn3(1,normalize(getColumn(1)));
  setColumn3(2,normalize(getColumn(2)));
}

void Matrix::translate(const Vector &v) {
  MAT(m,0,3)=MAT(m,0,0)*v.x+MAT(m,0,1)*v.y+MAT(m,0,2)*v.z+MAT(m,0,3);
  MAT(m,1,3)=MAT(m,1,0)*v.x+MAT(m,1,1)*v.y+MAT(m,1,2)*v.z+MAT(m,1,3);
  MAT(m,2,3)=MAT(m,2,0)*v.x+MAT(m,2,1)*v.y+MAT(m,2,2)*v.z+MAT(m,2,3);
  MAT(m,3,3)=MAT(m,3,0)*v.x+MAT(m,3,1)*v.y+MAT(m,3,2)*v.z+MAT(m,3,3);
}

void Matrix::translateTransposed(const Vector &v) {
  MAT(m,3,0)=MAT(m,0,0)*v.x+MAT(m,1,0)*v.y+MAT(m,2,0)*v.z+MAT(m,3,0);
  MAT(m,3,1)=MAT(m,0,1)*v.x+MAT(m,1,1)*v.y+MAT(m,2,1)*v.z+MAT(m,3,1);
  MAT(m,3,2)=MAT(m,0,2)*v.x+MAT(m,1,2)*v.y+MAT(m,2,2)*v.z+MAT(m,3,2);
  MAT(m,3,3)=MAT(m,0,3)*v.x+MAT(m,1,3)*v.y+MAT(m,2,3)*v.z+MAT(m,3,3);
}

void Matrix::scale3(const Vector &v) {
  MAT(m,0,0) *= v.x;
  MAT(m,1,0) *= v.x;
  MAT(m,2,0) *= v.x;

  MAT(m,0,1) *= v.y;
  MAT(m,1,1) *= v.y;
  MAT(m,2,1) *= v.y;

  MAT(m,0,2) *= v.z;
  MAT(m,1,2) *= v.z;
  MAT(m,2,2) *= v.z;
}

void Matrix::scale3Transposed(const Vector &v) {
  MAT(m,0,0) *= v.x;
  MAT(m,0,1) *= v.x;
  MAT(m,0,2) *= v.x;

  MAT(m,1,0) *= v.y;
  MAT(m,1,1) *= v.y;
  MAT(m,1,2) *= v.y;

  MAT(m,2,0) *= v.z;
  MAT(m,2,1) *= v.z;
  MAT(m,2,2) *= v.z;
}

Matrix rotationMatrixX(double a) {
  return rotationMatrixAngleAxis(a,1,0,0);
}

Matrix rotationMatrixY(double a) {
  return rotationMatrixAngleAxis(a,0,1,0);
}
                 
Matrix rotationMatrixZ(double a) {
  return rotationMatrixAngleAxis(a,0,0,1);
}

Matrix rotationMatrixAngleAxis(double a, double x, double y, double z) {
  double OneMinusCosAngle, CosAngle, SinAngle;
  double A_OneMinusCosAngle, C_OneMinusCosAngle;
  CosAngle = cos(a);
  OneMinusCosAngle = 1.0 - CosAngle;
  SinAngle = sin(a);
  A_OneMinusCosAngle = x*OneMinusCosAngle;
  C_OneMinusCosAngle = z*OneMinusCosAngle;

  Matrix matrix;

  matrix.m[0] = x*A_OneMinusCosAngle + CosAngle;
  matrix.m[1] = y*A_OneMinusCosAngle + z * SinAngle;
  matrix.m[2] = z*A_OneMinusCosAngle - y * SinAngle;
  matrix.m[3] = 0.0;

  matrix.m[4] = y*A_OneMinusCosAngle-z*SinAngle;
  matrix.m[5] = y*y*OneMinusCosAngle+CosAngle;
  matrix.m[6] = y*C_OneMinusCosAngle+x*SinAngle;
  matrix.m[7] = 0.0;

  matrix.m[8] = x*C_OneMinusCosAngle+y*SinAngle;
  matrix.m[9] = y*C_OneMinusCosAngle-x*SinAngle;
  matrix.m[10] = z*C_OneMinusCosAngle+CosAngle;
  matrix.m[11] = 0.0; 

  matrix.m[12] = 0.0;
  matrix.m[13] = 0.0;
  matrix.m[14] = 0.0;
  matrix.m[15] = 1.0;
  
  return matrix;
}

Matrix rotationMatrixAngleAxis(double a, const Vector &b) {
  return rotationMatrixAngleAxis(a, b.x, b.y, b.z);
}

Matrix inverse(const Matrix &a) {
  Matrix c = a;
  c.invert();
  return c;
}

Matrix rotationMatrixFromNormal(const Vector &a) {
  Matrix ret;
  if (length(a.xz())<0.001) { // here is a "bug" (gimbal lock)
    Vector forward = Vector(0,0,1);
    Vector x = normalize(cross(forward,a));
    Vector z = normalize(cross(a,x));
    Vector y = normalize(cross(z,x));
    x = normalize(cross(z,y));
    z = normalize(cross(y,x));
    y = normalize(cross(z,x));
    x.w =  z.w = y.w = 0;
    ret.setColumn(0,x);
    ret.setColumn(1,y);
    ret.setColumn(2,z);
  } else {
    Vector up = Vector(0,1,0);
    Vector x = normalize(cross(up,a));
    Vector z = normalize(cross(a,x));
    Vector y = normalize(cross(z,x));
    x = normalize(cross(z,y));
    z = normalize(cross(y,x));
    y = normalize(cross(z,x));
    x.w =  z.w = y.w = 0;
    ret.setColumn(0,x);
    ret.setColumn(1,y);
    ret.setColumn(2,z);
  }
  return ret;
}

Matrix transpose(const Matrix &a) {
  Matrix c = a;
  c.transpose();
  return c;
}

#define GET(__x__,__y__) m[(__x__)+(__y__)*4]

Vector transform(const Vector &a, const Matrix &b) {
  Vector r;
  r.x = b.GET(0,0)*a.x+b.GET(0,1)*a.y+b.GET(0,2)*a.z+b.GET(0,3)*a.w;
  r.y = b.GET(1,0)*a.x+b.GET(1,1)*a.y+b.GET(1,2)*a.z+b.GET(1,3)*a.w;
  r.z = b.GET(2,0)*a.x+b.GET(2,1)*a.y+b.GET(2,2)*a.z+b.GET(2,3)*a.w;
  r.w = b.GET(3,0)*a.x+b.GET(3,1)*a.y+b.GET(3,2)*a.z+b.GET(3,3)*a.w;
  return r;
}

Vector transform3(const Vector &a, const Matrix &b) {
  Vector r;
  r.x = b.GET(0,0)*a.x+b.GET(0,1)*a.y+b.GET(0,2)*a.z+b.GET(0,3);
  r.y = b.GET(1,0)*a.x+b.GET(1,1)*a.y+b.GET(1,2)*a.z+b.GET(1,3);
  r.z = b.GET(2,0)*a.x+b.GET(2,1)*a.y+b.GET(2,2)*a.z+b.GET(2,3);
  r.w = 1;
  return r;
}

Vector transformTransposed(const Vector &a, const Matrix &b) {
  Vector r;
  r.x = b.GET(0,0)*a.x+b.GET(1,0)*a.y+b.GET(2,0)*a.z+b.GET(3,0)*a.w;
  r.y = b.GET(0,1)*a.x+b.GET(1,1)*a.y+b.GET(2,1)*a.z+b.GET(3,1)*a.w;
  r.z = b.GET(0,2)*a.x+b.GET(1,2)*a.y+b.GET(2,2)*a.z+b.GET(3,2)*a.w;
  r.w = b.GET(0,3)*a.x+b.GET(1,3)*a.y+b.GET(2,3)*a.z+b.GET(3,3)*a.w;
  return r;
}

Vector transform3x3(const Vector &a, const Matrix &b) {
  Vector r;
  r.x = b.GET(0,0)*a.x+b.GET(0,1)*a.y+b.GET(0,2)*a.z;
  r.y = b.GET(1,0)*a.x+b.GET(1,1)*a.y+b.GET(1,2)*a.z;
  r.z = b.GET(2,0)*a.x+b.GET(2,1)*a.y+b.GET(2,2)*a.z;
  return r;
}

Vector transformTransposed3x3(const Vector &a, const Matrix &b) {
  Vector r;
  r.x = b.GET(0,0)*a.x+b.GET(1,0)*a.y+b.GET(2,0)*a.z;
  r.y = b.GET(0,1)*a.x+b.GET(1,1)*a.y+b.GET(2,1)*a.z;
  r.z = b.GET(0,2)*a.x+b.GET(1,2)*a.y+b.GET(2,2)*a.z;
  return r;
}

Vector project(const Vector &a, const Matrix &b) {
  Vector r;
  r.x = b.GET(0,0)*a.x+b.GET(0,1)*a.y+b.GET(0,2)*a.z+b.GET(0,3)*a.w;
  r.y = b.GET(1,0)*a.x+b.GET(1,1)*a.y+b.GET(1,2)*a.z+b.GET(1,3)*a.w;
  r.z = b.GET(2,0)*a.x+b.GET(2,1)*a.y+b.GET(2,2)*a.z+b.GET(2,3)*a.w;
  r.w = b.GET(3,0)*a.x+b.GET(3,1)*a.y+b.GET(3,2)*a.z+b.GET(3,3)*a.w;
  r.x /= r.w;
  r.y /= r.w;
  r.z /= r.w;
  r.w /= r.w;
  return r;
}

Vector projectTransposed(const Vector &a, const Matrix &b) {
  Vector r;
  r.x = b.GET(0,0)*a.x+b.GET(1,0)*a.y+b.GET(2,0)*a.z+b.GET(3,0)*a.w;
  r.y = b.GET(0,1)*a.x+b.GET(1,1)*a.y+b.GET(2,1)*a.z+b.GET(3,1)*a.w;
  r.z = b.GET(0,2)*a.x+b.GET(1,2)*a.y+b.GET(2,2)*a.z+b.GET(3,2)*a.w;
  r.w = b.GET(0,3)*a.x+b.GET(1,3)*a.y+b.GET(2,3)*a.z+b.GET(3,3)*a.w;
  r.x /= r.w;
  r.y /= r.w;
  r.z /= r.w;
  r.w /= r.w;
  return r;
}

Matrix mul(const Matrix &a, const Matrix &b) {
  Matrix c = a;
  c.mul(b);
  return c;
}

Matrix operator *(const Matrix &a, const Matrix &b) {
  return mul(a,b);
}

Matrix operator /(const Matrix &a, const Matrix &b) {
  return mul(a,inverse(b));
}
