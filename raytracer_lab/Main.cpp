#include <vector>
#include <iostream>
#include <thread>
#include <Windows.h>

#include <glut.h>
#include "Vec3.h"
#include "Image.h"
#include "Ray.h"
#include "Sphere.h"
#include "Camera.h"
#include "LightSource.h"
#include "Stats.h"

#define PI 3.141592f
#define TRACE_DEPTH_LIMIT 100
#define THREADS 4

extern Stats stats = Stats();

class Scene {
public:
	std::vector<Sphere> spheres;		
	Camera * cam;
	LightSource * light;
	int sphere_count;

	Scene(void): cam(nullptr), light(nullptr), sphere_count(0) {}

	void add(Sphere & s) {
		spheres.emplace_back(s);
		std::cout << "Sphere #" << ++sphere_count <<" added: " << "r = " << spheres[spheres.size()-1].r << std::endl;
	}

	void load_spheres(const char * file_name) {
		FILE * fp;
		char buffer[250];
		float x, y, z, rad, ar, ag, ab, dr, dg, db, sr, sg, sb, shine;

		fopen_s(&fp, file_name, "r");
		if (fp == nullptr) {
			printf("ERROR: fopen cant open file");
		}

		while (fgets(buffer, sizeof(buffer), fp) != nullptr) {
			sscanf_s(buffer, "%f %f %f %f %f %f %f %f %f %f %f %f %f %f", &x, &y, &z, &rad, &ar, &ag, &ab, &dr, &dg, &db,
			       &sr, &sg, &sb, &shine);
			Sphere sphere = Sphere(Vec3f(x, y, z), rad);
			shine = shine * 128.0f;
			sphere.set_mat_props(Vec3f(ar, ag, ab), Vec3f(dr, dg, db), Vec3f(sr, sg, sb), shine);

			this->add(sphere);
		}
	}
};

void glSetPixel(const int x, const int y, Vec3f & c) {
	glColor3f(c.r, c.g, c.b);
	glBegin(GL_POINTS);
	glVertex2i(x, y);
	glEnd();
}


class SimpleRayTracer {
private: 
	Scene * scene_;
	Image * image_;

	Vec3f getEyeRayDirection(const int x, const int y) const {
		const float aspect_ratio = image_->get_widthf() / image_->get_heightf();
		const float px = (-1.0f + 2.0f * ((x + 0.5f) / image_->get_widthf())) * tanf(scene_->cam->fov * PI / 180.0f / 2.0f) * aspect_ratio;
		const float py = (1.0f - 2.0f * ((y + 0.5f) / image_->get_heightf())) * tanf(scene_->cam->fov * PI / 180.0f / 2.0f);

		return (Vec3f(px, py, -1.0f) - scene_->cam->position).normalize(); // return ray direction. (Px, Py, -1.0f) is the pixel's coordinates in the camera space.

		/* Px calculation explanation:
		   To go from raster space(x,y) to NDC space we use (x + 0.5f) / image->getWidthf(). This is the normalized pixel position using frame dimensions (range for NDC space is [0, 1]).
		   Taking (NDC coordinates * 2 - 1) gives us coordinates in the screen space (range for screen space is [-1, 1]).
		   * tan(fov / 2) changes how much of the scene we can see.
		   To make Px square it is multiplied by aspect ratio. */
	}

public:
	SimpleRayTracer(Scene * scene, Image * image) {
		this->scene_ = scene;
		this->image_ = image;
	}

	void set_all_pixels() const {
		for (auto y = 0; y < image_->get_height(); y++) {
			for (auto x = 0; x <image_->get_width(); x++) {
				Vec3f pixel = image_->get_pixel(x, y);
				glSetPixel(x, y, pixel);
			}
		}
	}

	void search_closest_hit(const Ray & ray, HitRec & hit_rec) const {
		hit_rec.any_hit = false;
	
		for (auto i = 0; i < scene_->spheres.size(); i++) {
			if (scene_->spheres[i].hit(ray, hit_rec))
				hit_rec.prim_index = i;
		}

		// Get intersection point(p) and normal(n) for the closest hitting sphere
		if(hit_rec.any_hit)
			scene_->spheres[hit_rec.prim_index].compute_surface_hit_fields(ray, hit_rec);
	}

	Vec3f lighting(const Ray & ray, HitRec & hit_rec, const int calls) {
		const Sphere sphere = scene_->spheres[hit_rec.prim_index];
		const Vec3f N = hit_rec.n;
		Vec3f L = (scene_->light->position - hit_rec.p).normalize();
		const Vec3f V = -ray.d;
		Vec3f R = N * (2 * L.dot(N)) - L; // r = 2(l.n)n-l

		const Vec3f ambient_comp = scene_->light->la.mult_coordwise(sphere.ka);
		Vec3f color = ambient_comp;

		/* Shadow ray */
		Ray s_ray;
		HitRec s_rec;
		s_ray.d = L;
		s_ray.o = hit_rec.p + (s_ray.d * 0.5f); // Set origin a short distance away from the hit point to avoid self collision
		search_closest_hit(s_ray, s_rec);

		// If no hit with shadow ray, add diffuse and specular to color
		if ( !s_rec.any_hit) {
			const Vec3f diffuse_comp = scene_->light->ld.mult_coordwise(sphere.kd) * max(L.dot(N), 0.0f);
			const Vec3f specular_comp = scene_->light->ls.mult_coordwise(sphere.ks) * pow(max(R.dot(V), 0.0f), sphere.sh);
			color += diffuse_comp + specular_comp;
		}

		/* Reflection ray */
		Ray r_ray;
		HitRec r_rec;
		float s = 2 * (ray.d.dot(hit_rec.n));
		Vec3f scalar = Vec3f(s, s, s);
		r_ray.d = ray.d - (scalar.mult_coordwise(hit_rec.n));
		r_ray.o = hit_rec.p + (r_ray.d * 0.5f);

		color += ray_trace(r_ray, r_rec, calls - 1).mult_coordwise(sphere.ks * 0.2);

		return color;
	}

	Vec3f ray_trace(const Ray & ray, HitRec & hit_rec, const int calls) {
		/* End of recursive calls */
		if (calls <= 0) {
			return {0.0f, 0.0f, 0.0f};
		}

		search_closest_hit(ray, hit_rec);

		Vec3f color = hit_rec.any_hit ? lighting(ray, hit_rec, calls) : Vec3f(0.258f, 0.756f, 0.957f);

		/* Before returning the color, divide it by the number of reflection rays to get an average color (else it could have values over 1) */
		// TODO - fix this
		int numRay = TRACE_DEPTH_LIMIT - calls;
		//return (numRay = 0) ? color : color * (1.0f / (numRay));
		color.div(numRay);
		return color;
	}

	void fire_rays(const int start_x, const int jump_x) { 
		Ray ray;
		HitRec hit_rec;

		ray.o = scene_->cam->position; // Set the starting position of the eye rays to the origin

		for (auto y = 0; y < image_->get_height(); y++) {
			for (auto x = start_x; x < image_->get_width(); x += jump_x) {
				ray.d = getEyeRayDirection(x, y);
				
				Vec3f color = ray_trace(ray, hit_rec, TRACE_DEPTH_LIMIT);

				color.clamp(0.0f, 1.0f);
				image_->set_pixel(x, y, color);
			}
		}
	}
};

SimpleRayTracer * ray_tracer;

void start_render_threads(const int number_of_threads) {
	std::vector<std::thread> vec_thr;

	for (auto i = 0; i < number_of_threads; i++) {
		//SetThreadPriority(vecThr.back().native_handle(), THREAD_PRIORITY_ABOVE_NORMAL); // don't need this
		vec_thr.emplace_back(&SimpleRayTracer::fire_rays, ray_tracer, i, number_of_threads);
		std::cout << std::boolalpha << std::endl << "Thread created: " << vec_thr.back().get_id() << "\tPriority: " <<
			GetThreadPriority(vec_thr.back().native_handle()) << "\tJoinable: " << vec_thr.back().joinable();
	}

	for (auto& t : vec_thr) 
		if(t.joinable()) t.join();	
}

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if (!stats.render_complete) {
		stats.start_timer();
		
		/* Start render threads */
		start_render_threads(THREADS);

		/* Start rendering with the maximum number of concurrent threads supported */
		//start_render_threads(thread::hardware_concurrency());

		/* Render using no seperate threads */
		//rayTracer->fireRays(0, 1);

		stats.render_complete = TRUE;
		std::cout << std::endl << "Rendertime: " << stats.get_time() << " seconds" << std::endl;

		if (stats.ray_int_test_count != 0)
			std::cout << "Executed ray-sphere intersection tests: " << stats.ray_int_test_count << std::endl;
	}

	ray_tracer->set_all_pixels();

	glFlush();
}

void change_size(const int w, const int h) {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, w, 0, h);
	glViewport(0, 0, w, h);
}

void init(void) {
	/* Set resolution */
	Image * image = new Image(1920, 1080);
	//Image * image = new Image(2560, 1440);
	//Image * image = new Image(3840, 2160);

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(image->get_width(), image->get_height());
	glutCreateWindow("SimpleRayTracer");
	glutDisplayFunc(display);
	glutReshapeFunc(change_size);
	//glutKeyboardFunc(keypress);

	glClearColor(0.0f, 1.0f, 0.0f, 1.0f);

	Scene * scene = new Scene;

	scene->load_spheres("spheres1k.txt");
	scene->cam = new Camera(Vec3f(0.0f, 0.0f, 0.0f), 75.0f);
	scene->light = new LightSource(Vec3f(-50.0f, -100.0f, 50.0f), Vec3f(1.0f, 1.0f, 1.0f), Vec3f(1.0f, 1.0f, 1.0f), Vec3f(1.0f, 1.0f, 1.0f));

	ray_tracer = new SimpleRayTracer(scene, image);
}

void main(int argc, char **argv) {
	glutInit(&argc, argv);
	init();
	glutMainLoop();
}
