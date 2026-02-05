#ifndef __VECTOR_HPP__
#define __VECTOR_HPP__

#include "object.hpp"
#include "string.hpp"
#include <stdlib.h>

class Matrix;

class Vector : public Object {
public:
  double x,y,z;
  double w;

  Vector() {
    x  = 0;
    y  = 0;
    z  = 0;
    w  = 1;
  }

  Vector(const Vector &v, const double _w) {
    x = v.x;
    y = v.y;
    z = v.z;
    w = _w;
  }

  Vector(double _x, double _y, double _z = 0.0, double _w = 1.0) { // atleast vec2, if only x would be left it would override double in some template cases
    x = _x;
    y = _y;
    z = _z;
    w = _w;
  }


  virtual ~Vector() {}

  virtual uint32_t hash() const {
    return getHash(x)+getHash(y)*3+getHash(z)*7+getHash(w)*11;
  }
  
  virtual bool equals(Object *a) const {
    Vector *v = dynamic_cast<Vector*>(a);
    if (v == NULL) return false;
    return ::equals(x,v->x) && ::equals(y,v->y) && ::equals(z,v->z);
  }
  
  virtual String toString() const {
    return "[" + ::toString(x) + "," + ::toString(y) + "," + ::toString(z) + "," + ::toString(w) + "]";
  }

  bool operator==(const Vector &b) {return b.x==x&&b.y==y&&b.z==z&&b.w==w;}
  bool operator!=(const Vector &b) {return !(b.x==x&&b.y==y&&b.z==z&&b.w==w);}
   
  Vector &set(double _x, double _y, double _z);
  Vector &set(double _x, double _y, double _z, double _w);
  Vector &set(double v);
  Vector &set(const Vector &b);
  double get(int index) const;

  Vector &zero();

  Vector &add(double _x, double _y, double _z);
  Vector &sub(double _x, double _y, double _z);
  Vector &div(double _x, double _y, double _z);
  Vector &mul(double _x, double _y, double _z);

  Vector &add(const Vector &b);
  Vector &sub(const Vector &b);
  Vector &div(const Vector &b);
  Vector &mul(const Vector &b);

  Vector &add(double v);
  Vector &sub(double v);
  Vector &div(double v);
  Vector &mul(double v);
  
  Vector &div(const Matrix &b);
  Vector &mul(const Matrix &b);

  double length() const;
  double lengthSqr() const;
  double manhatten() const;

  Vector &normalize();

  Vector &operator-();

  Vector &operator+=(const Vector &b);
  Vector &operator-=(const Vector &b);
  Vector &operator*=(const Vector &b);
  Vector &operator/=(const Vector &b);

  Vector &operator+=(const double &b);
  Vector &operator-=(const double &b);
  Vector &operator*=(const double &b);
  Vector &operator/=(const double &b);

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

double length(const Vector &a);
double lengthSqr(const Vector &a);
double manhatten(const Vector &a);
double dot(const Vector &a, const Vector &b);
Vector cross(const Vector &a, const Vector &b);
Vector normalize(const Vector &b);
Vector reflect(const Vector &a, const Vector &b);

Vector add(const Vector &a, const Vector &b);
Vector sub(const Vector &a, const Vector &b);
Vector mul(const Vector &a, const Vector &b);
Vector div(const Vector &a, const Vector &b);

Vector add(const Vector &a, double v);
Vector sub(const Vector &a, double v);
Vector mul(const Vector &a, double v);
Vector div(const Vector &a, double v);

Vector add(double v, const Vector &a);
Vector sub(double v, const Vector &a);
Vector mul(double v, const Vector &a);
Vector div(double v, const Vector &a);

Vector add(const Vector &a, double x, double y, double z);
Vector sub(const Vector &a, double x, double y, double z);
Vector mul(const Vector &a, double x, double y, double z);
Vector div(const Vector &a, double x, double y, double z);

Vector mul(const Vector &a, const Matrix &b);
Vector div(const Vector &a, const Matrix &b);
Vector mul(const Matrix &a, const Vector &b);
Vector div(const Matrix &a, const Vector &b);

Vector operator-(const Vector &a, const Vector &b);
Vector operator+(const Vector &a, const Vector &b);
Vector operator*(const Vector &a, const Vector &b);
Vector operator/(const Vector &a, const Vector &b);

Vector operator+(const Vector &a, const double &b);
Vector operator-(const Vector &a, const double &b);
Vector operator+(const double &a, const Vector &b);
Vector operator-(const double &a, const Vector &b);

Vector operator*(const Vector &a, const double &b);
Vector operator/(const Vector &a, const double &b);
Vector operator*(const double &a, const Vector &b);
Vector operator/(const double &a, const Vector &b);

Vector operator*(const Vector &a, const Matrix &b);
Vector operator/(const Vector &a, const Matrix &b);
Vector operator*(const Matrix &a, const Vector &b);
Vector operator/(const Matrix &a, const Vector &b);

Vector max(const Vector &a, const Vector &b);
Vector min(const Vector &a, const Vector &b);
Vector mod(const Vector &a, const Vector &b);
Vector abs(const Vector &a);
Vector sin(const Vector &a);
Vector cos(const Vector &a);
Vector pow(const Vector &a, const double &b);
Vector pow(const Vector &a, const Vector &b);
Vector sign(const Vector &a);
Vector scalar(const double &a);
Vector clamp(const Vector &a, const double &b, const double &c);
Vector saturate(const Vector &a);
Vector lerp(const Vector &a, const Vector &b, const double &c);
Vector mix(const Vector &a, const Vector &b, const double &c);
double distance(const Vector &a, const Vector &b);
Vector toPolar(const Vector &a);
Vector fromPolar(const Vector &a);
unsigned int toRGBA(const Vector &a);
unsigned int toRGB(const Vector &a);

Vector rotateX(const Vector &a, double angle); 
Vector rotateY(const Vector &a, double angle); 
Vector rotateZ(const Vector &a, double angle); 

#endif //__VECTOR_HPP__
