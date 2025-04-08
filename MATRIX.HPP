#ifndef __MATRIX_HPP__
#define __MATRIX_HPP__

#include "vector.hpp"

class Matrix {
public:
  float m[4*4];

  Matrix() {
    for (int i = 0; i < 16; i++) m[i]=0.f;
  }

  Matrix &zero();
  Matrix &identity();

  Matrix &set(int x, int y, float v);
  Matrix &set(const Matrix &a);
  float get(int x, int y) const;
  
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
};

Matrix rotationMatrixX(float a);
Matrix rotationMatrixY(float a);
Matrix rotationMatrixZ(float a);
Matrix rotationMatrixAngleAxis(float a, float x, float y, float z);
Matrix rotationMatrixAngleAxis(float a, const Vector &b);

Matrix inverse(const Matrix &a);

Vector transform(const Vector &a, const Matrix &b);
Vector transformTransposed(const Vector &a, const Matrix &b);
Vector transform3x3(const Vector &a, const Matrix &b);
Vector transformTransposed3x3(const Vector &a, const Matrix &b);

Matrix mul(const Matrix &a, const Matrix &b);
Matrix operator *(const Matrix &a, const Matrix &b);
Matrix operator /(const Matrix &a, const Matrix &b);

#endif //__MATRIX_HPP__
