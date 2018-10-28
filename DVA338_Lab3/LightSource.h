#ifndef LIGHTSOURCE_H
#define LIGHTSOURCE_H

#include "Vec3.h"

class LightSource {
public:
	Vec3f position;
	Vec3f la, ld, ls;

public:
	LightSource() = default;
	LightSource(Vec3f & pos, Vec3f & a, Vec3f & d, Vec3f & s) : position(pos), la(a), ld(d), ls(s) {}

	void set_position(const float x, const float y, const float z) { position.x = x; position.y = y; position.z = z; }

	void change_pos_x(const float x) { position.x += x; }
	void change_pos_y(const float y) { position.y += y; }
	void change_pos_z(const float z) { position.z += z; }
};

#endif