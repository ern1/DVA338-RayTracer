#ifndef VEC3_H
#define VEC3_H

#include <algorithm>

template<class T>
class Vec3 {
public:
	union {
		T arr[3];
		struct { T x, y, z; };
		struct { T r, g, b; };
	};

	Vec3() = default;
	Vec3(T xc, T yc, T zc) : x(xc), y(yc), z(zc) { }
	explicit Vec3(const T a[3]){ arr[0]=a[0]; arr[1]=a[1]; arr[2]=a[2]; }

	void set(T xc, T yc, T zc) { x = xc; y = yc; z = zc; }
	void set(const T a[3]) { arr[0]=a[0]; arr[1]=a[1]; arr[2]=a[2]; }
	void set(const Vec3 &v){ arr[0]=v.x; arr[1]=v.y; arr[2]=v.z; }
	
	T len(void) const { return (T) sqrt(x * x + y * y + z * z); }
	T len_sq(void) const { return x * x + y * y + z * z; }
	
	Vec3<T>& normalize(void) { T invLen = T(1.0) / this->len(); x *= invLen; y *= invLen; z *= invLen; return *this; }
	Vec3<T> get_normalized(void) { Vec3<T> c(*this); return c.normalize(); }         
	bool normalize_if(void);
	
	Vec3<T> operator+(const Vec3 &v) const { return Vec3<T>(x + v.x, y + v.y, z + v.z); } 
	void operator+=(const Vec3 &v) { x += v.x; y += v.y; z += v.z; }

	Vec3<T> operator-(const Vec3 &v) const { return Vec3<T>(x - v.x, y - v.y, z - v.z); }
	void operator-=(const Vec3 &v) { x -= v.x; y -= v.y; z -= v.z; }
	
	Vec3<T> operator-(void) const { return Vec3<T>(-x, -y, -z); }
	void flip(void) { x = -x; y = -y; z = -z;  }

	Vec3<T> operator*(T t) const { return Vec3<T>(x * t, y * t, z * t); }
	void operator*=(T t)  { x *= t; y *= t; z *= t; }
	
	T dot(const Vec3 &v) const { return x * v.x + y * v.y + z * v.z; }
	Vec3<T> cross(const Vec3 &v) const { return Vec3<T>(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x); }
	
	Vec3<T> mult_coordwise(const Vec3 &v) const { return Vec3<T>(x * v.x, y * v.y, z * v.z); }

	void div(T d) { x /= d; y /= d; z /= d; }

	void clamp(T min, T max) {
		x = x < min ? min : (x > max ? max : x);
		y = y < min ? min : (y > max ? max : y);
		z = z < min ? min : (z > max ? max : z);
	}
};

template<class T>
bool Vec3<T>::normalize_if(void) { 
	T length = len(); 
	
	if (length > 0.0000001) {
		T inv_len = T(1.0) / length;
		x *= inv_len; 
		y *= inv_len; 
		z *= inv_len;
		return true;
	} else {
		return false;
	}
}

typedef Vec3<float> Vec3f;

#endif