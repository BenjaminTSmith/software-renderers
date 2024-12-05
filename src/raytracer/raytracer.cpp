#include "raytracer.h"
#include "render.h"

#include <cmath>
#include <limits>

Vec3 camera(0, 0, 3);
Vec3 camera_direction = normalize(Vec3(0, 0, 1));
Vec3 up(0, 1, 0);
Vec3 right = cross(camera_direction, up);
double focal_length = 3; // this feels pretty good for now. can tweak if needed
const int samples_per_pixel = 1;

void render(Color framebuffer[], Object scene[], int object_count) {
    double viewport_height = 2.0;
    double viewport_width = viewport_height * aspect_ratio;

    // from https://raytracing.github.io/
    // For the raytracer we use right handed coordinates. Positive x is to the right,
    // positive y is up, positive z is backwards

    // camera basis vectors
    Vec3 w = camera_direction;
    Vec3 u = normalize(cross(up, w));
    Vec3 v = cross(w, u);

    u = viewport_width * u;
    v = -viewport_height * v;

    Vec3 du = u / width;
    Vec3 dv = v / height;

    Vec3 viewport_origin = camera - (focal_length * w) - u / 2 - v / 2;
    Vec3 pixel_origin = viewport_origin + 0.5 * (du + dv);

    for (int i = 0; i < width * height; i++) {
        Vec3 color_total(0, 0, 0);
        for (int j = 0; j < samples_per_pixel; j++) {
            double random_in_square = random_double() - 0.5;
            Vec3 pixel_center = pixel_origin + (((i % width) + random_in_square) * du) +
                ((((int)(i / width)) + random_in_square) * dv);
            Vec3 ray_direction = pixel_center - camera;

            Vec3 unit_direction = normalize(ray_direction);
            double a = 0.5 * (unit_direction.y + 1.0);

            int max_bounces = 5;
            Ray ray(camera, ray_direction);
            Vec3 color(((1 - a) + a * 0.5) * 255, ((1 - a) + a * 0.7) * 255, ((1 - a) + a * 1.0) * 255);
            HitRecord hit_record;
            while (hit_scene(ray, scene, object_count, hit_record) && max_bounces > 0) {
                // TODO(Ben): The way we are doing random vector seeding is causing the visual artifacts
                Vec3 direction = random_vector() + hit_record.normal;
                /*if (dot(direction, hit_record.normal) < 0) {
                    direction = -1 * direction;
                }*/
                ray = Ray(hit_record.point, direction);
                color = hit_record.color * color;
                max_bounces--;
            }
            color_total = color_total + color;
        }
        framebuffer[i] = Color(color_total.x / samples_per_pixel,
                               color_total.y / samples_per_pixel,
                               color_total.z / samples_per_pixel);
    }
}

bool hit_scene(const Ray& ray, Object scene[], int object_count, HitRecord& hit_record) {
    bool hit = false;
    double closest = std::numeric_limits<double>::infinity();
    for (int j = 0; j < object_count; j++) {
        if (scene[j].hit(ray, 0.001, closest, hit_record)) {
            hit = true;
            closest = hit_record.t;
            hit_record.color = scene[j].color;
        }
    }
    return hit;
}

bool hit_sphere(const Vec3& center, double radius,
                const Ray& ray, double tmin, double tmax, HitRecord& hit_record) {
    Vec3 to_sphere = center - ray.origin;
    double a = dot(ray.direction, ray.direction);
    double h = dot(ray.direction, to_sphere);
    double c = dot(to_sphere, to_sphere) - radius * radius;

    double discriminant = h * h - a * c;
    if (discriminant < 0) {
        return false;
    }
    double sqrt_discriminant = std::sqrt(discriminant);
    double root = (h - sqrt_discriminant) / a;
    if (root <= tmin || tmax <= root) {
        root = (h + sqrt_discriminant) / a;
        if (root <= tmin || tmax <= root) {
            return false;
        }
    }
    
    hit_record.t = root;
    hit_record.point = ray.at(root);
    hit_record.normal = (hit_record.point - center) / radius;
    hit_record.front_face = dot(ray.direction, hit_record.normal) < 0;
    hit_record.normal = hit_record.front_face ? hit_record.normal : -1 * hit_record.normal;

    return true;
}
