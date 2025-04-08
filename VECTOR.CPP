#include "vector.hpp"
#include "matrix.hpp"
#include <math.h>


Vector &Vector::set(float _x, float _y, float _z) {
  x = _x; 
  y = _y; 
  z = _z; 
  return *this;
}

Vector &Vector::set(float v) {
  x = v; 
  y = v; 
  z = v; 
  return *this;
}

Vector &Vector::set(const Vector &b) {
  x = b.x; 
  y = b.y; 
  z = b.z; 
  return *this;
}

Vector &Vector::zero() {
  x = 0; 
  y = 0; 
  z = 0; 
  return *this;
}

Vector &Vector::add(float _x, float _y, float _z) {
  x += _x; 
  y += _y; 
  z += _z; 
  return *this;
}

Vector &Vector::sub(float _x, float _y, float _z) {
  x -= _x; 
  y -= _y; 
  z -= _z; 
  return *this;
}

Vector &Vector::div(float _x, float _y, float _z) {
  x /= _x; 
  y /= _y; 
  z /= _z; 
  return *this;
}

Vector &Vector::mul(float _x, float _y, float _z) {
  x *= _x;
  y *= _y;
  z *= _z; 
  return *this;
}

Vector &Vector::add(const Vector &b) {
  x += b.x; 
  y += b.y; 
  z += b.z; 
  return *this;
}

Vector &Vector::sub(const Vector &b) {
  x -= b.x; 
  y -= b.y; 
  z -= b.z; 
  return *this;
}

Vector &Vector::div(const Vector &b) {
  x /= b.x; 
  y /= b.y; 
  z /= b.z; 
  return *this;
}

Vector &Vector::mul(const Vector &b) {
  x *= b.x; 
  y *= b.y; 
  z *= b.z; 
  return *this;
}

Vector &Vector::add(float v) {
  x += v; 
  y += v; 
  z += v; 
  return *this;
}

Vector &Vector::sub(float v) {
  x -= v; 
  y -= v; 
  z -= v; 
  return *this;
}

Vector &Vector::div(float v) {
  x /= v; 
  y /= v; 
  z /= v; 
  return *this;
}                                   

Vector &Vector::mul(float v) {
  x *= v; 
  y *= v; 
  z *= v; 
  return *this;
}

Vector &Vector::div(const Matrix &b) {
  Matrix c = b;
  c.invert();
  set(transform(*this,c));
  return *this;
}

Vector &Vector::mul(const Matrix &b) {
  set(transform(*this,b));
  return *this;
}

Vector Vector::xx() const {
  return Vector(x,x);
}
Vector Vector::xy() const {
  return Vector(x,y);
}
Vector Vector::xz() const {
  return Vector(x,z);
}
Vector Vector::yx() const {
  return Vector(y,x);
}
Vector Vector::yy() const {
  return Vector(y,y);
}
Vector Vector::yz() const {
  return Vector(y,z);
}
Vector Vector::zx() const {
  return Vector(z,x);
}
Vector Vector::zy() const {
  return Vector(z,y);
}
Vector Vector::zz() const {
  return Vector(z,z);
}

Vector Vector::xxx() const {
  return Vector(x,x,x);
}
Vector Vector::xxy() const {
  return Vector(x,x,y);
}
Vector Vector::xxz() const {
  return Vector(x,x,z);
}
Vector Vector::xyx() const {
  return Vector(x,y,x);
}
Vector Vector::xyy() const {
  return Vector(x,y,y);
}
Vector Vector::xyz() const {
  return Vector(x,y,z);
}
Vector Vector::xzx() const {
  return Vector(x,z,x);
}
Vector Vector::xzy() const {
  return Vector(x,z,y);
}
Vector Vector::xzz() const {
  return Vector(x,z,z);
}

Vector Vector::yxx() const {
  return Vector(y,x,x);
}
Vector Vector::yxy() const {
  return Vector(y,x,y);
}
Vector Vector::yxz() const {
  return Vector(y,x,z);
}
Vector Vector::yyx() const {
  return Vector(y,y,x);
}
Vector Vector::yyy() const {
  return Vector(y,y,y);
}
Vector Vector::yyz() const {
  return Vector(y,y,z);
}
Vector Vector::yzx() const {
  return Vector(y,z,x);
}
Vector Vector::yzy() const {
  return Vector(y,z,y);
}
Vector Vector::yzz() const {
  return Vector(y,z,z);
}

Vector Vector::zxx() const {
  return Vector(z,x,x);
}
Vector Vector::zxy() const {
  return Vector(z,x,y);
}
Vector Vector::zxz() const {
  return Vector(z,x,z);
}
Vector Vector::zyx() const {
  return Vector(z,y,x);
}
Vector Vector::zyy() const {
  return Vector(z,y,y);
}
Vector Vector::zyz() const {
  return Vector(z,y,z);
}
Vector Vector::zzx() const {
  return Vector(z,z,x);
}
Vector Vector::zzy() const {
  return Vector(z,z,y);
}
Vector Vector::zzz() const {
  return Vector(z,z,z);
}

float Vector::length() const {
  return sqrt(x*x+y*y+z*z);
}

float Vector::lengthSqr() const {
  return x*x+y*y+z*z;
}

Vector &Vector::normalize() {
  float l = length(); 
  if (l>0) 
    div(l); 
  return *this;
}

Vector &Vector::operator+=(const Vector &b) {
  return add(b);
}

Vector &Vector::operator-=(const Vector &b) {
  return sub(b);
}

Vector &Vector::operator*=(const Vector &b) {
  return mul(b);
}

Vector &Vector::operator/=(const Vector &b) {
  return div(b);
}

Vector &Vector::operator+=(const float &b) {
  return add(b);
}

Vector &Vector::operator-=(const float &b) {
  return sub(b);
}

Vector &Vector::operator*=(const float &b) {
  return mul(b);
}

Vector &Vector::operator/=(const float &b) {
  return div(b);
}

Vector &Vector::operator*=(const Matrix &b) {
  return mul(b);
}

Vector &Vector::operator/=(const Matrix &b) {
  return div(b);
}

float length(const Vector &a) {
  return a.length();
}

float lengthSqr(const Vector &a) {
  return a.lengthSqr();
}

float dot(const Vector &a, const Vector &b) {
  return a.x*b.x + a.y*b.y + a.z*b.z;
}

Vector cross(const Vector &a, const Vector &b)  {
  Vector r;
  r.x = a.y*b.z - a.z*b.y;
  r.y = a.z*b.x - a.x*b.z;
  r.z = a.x*b.y - a.y*b.x;
  return r;
}

Vector normalize(const Vector &b) {
  Vector r;
  r.set(b).normalize();
  return r;
}


Vector add(const Vector &a, const Vector &b) {
  Vector r;
  r.set(a).add(b);
  return r;
}

Vector sub(const Vector &a, const Vector &b) {
  Vector r;
  r.set(a).sub(b);
  return r;
}

Vector mul(const Vector &a, const Vector &b) { 
  Vector r;
  r.set(a).mul(b);
  return r;
}

Vector div(const Vector &a, const Vector &b) {
  Vector r;
  r.set(a).div(b);
  return r;
}

Vector add(const Vector &a, float v) {
  Vector r;
  r.set(a).add(v);
  return r;
}

Vector sub(const Vector &a, float v) {
  Vector r;
  r.set(a).sub(v);
  return r;
}

Vector mul(const Vector &a, float v) {
  Vector r;
  r.set(a).mul(v);
  return r;
}

Vector div(const Vector &a, float v) {
  Vector r;
  r.set(a).div(v);
  return r;
}

Vector add(float v, const Vector &a) {
  Vector r;
  r.set(a).add(v);
  return r;
}

Vector sub(float v, const Vector &a) {
  Vector r;
  r.set(a).sub(v);
  return r;
}

Vector mul(float v, const Vector &a) {
  Vector r;
  r.set(a).mul(v);
  return r;
}

Vector div(float v, const Vector &a) {
  Vector r;
  r.set(a).div(v);
  return r;
}

Vector add(const Vector &a, float x, float y, float z) {
  Vector r;
  r.set(a).add(x,y,z);
  return r;
}

Vector sub(const Vector &a, float x, float y, float z) {
  Vector r;
  r.set(a).sub(x,y,z);
  return r;
}

Vector mul(const Vector &a, float x, float y, float z) {
  Vector r;
  r.set(a).mul(x,y,z);
  return r;
}

Vector div(const Vector &a, float x, float y, float z) {
  Vector r;
  r.set(a).div(x,y,z);
  return r;
}

Vector mul(const Vector &a, const Matrix &b) {
  return transformTransposed(a,b);
}

Vector div(const Vector &a, const Matrix &b) {
  Matrix c = b;
  c.invert();
  return transformTransposed(a,c);
}

Vector mul(const Matrix &a, const Vector &b) {
  return transform(b,a);
}

Vector div(const Matrix &a, const Vector &b) {
  Matrix c = a;
  c.invert();
  return transform(b,c);
}

Vector operator-(const Vector &a, const Vector &b) {
  return sub(a,b);
}

Vector operator+(const Vector &a, const Vector &b) {
  return add(a,b);
}

Vector operator*(const Vector &a, const Vector &b) {
  return mul(a,b);
}

Vector operator/(const Vector &a, const Vector &b) {
  return div(a,b);
}


Vector operator*(const Vector &a, const float &b) {
  return mul(a,b);
}

Vector operator/(const Vector &a, const float &b) {
  return div(a,b);
}

Vector operator*(const float &a, const Vector &b) {
  return mul(a,b);
}

Vector operator/(const float &a, const Vector &b) {
  return div(a,b);
}

Vector operator*(const Vector &a, const Matrix &b) {
  return mul(a,b);
}

Vector operator/(const Vector &a, const Matrix &b) {
  return div(a,b);
}

Vector operator*(const Matrix &a, const Vector &b) {
  return mul(a,b);
}

Vector operator/(const Matrix &a, const Vector &b) {
  return div(a,b);
}
