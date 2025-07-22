#include "vector.hpp"
#include "matrix.hpp"
#include <math.h>

#define PI 3.14159265358979323846

Vector &Vector::set(double _x, double _y, double _z) {
  x = _x; 
  y = _y; 
  z = _z; 
  return *this;
}

Vector &Vector::set(double v) {
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

Vector &Vector::add(double _x, double _y, double _z) {
  x += _x; 
  y += _y; 
  z += _z; 
  return *this;
}

Vector &Vector::sub(double _x, double _y, double _z) {
  x -= _x; 
  y -= _y; 
  z -= _z; 
  return *this;
}

Vector &Vector::div(double _x, double _y, double _z) {
  if (_x==0||_y==0||_z==0) return *this;
  x /= _x; 
  y /= _y; 
  z /= _z; 
  return *this;
}

Vector &Vector::mul(double _x, double _y, double _z) {
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
  if (b.x==0||b.y==0||b.z==0) return *this;
  x = x/b.x; 
  y = y/b.y; 
  z = z/b.z; 
  return *this;
}

Vector &Vector::mul(const Vector &b) {
  x *= b.x; 
  y *= b.y; 
  z *= b.z; 
  return *this;
}

Vector &Vector::add(double v) {
  x += v; 
  y += v; 
  z += v; 
  return *this;
}

Vector &Vector::sub(double v) {
  x -= v; 
  y -= v; 
  z -= v; 
  return *this;
}

Vector &Vector::div(double v) {
  if (v==0) return *this;
  x = x/v; 
  y = y/v; 
  z = z/v; 
  return *this;
}                                   

Vector &Vector::mul(double v) {
  x = x*v; 
  y = y*v; 
  z = z*v; 
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

double Vector::length() const {
  return sqrt(x*x+y*y+z*z);
}

double Vector::lengthSqr() const {
  return x*x+y*y+z*z;
}

double Vector::manhatten() const {
  return fabs(x)+fabs(y)+fabs(z);
}

Vector &Vector::normalize() {
  double l = length(); 
  if (l>0) 
    div(l); 
  return *this;
}

Vector &Vector::operator-() {
  x = -x;
  y = -y;
  z = -z;
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

Vector &Vector::operator+=(const double &b) {
  return add(b);
}

Vector &Vector::operator-=(const double &b) {
  return sub(b);
}

Vector &Vector::operator*=(const double &b) {
  return mul(b);
}

Vector &Vector::operator/=(const double &b) {
  return div(b);
}

Vector &Vector::operator*=(const Matrix &b) {
  return mul(b);
}

Vector &Vector::operator/=(const Matrix &b) {
  return div(b);
}

double length(const Vector &a) {
  return a.length();
}

double lengthSqr(const Vector &a) {
  return a.lengthSqr();
}

double manhatten(const Vector &a) {
  return a.manhatten();
}

double dot(const Vector &a, const Vector &b) {
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

Vector reflect(const Vector &a, const Vector &b) {
  // a = i, b = n, ..normalize
  const double dot2NI = 2.0 * (a.x*b.x+a.y*b.y+a.z*b.z);
  return Vector(a.x-dot2NI*b.x,a.y-dot2NI*b.y,a.z-dot2NI*b.z);
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

Vector add(const Vector &a, double v) {
  Vector r;
  r.set(a).add(v);
  return r;
}

Vector sub(const Vector &a, double v) {
  Vector r;
  r.set(a).sub(v);
  return r;
}

Vector mul(const Vector &a, double v) {
  Vector r;
  r.set(a).mul(v);
  return r;
}

Vector div(const Vector &a, double v) {
  Vector r;
  r.set(a).div(v);
  return r;
}

Vector add(double v, const Vector &a) {
  Vector r;
  r.set(a).add(v);
  return r;
}

Vector sub(double v, const Vector &a) {
  Vector r;
  r.set(a).sub(v);
  return r;
}

Vector mul(double v, const Vector &a) {
  Vector r;
  r.set(a).mul(v);
  return r;
}

Vector div(double v, const Vector &a) {
  Vector r;
  r.set(a).div(v);
  return r;
}

Vector add(const Vector &a, double x, double y, double z) {
  Vector r;
  r.set(a).add(x,y,z);
  return r;
}

Vector sub(const Vector &a, double x, double y, double z) {
  Vector r;
  r.set(a).sub(x,y,z);
  return r;
}

Vector mul(const Vector &a, double x, double y, double z) {
  Vector r;
  r.set(a).mul(x,y,z);
  return r;
}

Vector div(const Vector &a, double x, double y, double z) {
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

Vector operator+(const Vector &a, const double &b) {
  return add(a,b);
}

Vector operator-(const Vector &a, const double &b) {
  return sub(a,b);
}

Vector operator+(const double &a, const Vector &b) {
  return add(a,b);
}

Vector operator-(const double &a, const Vector &b) {
  return sub(a,b);
}

Vector operator*(const Vector &a, const double &b) {
  return mul(a,b);
}

Vector operator/(const Vector &a, const double &b) {
  return div(a,b);
}

Vector operator*(const double &a, const Vector &b) {
  return mul(a,b);
}

Vector operator/(const double &a, const Vector &b) {
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


Vector max(const Vector &a,const Vector &b) {
  Vector r;
  r.x = a.x > b.x ? a.x : b.x;
  r.y = a.y > b.y ? a.y : b.y;
  r.z = a.z > b.z ? a.z : b.z;
  r.w = a.w;
  return r;
}

Vector min(const Vector &a,const Vector &b) {
  Vector r;
  r.x = a.x < b.x ? a.x : b.x;
  r.y = a.y < b.y ? a.y : b.y;
  r.z = a.z < b.z ? a.z : b.z;
  r.w = a.w;
  return r;
}

static double __inline fastfloor(double a) {
  int i = (int)a;
  if (i > a) i--;
  return i;
}

static double __inline mod(double a, double b) {
  // glsl way
  return a - (b*fastfloor(a/b));
}

Vector mod(const Vector &a, const Vector &b) {
  Vector r;
  r.x = mod(a.x,b.x);
  r.y = mod(a.y,b.y);
  r.z = mod(a.z,b.z);
  r.w = a.w;
  return r;
}

Vector abs(const Vector &a) {
  Vector r;
  r.x = fabs(a.x);
  r.y = fabs(a.y);
  r.z = fabs(a.z);
  r.w = a.w;
  return r;
}

Vector sin(const Vector &a) {
  Vector r;
  r.x = sin(a.x);
  r.y = sin(a.y);
  r.z = sin(a.z);
  r.w = a.w;
  return r;
}

Vector cos(const Vector &a) {
  Vector r;
  r.x = cos(a.x);
  r.y = cos(a.y);
  r.z = cos(a.z);
  r.w = a.w;
  return r;
}

Vector pow(const Vector &a, const double &b) {
  Vector r;
  r.x = pow(a.x,b);
  r.y = pow(a.y,b);
  r.z = pow(a.z,b);
  r.w = a.w;
  return r;
}

Vector pow(const Vector &a, const Vector &b) {
  Vector r;
  r.x = pow(a.x,b.x);
  r.y = pow(a.y,b.y);
  r.z = pow(a.z,b.z);
  r.w = a.w;
  return r;
}

Vector sign(const Vector &a) {
  Vector r;
  r.x = a.x < 0 ? -1 : (a.x > 0 ? 1 : 0);
  r.y = a.y < 0 ? -1 : (a.y > 0 ? 1 : 0);
  r.z = a.z < 0 ? -1 : (a.z > 0 ? 1 : 0);
  r.w = a.w;
  return r;
}

Vector scalar(const double &a) {
  Vector r;
  r.x = a;
  r.y = a;
  r.z = a;
  r.w = a;
  return r;
}

Vector clamp(const Vector &a, const double &b, const double &c) {
  Vector r;
  r.x = a.x < b ? b : (a.x > c ? c : a.x);
  r.y = a.y < b ? b : (a.y > c ? c : a.y);
  r.z = a.z < b ? b : (a.z > c ? c : a.z);
  r.w = a.w;
  return r;
}

Vector saturate(const Vector &a) {
  return clamp(a,0,1);
}

Vector lerp(const Vector &a, const Vector &b, const double &c) {
  Vector r;
  r.x = (b.x - a.x) * c + a.x;
  r.y = (b.y - a.y) * c + a.y;
  r.z = (b.z - a.z) * c + a.z;
  r.w = (b.w - a.w) * c + a.w;
  return r;
}

Vector mix(const Vector &a, const Vector &b, const double &c) {
  return lerp(a,b,c);
}

double distance(const Vector &a, const Vector &b) {
  return length(a-b);
}

Vector toPolar(const Vector &a) {
  Vector n = normalize(a).xzy()*Vector(1,1,-1);
  const double theta = atan2(n.y,n.x);
  const double phi = acos(n.z);
  return Vector(0.5+theta/(2.0*PI),phi/PI);
}

Vector fromPolar(const Vector &a) {
  const double theta = (a.x - 0.5) * 2 * PI;
  const double phi = a.y * PI;
  return Vector(sin(phi)*cos(theta),-cos(phi),sin(phi)*sin(theta));
}

static float __inline clamp(float v, float mn, float mx) {
  return v < mn ? mn : (v > mx ? mx : v);
}

unsigned int toRGBA(const Vector &a) {
  int r = (int)clamp(a.x*255.f,0.f,255.f);
  int g = (int)clamp(a.y*255.f,0.f,255.f);
  int b = (int)clamp(a.z*255.f,0.f,255.f);
  int a_ = (int)clamp(a.w*255.f,0.f,255.f);
  return r|(g<<8)|(b<<16)|(a_<<24);
}

unsigned int toRGB(const Vector &a) {
  int r = (int)clamp(a.x*255.f,0.f,255.f);
  int g = (int)clamp(a.y*255.f,0.f,255.f);
  int b = (int)clamp(a.z*255.f,0.f,255.f);
  return r|(g<<8)|(b<<16)|0xff000000;
}

Vector rotateX(const Vector &a, double angle) {
  return Vector(a.x,cos(angle)*a.y-sin(angle)*a.z,sin(angle)*a.y+cos(angle)*a.z);
}
 
Vector rotateY(const Vector &a, double angle) {
  return Vector(cos(angle)*a.x+sin(angle)*a.z,a.y,-sin(angle)*a.x+cos(angle)*a.z);
}
 
Vector rotateZ(const Vector &a, double angle) {
  return Vector(cos(angle)*a.x-sin(angle)*a.y,sin(angle)*a.x+cos(angle)*a.y,a.z);
} 
