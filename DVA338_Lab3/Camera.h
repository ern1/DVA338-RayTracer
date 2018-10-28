#ifndef CAMERA_H
#define CAMERA_H

#include "Vec3.h"

class Camera {
public:	
	Vec3f position;
	float fov{};						// Horizontal field-of-view

public:
	Camera() = default;
	Camera(Vec3f & pos, const float f) : position(pos), fov(f){}

	void set_position(const float x, const float y, const float z) { position.x = x; position.y = y; position.z = z; }
	//void set_direction(float x, float y, float z) { direction.x = x; direction.y = y; direction.z = z; }

	void change_pos_x(float x) { position.x += x; }
	void change_pos_y(float y) { position.y += y; }
	void change_pos_z(float z) { position.z += z; }
	//void change_dir_x(float x) { direction.x += x; position.normalize(); }
	//void change_dir_y(float y) { direction.y += y; position.normalize(); }
	//void change_dir_z(float z) { direction.z += z; position.normalize(); }
};

#endif