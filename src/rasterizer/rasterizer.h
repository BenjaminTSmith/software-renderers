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

void render(Color* framebuffer);
std::vector<std::string> split(const std::string& line, char delimiter=' ');
Mesh load_mesh(const std::string& mesh_file);
void sweep_triangle(Color* framebuffer, Vec2 v0, Vec2 v1, Vec2 v2, Color color);

#endif // !RASTERIZER_H
