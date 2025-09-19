#ifndef __COLL_HPP__
#define __COLL_HPP__

#include "vector.hpp"
#include "matrix.hpp"
#include "quaternn.hpp"
#include <math.h>

//---
// :Collision Types:
//---
// Capsule+Capsule
// Sphere+Sphere
// Sphere+Line
// Cube+Cube
// Cube+Sphere
// Cube+Line
// Line+Triangle
//---

// Capsule-Collision
// absolute height of capsule = tip to base
// width of the capsule = radius;
// derived from wickedengine.net
class CLCapsule {
public:
  CLCapsule() {radius = 0;}
  CLCapsule(const Vector &tip, const Vector &base, double radius) : tip(tip), base(base), radius(radius) {}
  Vector tip,base;
  double radius;

  Vector closestPointOnLineSegment(const Vector &A, const Vector &B, const Vector &Point) const {
    Vector AB = B - A;
    double k = dot(AB,AB);
    if (k == 0) k = 1;
    double t = dot(Point-A,AB) / k;
    return A + saturated(t) * AB;
  }

  // CAPSULE+CAPSULE
  bool collided(const CLCapsule &b, Vector *collisionVector = NULL) const {
    const CLCapsule &a = *this;
    Vector ad = a.tip-a.base;
    Vector bd = b.tip-b.base;
    double al = length(ad);
    double bl = length(bd);
    double ra = al < a.radius ? a.radius : al;
    double rb = bl < b.radius ? b.radius : bl;
    if (length((a.tip+a.base)-(b.tip+b.base))*0.5>ra+rb)
      return false;
  
    if (al == 0) al = 1;
    if (bl == 0) bl = 1;
  
    Vector a_Normal = ad/al;
    Vector a_LineOffset = a_Normal * a.radius;
    Vector a_A = a.base + a_LineOffset;
    Vector a_B = a.tip - a_LineOffset;
  
    Vector b_Normal = bd/bl;
    Vector b_LineOffset = b_Normal * b.radius;
    Vector b_A = b.base + b_LineOffset;
    Vector b_B = b.tip - b_LineOffset;
  
    Vector v0 = b_A - a_A;
    Vector v1 = b_B - a_A;
    Vector v2 = b_A - a_B;
    Vector v3 = b_B - a_B;
  
    double d0 = dot(v0,v0);
    double d1 = dot(v1,v1);
    double d2 = dot(v2,v2);
    double d3 = dot(v3,v3);
  
    Vector bestA;
    if (d2 < d0 || d2 < d1 || d3 < d0 || d3 < d1) {
      bestA = a_B;
    } else {
      bestA = a_A;
    }
  
    Vector bestB = closestPointOnLineSegment(b_A,b_B,bestA); // collision point b
    bestA = closestPointOnLineSegment(a_A,a_B,bestB); // collision point a
  
    Vector penetration_normal = bestA - bestB;
    double len = length(penetration_normal);
    if (len > 0) penetration_normal /= len;
    double penetration_depth = a.radius+b.radius-len;
    bool intersects = penetration_depth > 0;
    if (collisionVector!=NULL) {
      if (intersects)
        *collisionVector = penetration_normal * penetration_depth;
      else
        *collisionVector = Vector();
    }
    return intersects;
  }
};

// Sphere-Collision
class CLSphere {
public:
  Vector center;
  double radius;

  CLSphere() {}
  CLSphere(const Vector &center,const double &radius) : center(center), radius(radius) {}

  // SPHERE+SPHERE
  bool collided(const CLSphere &c2, Vector *collisionVector) const {
    Vector k = c2.center - center;
    const double l = length(k);
    const double d = c2.radius + radius;
    const bool intersects = l < c2.radius + radius;
    if (collisionVector!=NULL) {
      if (intersects) {
        *collisionVector = normalize(k)*(d - l);
      } else 
        *collisionVector = Vector();
    }
    return intersects;
  }
  // SPHERE+LINE
  // https://paulbourke.net/geometry/circlesphere/index.html#linesphere (not tested, yet)
  bool collided(const Vector &p0, const Vector &p1, Vector &collisionPoint1, Vector &collisionPoint2) {
    const double EPSILON = 0.00001;
    const Vector dp = p1 - p0;
    const double a = lengthSqr(dp);
    const double b = 2.0 * (dp.x * (p0.x - center.x) + dp.y * (p0.y - center.y) + dp.z * (p0.z - center.z));
    const double c = lengthSqr(center) + lengthSqr(p0) - 2.0 * dot(center,p0) - radius*radius;
    const double bb4ac = b * b - 4 * a * c;
    collisionPoint1.w = 0; collisionPoint2.w = 0;
    if (fabs(a)<EPSILON||bb4ac<0) {
      return false;
    }
    const double mu1 = (-b + sqrt(bb4ac)) / (2.0 * a);
    const double mu2 = (-b - sqrt(bb4ac)) / (2.0 * a);
    if (mu1 >= 0 && mu1 <= 1.0) {collisionPoint1 = p0 + mu1 * (p1 - p0); collisionPoint1.w = 1.0;}
    if (mu2 >= 0 && mu2 <= 1.0) {collisionPoint2 = p0 + mu2 * (p1 - p0); collisionPoint2.w = 1.0;}
    return true;
  }
};

// Cube-Collision
class CLCube {
public:
  Vector center;
  Vector size; // xyz from -size to +size
  Quaternion rotation;
  
  CLCube() {}
  CLCube(const Vector &center,const Vector &size,const Vector &rotation) : center(center), size(size), rotation(rotation) {}

  // CUBE+CUBE
  bool collided(const CLCube &c2) const {
    // Separating Axis Theorem
    const double EPSILON = 0.00001;
    Matrix m1,m2;
    m1.translate(center.xyz());  m1 *= rotation.rotationMatrix();  m1.scale3(size);
    m2.translate(c2.center.xyz()); m2 *= c2.rotation.rotationMatrix(); m2.scale3(c2.size);
    Vector axis[15];
    for (int j = 0; j < 15; j++) {
      if (j < 6) {
        switch(j) {
          case 0: axis[j] = rotation.rotationMatrix()*Vector(1,0,0); break;
          case 1: axis[j]  = rotation.rotationMatrix()*Vector(0,1,0); break;
          case 2: axis[j] = rotation.rotationMatrix()*Vector(0,0,1); break;
          case 3: axis[j] = c2.rotation.rotationMatrix()*Vector(1,0,0); break;
          case 4: axis[j] = c2.rotation.rotationMatrix()*Vector(0,1,0); break;
          case 5: axis[j] = c2.rotation.rotationMatrix()*Vector(0,0,1); break;
        }
      } else {
        // this part I don't get iask.ai called it edge on edge collisions
        int k = j - 6;
        int k1 = k % 3;
        int k2 = k / 3;
        axis[j] = cross(axis[k1],axis[k2+3]);
        if (length(axis[j])<EPSILON) 
          continue;
      }
      axis[j] = normalize(axis[j]); // for right coordinates
      axis[j].w = 0; // not needed
      double min1,min2,max1,max2;
      for (int i = 0; i < 8; i++) {
        Vector p;
        p.x = (i & 1) ? -1 : 1;
        p.y = (i & 2) ? -1 : 1;
        p.z = (i & 4) ? -1 : 1;
        p.w = 1;
        Vector p1 = m1 * p;
        Vector p2 = m2 * p;
        double dist1 = dot(p1,axis[j]);
        double dist2 = dot(p2,axis[j]);
        if (i == 0||dist1<min1) min1 = dist1;
        if (i == 0||dist2<min2) min2 = dist2;
        if (i == 0||dist1>max1) max1 = dist1;
        if (i == 0||dist2>max2) max2 = dist2;
      }
      const bool collision = (max1>min2+EPSILON&&max2>min1+EPSILON);
      if (!collision) 
        return false;
    }
    return true; // all axis reported a collision (no separating plane possible)
  }

  // CUBE+CUBE
  Vector collisionVector(const CLCube &c2) const {
    // Separating Axis Theorem
    // object must have collided before
    const double EPSILON = 0.00001;
    Matrix m1,m2;
    m1.translate(center.xyz());  m1 *= rotation.rotationMatrix();  m1.scale3(size);
    m2.translate(c2.center.xyz()); m2 *= c2.rotation.rotationMatrix(); m2.scale3(c2.size);
    Vector axis[15];
    double minOverlap;
    Vector collisionAxis;
    bool first = true;
    for (int j = 0; j < 15; j++) {
      if (j < 6) {
        switch(j) {
          case 0: axis[j] = rotation.rotationMatrix()*Vector(1,0,0); break;
          case 1: axis[j]  = rotation.rotationMatrix()*Vector(0,1,0); break;
          case 2: axis[j] = rotation.rotationMatrix()*Vector(0,0,1); break;
          case 3: axis[j] = c2.rotation.rotationMatrix()*Vector(1,0,0); break;
          case 4: axis[j] = c2.rotation.rotationMatrix()*Vector(0,1,0); break;
          case 5: axis[j] = c2.rotation.rotationMatrix()*Vector(0,0,1); break;
        }
      } else {
        // this part I don't get iask.ai called it edge on edge collisions
        int k = j - 6;
        int k1 = k % 3;
        int k2 = k / 3;
        axis[j] = cross(axis[k1],axis[k2+3]);
        if (length(axis[j])<EPSILON) 
          continue;
      }
      axis[j] = normalize(axis[j]); // for right coordinates
      axis[j].w = 0; // not needed
      double min1,min2,max1,max2;
      for (int i = 0; i < 8; i++) {
        Vector p;
        p.x = (i & 1) ? -1 : 1;
        p.y = (i & 2) ? -1 : 1;
        p.z = (i & 4) ? -1 : 1;
        p.w = 1;
        Vector p1 = m1 * p;
        Vector p2 = m2 * p;
        double dist1 = dot(p1,axis[j]);
        double dist2 = dot(p2,axis[j]);
        if (i == 0||dist1<min1) min1 = dist1;
        if (i == 0||dist2<min2) min2 = dist2;
        if (i == 0||dist1>max1) max1 = dist1;
        if (i == 0||dist2>max2) max2 = dist2;
      }
      double overlap = mind(max2,max1) - maxd(min2,min1);
      if (first || overlap<minOverlap) {
        first = false;
        minOverlap = overlap;
        collisionAxis = axis[j];
        if (dot(collisionAxis,center-c2.center)>0) collisionAxis = -collisionAxis;
      }
    }
    return minOverlap*collisionAxis;
  }

  // CUBE+SPHERE
  bool collided(const CLSphere &s2) const {
    // Separating Axis Theorem
    const double EPSILON = 0.00001;
    Matrix m1;
    m1.translate(center.xyz());  m1 *= rotation.rotationMatrix();  m1.scale3(size);
    Vector axis[4];
    for (int j = 0; j < 4; j++) {
      switch(j) {
        case 0: axis[j] = rotation.rotationMatrix()*Vector(1,0,0); break;
        case 1: axis[j]  = rotation.rotationMatrix()*Vector(0,1,0); break;
        case 2: axis[j] = rotation.rotationMatrix()*Vector(0,0,1); break;
        case 3: {
          // search nearest cube vertex to the center of sphere
          //double le;
          //for (int i = 0; i < 8; i++) { // not perfect, closest point to sphere center may also be an edge point, a face point, not just the closest cube vertex
          //  Vector p;
          //  p.x = (i & 1) ? -1 : 1;
          //  p.y = (i & 2) ? -1 : 1;
          //  p.z = (i & 4) ? -1 : 1;
          //  p.w = 1;
          //  Vector p1 = m1 * p;
          //  Vector e = p1 - s2.center;
          //  if (i==0||length(e)<le) {
          //    le = length(e);
          //    axis[j] = e;
          //  }
          //}
          Vector mc = inverse(m1)*s2.center;
          mc = clamp(mc,-1,1);
          mc = m1 * mc;
          axis[j] = s2.center - mc;
        } break;
      }
      axis[j] = normalize(axis[j]); // for right coordinates
      axis[j].w = 0; // not needed
      double min1,min2,max1,max2;
      // cube min max
      for (int i = 0; i < 8; i++) {
        Vector p;
        p.x = (i & 1) ? -1 : 1;
        p.y = (i & 2) ? -1 : 1;
        p.z = (i & 4) ? -1 : 1;
        p.w = 1;
        Vector p1 = m1 * p;
        double dist1 = dot(p1,axis[j]);
        if (i == 0||dist1<min1) min1 = dist1;
        if (i == 0||dist1>max1) max1 = dist1;
      }
      // sphere min max
      double dist2 = dot(s2.center,axis[j])+s2.radius;
      min2 = dist2;
      max2 = dist2;
      dist2 = dot(s2.center,axis[j])-s2.radius;
      if (dist2<min2) min2 = dist2;
      if (dist2>max2) max2 = dist2;
      const bool collision = (max1>min2+EPSILON&&max2>min1+EPSILON);
      if (!collision)
        return false;
    }
    return true; // all axis reported a collision (no separating plane possible)
  }

  // CUBE+SPHERE
  Vector collisionVector(const CLSphere &s2) const {
    // Separating Axis Theorem
    const double EPSILON = 0.00001;
    Matrix m1;
    m1.translate(center.xyz());  m1 *= rotation.rotationMatrix();  m1.scale3(size);
    Vector axis[4];
    double minOverlap;
    Vector collisionAxis;
    bool first = true;
    for (int j = 0; j < 4; j++) {
      switch(j) {
        case 0: axis[j] = rotation.rotationMatrix()*Vector(1,0,0); break;
        case 1: axis[j]  = rotation.rotationMatrix()*Vector(0,1,0); break;
        case 2: axis[j] = rotation.rotationMatrix()*Vector(0,0,1); break;
        case 3: {
          // search nearest cube vertex to the center of sphere
          //double le;
          //for (int i = 0; i < 8; i++) { // not perfect, closest point to sphere center may also be an edge point, a face point, not just the closest cube vertex
          //  Vector p;
          //  p.x = (i & 1) ? -1 : 1;
          //  p.y = (i & 2) ? -1 : 1;
          //  p.z = (i & 4) ? -1 : 1;
          //  p.w = 1;
          //  Vector p1 = m1 * p;
          //  Vector e = p1 - s2.center;
          //  if (i==0||length(e)<le) {
          //    le = length(e);
          //    axis[j] = e;
          //  }
          //}
          Vector mc = inverse(m1)*s2.center;
          mc = clamp(mc,-1,1);
          mc = m1 * mc;
          axis[j] = s2.center - mc;
        } break;
      }
      axis[j] = normalize(axis[j]); // for right coordinates
      axis[j].w = 0; // not needed
      double min1,min2,max1,max2;
      // cube min max
      for (int i = 0; i < 8; i++) {
        Vector p;
        p.x = (i & 1) ? -1 : 1;
        p.y = (i & 2) ? -1 : 1;
        p.z = (i & 4) ? -1 : 1;
        p.w = 1;
        Vector p1 = m1 * p;
        double dist1 = dot(p1,axis[j]);
        if (i == 0||dist1<min1) min1 = dist1;
        if (i == 0||dist1>max1) max1 = dist1;
      }
      // sphere min max
      double dist2 = dot(s2.center,axis[j])+s2.radius;
      min2 = dist2;
      max2 = dist2;
      dist2 = dot(s2.center,axis[j])-s2.radius;
      if (dist2<min2) min2 = dist2;
      if (dist2>max2) max2 = dist2;
      double overlap = mind(max2,max1) - maxd(min2,min1);
      if (first || overlap<minOverlap) {
        first = false;
        minOverlap = overlap;
        collisionAxis = axis[j];
        if (dot(collisionAxis,center-s2.center)>0) collisionAxis = -collisionAxis;
      }
    }
    return minOverlap*collisionAxis;
  }

  // CUBE+LINE
  static bool linePoint(const double fDst1, const double fDst2,const Vector &p1, const Vector &p2, Vector &hit) {
    if ((fDst1*fDst2)>=0.0) return false;
    if (fDst1==fDst2) return false;
    hit = p1 + (p2-p1)*(-fDst1/(fDst2-fDst1));
    return true;
  }
  // CUBE+LINE
  static bool inBox(const Vector &hit, const Vector &b1, const Vector &b2, const int axis) {
    switch(axis) {
    case 1: {if (hit.z>=b1.z&&hit.z<b2.z&&hit.y>=b1.y&&hit.y<b2.y) return true;} break;
    case 2: {if (hit.z>=b1.z&&hit.z<b2.z&&hit.x>=b1.x&&hit.x<b2.x) return true;} break;
    case 3: {if (hit.x>=b1.x&&hit.x<b2.x&&hit.y>=b1.y&&hit.y<b2.y) return true;} break;
    }
    return false;
  }
  // CUBE+LINE
  static bool collided(const Matrix &cube, const Vector &p0,const Vector &p1, Vector &collisionPoint, bool checkIfInside = true) {
    Matrix im1 = inverse(cube);
    const Vector l1 = im1 * p0;
    const Vector l2 = im1 * p1;
    const Vector b1(-1,-1,-1);
    const Vector b2(1,1,1);
    if (l1.x<b1.x && l2.x<b1.x) return false;
    if (l1.y<b1.y && l2.y<b1.y) return false;
    if (l1.z<b1.z && l2.z<b1.z) return false;
    if (l1.x>=b2.x && l2.x>=b2.x) return false;
    if (l1.y>=b2.y && l2.y>=b2.y) return false;
    if (l1.z>=b2.z && l2.z>=b2.z) return false;
    
    if (checkIfInside && l1.x>=b1.x && l1.y>=b1.y && l1.z>=b1.z && l1.x<b2.x && l1.y<b2.y && l1.z<b2.z) {collisionPoint=cube*l1;return true;}
    
    if (linePoint(l1.x-b1.x,l2.x-b1.x,l1,l2,collisionPoint)&&inBox(collisionPoint,b1,b2,1)) {collisionPoint=cube*collisionPoint;return true;}
    if (linePoint(l1.y-b1.y,l2.y-b1.y,l1,l2,collisionPoint)&&inBox(collisionPoint,b1,b2,2)) {collisionPoint=cube*collisionPoint;return true;}
    if (linePoint(l1.z-b1.z,l2.z-b1.z,l1,l2,collisionPoint)&&inBox(collisionPoint,b1,b2,3)) {collisionPoint=cube*collisionPoint;return true;}
    if (linePoint(l1.x-b2.x,l2.x-b2.x,l1,l2,collisionPoint)&&inBox(collisionPoint,b1,b2,1)) {collisionPoint=cube*collisionPoint;return true;}
    if (linePoint(l1.y-b2.y,l2.y-b2.y,l1,l2,collisionPoint)&&inBox(collisionPoint,b1,b2,2)) {collisionPoint=cube*collisionPoint;return true;}
    if (linePoint(l1.z-b2.z,l2.z-b2.z,l1,l2,collisionPoint)&&inBox(collisionPoint,b1,b2,3)) {collisionPoint=cube*collisionPoint;return true;}
    return false;
  }
  // CUBE+LINE
  bool collided(const Vector &p0,const Vector &p1, Vector &collisionPoint) const {
    Matrix m1; m1.translate(center.xyz());  m1 *= rotation.rotationMatrix();  m1.scale3(size);
    return collided(m1, p0, p1, collisionPoint);
  }
};

// LINE+TRIANGLE
double signedVolume(const Vector &a, const Vector &b, const Vector &c, const Vector &d) {return (1.0/6.0)*dot(cross(b-a,c-a),d-a);}
// LINE+TRIANGLE
bool collided(const Vector &l0,const Vector &l1, const Vector &t0, const Vector &t1, const Vector &t2) {
  const double s0 = signedVolume(l0,t0,t1,t2);
  const double s1 = signedVolume(l1,t0,t1,t2);
  if (s0*s1>=0) return false;
  const double a0 = signedVolume(l0,l1,t0,t1);
  const double a1 = signedVolume(l0,l1,t1,t2);
  if (a0*a1<0) return false;
  const double a2 = signedVolume(l0,l1,t2,t0);
  if (a1*a2<0) return false;
  return true;
}
// LINE+TRIANGLE (to be called after collided returned true (it's just a ray collision not a line))
bool collisionPoint(const Vector &l0,const Vector &l1, const Vector &t0, const Vector &t1, const Vector &t2, Vector &collisionPoint) {
  Vector e1 = t1-t0;
  Vector e2 = t2-t0;
  Vector rd = normalize(l1-l0); // ray dir
  Vector n = cross(e1,e2);
  double det = -dot(rd,n);
  double invdet = det!=0?1.0/det:1.0;
  Vector a0 = l0 - t0;
  Vector da0 = cross(a0,rd);
  double u = dot(e2,da0)*invdet;
  double v = -dot(e1,da0)*invdet;
  double t = dot(a0,n)*invdet;
  bool hit = fabs(det)>=1e-6&&t>=0.0&&u>=0.0&&v>=0.0&&(u+v)<=1.0;
  if (hit) collisionPoint = l0 + t * rd;
  return hit;
}

#endif // __COLL_HPP__
