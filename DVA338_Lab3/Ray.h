#ifndef RAY_H
#define RAY_H

#include "Vec3.h"

// Represents the ray that is ray(t) = pos + t * dir, where (t_min <= t <= t_max)
class Ray {
public:
	Vec3f o, d;				// ray origin and ray direction
	float t_clip;
	static float ray_eps;
	static float t_max;
public:
	Ray() : o(), d(), t_clip(t_max){}
	Ray(const Vec3f & o, const Vec3f& d, float tclip = t_max) : o(o), d(d), t_clip(t_max) { }
	
	void eps_move_start_along_surface_normal(const Vec3f & n) { o = o + n * ray_eps; }
	void eps_move_start_along_dir() { o = o + d * ray_eps; }
	
	static void inc_ray_eps() { ray_eps *= 10.0f; }
	static void dec_ray_eps() { ray_eps /= 10.0f; }
};


class HitRec {
public:	
	float t_hit{};		// t-value of closest sphere the the ray hit
	int prim_index{};	// index of the closest sphere (or primitive) that the ray hit
	Vec3f p;		// point of intersection
	Vec3f n;		// normal for p
	bool any_hit{};	// if ray hit anything

public:
	HitRec() : p(), n(){};
};

#endif