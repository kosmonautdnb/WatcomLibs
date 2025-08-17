#ifndef __MATRIX_HPP__
#define __MATRIX_HPP__

#include "vector.hpp"
#include "object.hpp"

class Matrix : public Object {
public:
  double m[4*4];

  Matrix() {
    for (int32_t i = 0; i < 16; i++) m[i]=0.0;
  }

  virtual ~Matrix() {
  }

  virtual uint32_t hash() const {
    uint32_t hash = 0;
    for (int32_t i = 0; i < 16; i++) hash += getHash(m[i]);
    return hash;
  }
  
  virtual bool equals(Object *a) const {
    Matrix *v = dynamic_cast<Matrix*>(a);
    if (v == NULL) return false;
    for (int32_t i = 0; i < 16; i++) if (!(::equals(m[i],v->m[i]))) return false;
    return true;
  }
  
  virtual String toString() const {
    String r = "[";
    for (int32_t i = 0; i < 16; i++) {if (i != 0) r += ","; r += ::toString(m[i]);}
    r += "]";
    return r;
  }

  Matrix &zero();
  Matrix &identity();

  Matrix &set(int x, int y, double v);
  Matrix &set(const Matrix &a);
  double get(int x, int y) const;
  
  Matrix &setRow(int y, const Vector &b);
  Vector getRow(int y) const;
  Matrix &setColumn(int x, const Vector &b);
  Vector getColumn(int x) const;
  
  Matrix &transpose();
  Matrix &transpose3x3();
  
  Matrix &mul(const Matrix &a);
  Matrix &mul3x3(const Matrix &a);
  Matrix &mulTransposed(const Matrix &a);
  Matrix &mul3x3Transposed(const Matrix &a);

  Matrix &operator *=(const Matrix &a);
  Matrix &operator /=(const Matrix &a);

  bool invert();
  void normalize3x3();
  void translate(const Vector &v);
  void translateTransposed(const Vector &v);
  void scale3(const Vector &v);
  void scale3Transposed(const Vector &v);
};

Matrix rotationMatrixX(double a);
Matrix rotationMatrixY(double a);
Matrix rotationMatrixZ(double a);
Matrix rotationMatrixAngleAxis(double a, double x, double y, double z);
Matrix rotationMatrixAngleAxis(double a, const Vector &b);

Matrix inverse(const Matrix &a);
Matrix transpose(const Matrix &a);

Vector transform(const Vector &a, const Matrix &b);
Vector transformTransposed(const Vector &a, const Matrix &b);
Vector transform3x3(const Vector &a, const Matrix &b);
Vector transformTransposed3x3(const Vector &a, const Matrix &b);

Vector project(const Vector &a, const Matrix &b);
Vector projectTransposed(const Vector &a, const Matrix &b);

Matrix mul(const Matrix &a, const Matrix &b);
Matrix operator *(const Matrix &a, const Matrix &b);
Matrix operator /(const Matrix &a, const Matrix &b);

#endif //__MATRIX_HPP__
