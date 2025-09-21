#include "quaternn.hpp"
#include "matrix.hpp"
#include <math.h>

#ifndef PI
#define PI 3.14159265358979323846
#endif

template<class T> static __inline T clamp(const T v, const T mn, const T mx) {
  return v<mn?mn:(v>mx?mx:v);
}

Vector Quaternion::complex() const {
  return Vector(x,y,z,0);
}

double Quaternion::real() const {
  return w;
}

Quaternion Quaternion::conjugate() const {
  return Quaternion(-x,-y,-z,w);
}

Quaternion Quaternion::inverse() const {
  return conjugate()/length();
}

Quaternion Quaternion::product(const Quaternion &b) const {
  return Quaternion(
    y*b.z-z*b.y+x*b.w+w*b.x,
    z*b.x-x*b.z+y*b.w+w*b.y,
    x*b.y-y*b.x+z*b.w+w*b.z,
    w*b.w-x*b.x-y*b.y-z*b.z);
}

Quaternion Quaternion::operator-() const {
  return Quaternion(-x,-y,-z,-w);
}

Quaternion Quaternion::operator*=(const double &v) {
  (*this)=(*this)*v;
  return *this;
}

Quaternion Quaternion::operator/=(const double &v) {
  (*this)=(*this)/v;
  return *this;
}

Quaternion Quaternion::operator*=(const Quaternion &b) {
  (*this)=(*this)*b;
  return *this;
}

Quaternion Quaternion::operator/=(const Quaternion &b) {
  (*this)=(*this)/b;
  return *this;
}

Quaternion Quaternion::operator+=(const Quaternion &b) {
  (*this)=(*this)+b;
  return *this;
}

Quaternion Quaternion::operator-=(const Quaternion &b) {
  (*this)=(*this)-b;
  return *this;
}

Matrix Quaternion::rotationMatrix() const {
  Matrix m;
  //m.SETROW(0,Vector(1-2*y*y-2*z*z,2*x*y-2*z*w,2*x*z+2*y*w,0)); // this doesn't got right, even with normalizing before
  //m.SETROW(1,Vector(2*x*y+2*z*w,1-2*x*x-2*z*z,2*y*z-2*x*w,0));
  //m.SETROW(2,Vector(2*x*z-2*y*w,2*y*z+2*x*w,1-2*x*x-2*y*y,0));
  //m.SETROW(3,Vector(0,0,0,1));
  Vector mx = Vector(1,0,0);
  Vector my = Vector(0,1,0);
  Vector mz = Vector(0,0,1);
  mx = rotateVector(mx);
  my = rotateVector(my);
  mz = rotateVector(mz);
  m.setColumn(0,Vector(mx,0));
  m.setColumn(1,Vector(my,0));
  m.setColumn(2,Vector(mz,0));
  m.setColumn(3,Vector(0,0,0,1));
  return m;
}

Vector Quaternion::vector() const {
  return Vector(x,y,z,w);
}

double Quaternion::length() const {
  return sqrt(x*x+y*y+z*z+w*w);
}

Vector Quaternion::rotateVector(const Vector &v) const {
  return Vector((((*this)*Quaternion(v,0))*conjugate()).complex(),1.0);
}

void Quaternion::yawPitchRoll(double yaw, double pitch, double roll) {
  const double c1 = cos(roll*0.5);
  const double c2 = cos(yaw*0.5);
  const double c3 = cos(pitch*0.5);
  const double s1 = sin(roll*0.5);
  const double s2 = sin(yaw*0.5);
  const double s3 = sin(pitch*0.5);
  this->x = c1*c2*s3 - s1*s2*c3;
  this->y = c1*s2*c3 + s1*c2*s3;
  this->z = s1*c2*c3 - c1*s2*s3;
  this->w = c1*c2*c3 + s1*s2*s3;
}

Vector Quaternion::getYawPitchRoll() const {
  Vector euler;
  const static double PI_OVER_2 = PI*0.5;
  const static double EPSILON = 1e-10;
  euler.x=asin(2.0*(w*y-x*z));
  if (PI_OVER_2-fabs(euler.x) > EPSILON) {
    double sqw,sqx,sqy,sqz;
    sqx = x*x;
    sqy = y*y;
    sqz = z*z;
    sqw = w*w;
    euler.z = atan2(2.0*(x*y+w*z),sqx-sqy-sqz+sqw);
    euler.y = atan2(2.0*(w*x+y*z),sqw-sqx-sqy+sqz);
  } else {
    euler.z = atan2(2*y*z-2*x*w,2*x*z+2*y*w);
    euler.y = 0;
    if (euler.x<0)
      euler.z = PI-euler.z;
  }
  euler.w = 0;
  return euler;
}

Quaternion Quaternion::slerp(const Quaternion &q1, double t) const {
  return ::slerp(*this,q1,t);
}

Quaternion Quaternion::nslerp(const Quaternion &q1, double t) const {
  return ::nslerp(*this,q1,t);
}

Quaternion normalize(const Quaternion &q) {
  Quaternion b = q;
  double n = b.length();
  if (n>0.0) {
    b/=n;
  }
  return b;
}

Quaternion rotationMatrixToQuaternion(const Matrix &m) {
  double trace;
  Quaternion q;
#define M(__x__,__y__) m.m[(__x__)*4+(__y__)]
  if (M(2,2)<0) {
    if (M(0,0)>M(1,1)) {
      trace = 1 + M(0,0) - M(1,1) - M(2,2);
      q = Quaternion(trace,M(0,1)+M(1,0),M(2,0)+M(0,2),M(1,2)-M(2,1));
    } else {
      trace = 1 - M(0,0) + M(1,1) - M(2,2);
      q = Quaternion(M(0,1)+M(1,0),trace,M(1,2)+M(2,1),M(2,0)-M(0,2));
    }
  } else {
    if (M(0,0)<-M(1,1)) {
      trace = 1 - M(0,0) - M(1,1) + M(2,2);
      q = Quaternion(M(2,0)+M(0,2),M(1,2)+M(2,1),trace,M(0,1)-M(1,0));
    } else {
      trace = 1 + M(0,0) + M(1,1) + M(2,2);
      q = Quaternion(M(1,2)-M(2,1),M(2,0)-M(0,2),M(0,1)-M(1,0),trace);
    }
  }
#undef M
  q *= 0.5/sqrt(trace);
  return q;
}

Quaternion slerp(const Quaternion &q0, const Quaternion &q1, double t) {
//  double omega = acos(clamp(q0.x*q1.x+q0.y*q1.y+q0.z*q1.z+q0.w*q1.w,-1.0,1.0));
//  if (fabs(omega)<1e-10) omega = 1e-10;
//  double som = sin(omega);
//  double st0 = sin((1-t)*omega)/som;
//  double st1 = sin(t*omega)/som;
//  return Quaternion(q0.x*st0+q1.x*st1,q0.y*st0+q1.y*st1,q0.z*st0+q1.z*st1,q0.w*st0+q1.w*st1);
  double cosHalfTheta=q0.x*q1.x+q0.y*q1.y+q0.z*q1.z+q0.w*q1.w;
  if (fabs(cosHalfTheta)>=1) return q0;
  bool reverse = false;
  if (cosHalfTheta<0) {
    cosHalfTheta = -cosHalfTheta;
    reverse = true;
  }
  double halfTheta=acos(cosHalfTheta);
  double sinHalfTheta=sqrt(1-cosHalfTheta*cosHalfTheta);
  
  if (fabs(sinHalfTheta)<0.001) {
    double st0 = 1-t;
    double st1 = t;
    if (reverse)
      return Quaternion(q0.x*st0-q1.x*st1,q0.y*st0-q1.y*st1,q0.z*st0-q1.z*st1,q0.w*st0-q1.w*st1);
    return Quaternion(q0.x*st0+q1.x*st1,q0.y*st0+q1.y*st1,q0.z*st0+q1.z*st1,q0.w*st0+q1.w*st1);
  }
  double st0 = sin((1-t)*halfTheta)/sinHalfTheta;
  double st1 = sin(t*halfTheta)/sinHalfTheta;
  if (reverse)
    return Quaternion(q0.x*st0-q1.x*st1,q0.y*st0-q1.y*st1,q0.z*st0-q1.z*st1,q0.w*st0-q1.w*st1);
  return Quaternion(q0.x*st0+q1.x*st1,q0.y*st0+q1.y*st1,q0.z*st0+q1.z*st1,q0.w*st0+q1.w*st1);
}

Quaternion nslerp(const Quaternion &q0, const Quaternion &q1, double t) {
  return normalize(slerp(q0,q1,t));
}

Quaternion operator *(const Quaternion &a,const Quaternion &b) {
  return a.product(b);
}

Quaternion operator *(const Quaternion &a,const double &s) {
  return Quaternion(a.x*s,a.y*s,a.z*s,a.w*s);
}

Quaternion operator *(const double &s,const Quaternion &b) {
  return Quaternion(b.x*s,b.y*s,b.z*s,b.w*s);
}

Quaternion operator +(const Quaternion &a,const Quaternion &b) {
  return Quaternion(a.x+b.x,a.y+b.y,a.z+b.z,a.w+b.w);
}

Quaternion operator -(const Quaternion &a,const Quaternion &b) {
  return Quaternion(a.x-b.x,a.y-b.y,a.z-b.z,a.w-b.w);
}

Quaternion operator /(const Quaternion &a,const double &b) {
  if (b==0.0) return a; // should never happen
  return Quaternion(a.x/b,a.y/b,a.z/b,a.w/b);
}

Quaternion operator /(const Quaternion &a,const Quaternion &b) {
  return Quaternion(b.x==0?0:a.x/b.x,b.y==0?0:a.y/b.y,b.z==0?0:a.z/b.z,b.w==0?0:a.w/b.w);
}
