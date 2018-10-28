#include "Sphere.h"

bool Sphere::hit(const Ray & r, HitRec & rec) const {	
	/* This uses mutex, which is necessary when using multiple threads (but really slow) */
	//stats.inc_ray_int_test_count();

	/* Instead use this, but with only 1 thread */
	//stats.ray_int_test_count++;


	Vec3f v = c - r.o;
	const float s = v.dot(r.d);
	const float v_len_sq = v.dot(v);
	const float rad_sq = this->r * this->r;
	if (s < 0 && v_len_sq > rad_sq) {
		return false;
	}
	const float m_sq = v_len_sq - s * s;
	if (m_sq > rad_sq) {
		return false;
	}

	const float A = r.d.dot(r.d);
	const float B = 2 * (r.o - this->c).dot(r.d);
	const float C = (r.o - this->c).dot(r.o - this->c) - (this->r * this->r);
	const float D = B * B - 4 * A * C;
	
	// No intersection
	if (D < 0) {
		return false;
	}

	// D > 0 = Ray intersected the sphere. (if D = 0 => t1 = t2 meaning ray graced the sphere)
	const float t1 = (-B + sqrtf(D)) / (2 * A);
	const float t2 = (-B - sqrtf(D)) / (2 * A);

	const float t = t1 < t2 ? t1 : t2;

	// Check if it is the closest one yet and save intersection
	if (!rec.any_hit || t < rec.t_hit) {
		rec.t_hit = t;
		rec.any_hit = true;
		return true;
	}
	return false; 
}

void Sphere::compute_surface_hit_fields(const Ray & r, HitRec & rec) const {
	rec.p = r.o + r.d * rec.t_hit;
	rec.n = (rec.p - c).normalize();
}
