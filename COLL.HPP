#ifndef __COLL_HPP__
#define __COLL_HPP__

#include "vector.hpp"
#include "matrix.hpp"
#include "quaternn.hpp"

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

  Vector closestPointOnLineSegment(const Vector &A, const Vector &B, const Vector &Point) {
    Vector AB = B - A;
    double k = dot(AB,AB);
    if (k == 0) k = 1;
    double t = dot(Point-A,AB) / k;
    return A + saturated(t) * AB;
  }

  bool collided(const CLCapsule &b, Vector *collisionVector = NULL) {
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

// Cube-Collision
class CLCube {
public:
  Vector center;
  Vector size; // xyz from -size to +size
  Quaternion rotation;
  
  CLCube() {}
  CLCube(const Vector &center,const Vector &size,const Vector &rotation) : center(center), size(size), rotation(rotation) {}

  bool collided(CLCube &c2) {
    // Separating Axis Theorem
    Matrix m1,m2;
    m1.identity();  m1.translate(center);  m1 *= rotation.rotationMatrix();  m1.scale3(size);
    m2.identity(); m2.translate(c2.center); m2 *= c2.rotation.rotationMatrix(); m2.scale3(c2.size);
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
        if (length(axis[j])<0.0000001) 
          continue;
      }
      // no axis normalize needed
      double min1,min2,max1,max2;
      for (int i = 0; i < 8; i++) {
        Vector p;
        p.x = (i & 1) ? -1 : 1;
        p.y = (i & 2) ? -1 : 1;
        p.z = (i & 4) ? -1 : 1;
        Vector p1 = m1 * p;
        Vector p2 = m2 * p;
        double dist1 = dot(p1,axis[j]);
        double dist2 = dot(p2,axis[j]);
        if (i == 0||dist1<min1) min1 = dist1;
        if (i == 0||dist2<min2) min2 = dist2;
        if (i == 0||dist1>max1) max1 = dist1;
        if (i == 0||dist2>max2) max2 = dist2;
      }
      //bool collision = (max1>min2&&max2>min1);
      bool collision = (min1>min2&&min1<max2)||(min2>min1&&min2<max1);
      if (!collision) 
        return false;
    }
    return true; // all axis reported a collision (but check collisionVector length later, too...(because something seems not 100% alright here))
  }

  Vector collisionVector(CLCube &c2) {
    // Separating Axis Theorem
    // object must have collided before
    Matrix m1,m2;
    m1.identity();  m1.translate(center);  m1 *= rotation.rotationMatrix();  m1.scale3(size);
    m2.identity(); m2.translate(c2.center); m2 *= c2.rotation.rotationMatrix(); m2.scale3(c2.size);
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
        if (length(axis[j])<0.0000001) 
          continue;
      }
      axis[j] = normalize(axis[j]); // for right coordinates
      double min1,min2,max1,max2;
      for (int i = 0; i < 8; i++) {
        Vector p;
        p.x = (i & 1) ? -1 : 1;
        p.y = (i & 2) ? -1 : 1;
        p.z = (i & 4) ? -1 : 1;
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
};

#endif // __COLL_HPP__
