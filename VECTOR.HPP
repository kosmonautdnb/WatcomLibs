#ifndef __VECTOR_HPP__
#define __VECTOR_HPP__

class Matrix;

class Vector {
public:
  float x,y,z;
  float w;

  Vector() {
    x  = 0;
    y  = 0;
    z  = 0;
    w  = 1;
  }

  Vector(float _x, float _y, float _z = 0.f, float _w = 1.f) {
    x = _x;
    y = _y;
    z = _z;
    w = _w;
  }
   
  Vector &set(float _x, float _y, float _z);
  Vector &set(float v);
  Vector &set(const Vector &b);

  Vector &zero();

  Vector &add(float _x, float _y, float _z);
  Vector &sub(float _x, float _y, float _z);
  Vector &div(float _x, float _y, float _z);
  Vector &mul(float _x, float _y, float _z);

  Vector &add(const Vector &b);
  Vector &sub(const Vector &b);
  Vector &div(const Vector &b);
  Vector &mul(const Vector &b);

  Vector &add(float v);
  Vector &sub(float v);
  Vector &div(float v);
  Vector &mul(float v);
  
  Vector &div(const Matrix &b);
  Vector &mul(const Matrix &b);

  float length() const;
  float lengthSqr() const;

  Vector &normalize();

  Vector &operator+=(const Vector &b);
  Vector &operator-=(const Vector &b);
  Vector &operator*=(const Vector &b);
  Vector &operator/=(const Vector &b);

  Vector &operator+=(const float &b);
  Vector &operator-=(const float &b);
  Vector &operator*=(const float &b);
  Vector &operator/=(const float &b);

  Vector &operator*=(const Matrix &b); // just calls transform
  Vector &operator/=(const Matrix &b); // uses just transform

  Vector xx() const;
  Vector xy() const;
  Vector xz() const;
  Vector yx() const;
  Vector yy() const;
  Vector yz() const;
  Vector zx() const;
  Vector zy() const;
  Vector zz() const;
  Vector xxx() const;
  Vector xxy() const;
  Vector xxz() const;
  Vector xyx() const;
  Vector xyy() const;
  Vector xyz() const;
  Vector xzx() const;
  Vector xzy() const;
  Vector xzz() const;
  Vector yxx() const;
  Vector yxy() const;
  Vector yxz() const;
  Vector yyx() const;
  Vector yyy() const;
  Vector yyz() const;
  Vector yzx() const;
  Vector yzy() const;
  Vector yzz() const;
  Vector zxx() const;
  Vector zxy() const;
  Vector zxz() const;
  Vector zyx() const;
  Vector zyy() const;
  Vector zyz() const;
  Vector zzx() const;
  Vector zzy() const;
  Vector zzz() const;
};

float length(const Vector &a);
float lengthSqr(const Vector &a);
float dot(const Vector &a, const Vector &b);
Vector cross(const Vector &a, const Vector &b);
Vector normalize(const Vector &b);

Vector add(const Vector &a, const Vector &b);
Vector sub(const Vector &a, const Vector &b);
Vector mul(const Vector &a, const Vector &b);
Vector div(const Vector &a, const Vector &b);

Vector add(const Vector &a, float v);
Vector sub(const Vector &a, float v);
Vector mul(const Vector &a, float v);
Vector div(const Vector &a, float v);

Vector add(float v, const Vector &a);
Vector sub(float v, const Vector &a);
Vector mul(float v, const Vector &a);
Vector div(float v, const Vector &a);

Vector add(const Vector &a, float x, float y, float z);
Vector sub(const Vector &a, float x, float y, float z);
Vector mul(const Vector &a, float x, float y, float z);
Vector div(const Vector &a, float x, float y, float z);

Vector mul(const Vector &a, const Matrix &b);
Vector div(const Vector &a, const Matrix &b);
Vector mul(const Matrix &a, const Vector &b);
Vector div(const Matrix &a, const Vector &b);

Vector operator-(const Vector &a, const Vector &b);
Vector operator+(const Vector &a, const Vector &b);
Vector operator*(const Vector &a, const Vector &b);
Vector operator/(const Vector &a, const Vector &b);

Vector operator*(const Vector &a, const float &b);
Vector operator/(const Vector &a, const float &b);
Vector operator*(const float &a, const Vector &b);
Vector operator/(const float &a, const Vector &b);

Vector operator*(const Vector &a, const Matrix &b);
Vector operator/(const Vector &a, const Matrix &b);
Vector operator*(const Matrix &a, const Vector &b);
Vector operator/(const Matrix &a, const Vector &b);

#endif //__VECTOR_HPP__
