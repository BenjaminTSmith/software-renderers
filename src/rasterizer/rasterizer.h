#ifndef RASTERIZER_H
#define RASTERIZER_H

#include "render.h"

#include <vector>
#include <string>

extern float z_buffer[width * height];

struct Mesh {
    std::vector<Vec3> vertices;
    std::vector<std::vector<int>> faces;
};

struct Model {
    Mesh mesh;
    Vec3 position;
};

struct Camera {
    Vec3 position;
    Vec3 direction;
};

void render(Color* framebuffer, const Camera& camera, Model models[], int model_count);
std::vector<std::string> split(const std::string& line, char delimiter=' ');
Mesh load_mesh(const std::string& mesh_file);
void sweep_triangle(Color* framebuffer, Vec3 v0, Vec3 v1, Vec3 v2, Color color);
Mat4 look_at(Vec3 position, Vec3 target, Vec3 up);
Mat4 translate(Vec3 translation);

#endif // !RASTERIZER_H
