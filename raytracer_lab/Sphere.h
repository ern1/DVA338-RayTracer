#ifndef SPHERE_H
#define SPHERE_H

#include "Vec3.h"
#include "Ray.h"
#include "Stats.h"

extern Stats stats;

class Sphere {
public:
	Vec3f c; // the center of the sphere
	float r; // radius

	// Material properties
	Vec3f ka, kd, ks;
	float sh{};

public:
	Sphere(const Vec3f & cen, const float rad) : c(cen), r(rad), ka(), kd(), ks(){}

	void set_mat_props(const Vec3f & a, const Vec3f & d, const Vec3f & s, const float shine) { ka = a; kd = d; ks = s; sh = shine; }
	bool hit(const Ray & r, HitRec & rec) const;
	void compute_surface_hit_fields(const Ray & r, HitRec & rec) const;
};

#endif