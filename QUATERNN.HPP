// https://cs.stanford.edu/~acoates/quaternion.h
#ifndef __QUATERNION_HPP__
#define __QUATERNION_HPP__

#include "object.hpp"
#include "string.hpp"
#include "vector.hpp"

class Matrix;

class Quaternion : public Object {
public:
  double x,y,z,w;

  Quaternion() {
    x  = 0;
    y  = 0;
    z  = 0;
    w  = 1;
  }

  Quaternion(const Vector &v, double _w) {
    x = v.x;
    y = v.y;
    z = v.z;
    w = _w;
  }

  Quaternion(const Vector &v) {
    x = v.x;
    y = v.y;
    z = v.z;
    w = v.w;
  }

  Quaternion(double _x, double _y, double _z, double _w) {
    x = _x;
    y = _y;
    z = _z;
    w = _w;
  }

  virtual ~Quaternion() {}

  virtual uint32_t hash() const {
    return getHash(x)+getHash(y)*3+getHash(z)*7+getHash(w)*11;
  }
  
  virtual bool equals(Object *a) const {
    Quaternion *v = dynamic_cast<Quaternion*>(a);
    if (v == NULL) return false;
    return ::equals(x,v->x) && ::equals(y,v->y) && ::equals(z,v->z) && ::equals(w,v->w);
  }
  
  virtual String toString() const {
    return "[" + ::toString(x) + "," + ::toString(y) + "," + ::toString(z) + "," + ::toString(w) + "]";
  }

  void identity() {x=0;y=0;z=0;w=1;}

  Vector complex() const;

  double real() const;

  Quaternion conjugate() const;

  Quaternion inverse() const;  

  Quaternion product(const Quaternion &b) const;

  Quaternion operator-() const;
  Quaternion operator*=(const double &v);
  Quaternion operator/=(const double &v);
  Quaternion operator*=(const Quaternion &b);
  Quaternion operator/=(const Quaternion &b);
  Quaternion operator+=(const Quaternion &b);
  Quaternion operator-=(const Quaternion &b);

  Matrix rotationMatrix() const;

  Vector vector() const;

  double length() const;

  Vector rotateVector(const Vector &v) const;

  void yawPitchRoll(double yaw, double pitch, double roll);

  Vector getYawPitchRoll() const;

  Quaternion slerp(const Quaternion &q1, double t) const;
  Quaternion nslerp(const Quaternion &q1, double t) const;

};

Quaternion normalize(const Quaternion &q);
Quaternion rotationMatrixToQuaternion(const Matrix &m); // somehow not soo precise
Quaternion slerp(const Quaternion &q0, const Quaternion &q1, double t);
Quaternion nslerp(const Quaternion &q0, const Quaternion &q1, double t);
Quaternion operator *(const Quaternion &a,const Quaternion &b);
Quaternion operator *(const Quaternion &a,const double &s);
Quaternion operator *(const double &s,Quaternion &b);
Quaternion operator +(const Quaternion &a,const Quaternion &b);
Quaternion operator -(const Quaternion &a,const Quaternion &b);
Quaternion operator /(const Quaternion &a,const double &b);
Quaternion operator /(const Quaternion &a,const Quaternion &b); // problems with 0.0

#endif //__QUATERNION_HPP__
