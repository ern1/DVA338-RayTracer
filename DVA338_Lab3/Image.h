#ifndef IMAGE_H
#define IMAGE_H

#include "Vec3.h"

class Image { 
private:
	int width_, height_;
	Vec3f * pixels_;

public:
	Image(const int w, const int h) : width_(w), height_(h) {
		pixels_ = new Vec3f[width_ * height_];
	}
	~Image(void) {
		delete [] pixels_;
	}

	int get_width(void) const { return width_; }
	int get_height(void) const { return height_; }
	float get_widthf(void) const { return float(width_); }
	float get_heightf(void) const { return float(height_); }

	void set_pixel(const int x, const int y, const Vec3f & c) const { pixels_[y * width_ + x] = c; }
	Vec3f get_pixel(const int x, const int y) const { return pixels_[y * width_ + x]; }
	Vec3f * get_pixel_buffer_ptr(void) const { return pixels_; }
};

#endif