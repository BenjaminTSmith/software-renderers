#ifndef RAYTRACER_H
#define RAYTRACER_H

#include <random>

#include "render.h"

// TODO(Ben): potentially use pcg hash instead of trashy C++ random stl

static double random_double() {
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    static std::mt19937 generator;
    return distribution(generator);
}

static Vec3 random_vector() {
    return normalize(Vec3(random_double() * 2 - 1, random_double() * 2 - 1, random_double() * 2 - 1));
}

// a is start b is end
struct Ray {
    Vec3 origin;
    Vec3 direction;

    Ray() {}
    Ray(const Vec3& origin, const Vec3& direction) : origin(origin), direction(direction) {}

    Vec3 at(double t) const {
        return origin + t * direction;
    }
};

enum ObjectType {
    Sphere
};

struct HitRecord {
    Vec3 point;
    Vec3 normal;
    double t;
    bool front_face;
    Vec3 color;
};

// Raytracing code for now
bool hit_sphere(const Vec3& center, double radius, const Ray& ray, double tmin, double tmax, HitRecord& hit_record);

struct Object {
    ObjectType object_type;
    Vec3 center;
    double radius;
    Vec3 color;

    bool hit(const Ray& ray, double tmin, double tmax, HitRecord& hit_record) const {
        switch (object_type) {
            case Sphere: {
                return hit_sphere(center, radius, ray, tmin, tmax, hit_record);
            } default: {
                return false;
            }
        }
    }
};

static Object create_sphere(Vec3 center, double radius, Color color) {
    Object object;
    object.object_type = Sphere;
    object.center = center;
    object.radius = radius;
    object.color.x = color.r / 255.0;
    object.color.y = color.g / 255.0;
    object.color.z = color.b / 255.0;
    return object;
}

void render(Color framebuffer[], Object scene[], int object_count);
bool hit_scene(const Ray& ray, Object scene[], int object_count, HitRecord& hit_record);

extern Vec3 camera;
extern Vec3 camera_direction;
extern Vec3 up;
extern Vec3 right;
extern double focal_length;
extern const int samples_per_pixel;

#endif // !RENDERER_H
