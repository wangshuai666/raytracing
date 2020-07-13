#ifndef RAY_H
#define RAY_H

#include "vec3.h"

class ray {
public:
  vec3 orig;
  vec3 dir;

public:
  ray() {}
  ray(const vec3 &origin, const vec3 &direction)
      : orig(origin), dir(direction) {}
  inline vec3 origin() const { return orig; }
  inline vec3 direction() const { return dir; }

  vec3 at(double t) const { return orig + t * dir; }
};

#endif