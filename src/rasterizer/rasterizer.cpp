#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>

#include "rasterizer.h"
#include "render.h"

float z_buffer[width * height] = {};

// adapted from tinyrenderer
void bresenham_line(Color* framebuffer, float x0, float x1, float y0, float y1, Color color) {
    bool steep = false; 
    if (std::abs(x0 - x1) < std::abs(y0 - y1)) { 
        std::swap(x0, y0); 
        std::swap(x1, y1); 
        steep = true; 
    } 
    if (x0 > x1) { 
        std::swap(x0, x1); 
        std::swap(y0, y1); 
    } 
    int dx = x1 - x0; 
    int dy = y1 - y0; 
    int derror2 = std::abs(dy) * 2; 
    int error2 = 0; 
    int y = y0; 
    for (int x = x0; x <= x1; x++) { 
        if (steep) { 
            framebuffer[x * width + y] = color;
        } else { 
            framebuffer[y * width + x] = color;
        } 
        error2 += derror2; 
        if (error2 > dx) { 
            y += (y1 > y0 ? 1 : -1); 
            error2 -= dx * 2; 
        } 
    } 
}

void sweep_triangle(Color* framebuffer, Vec2 v0, Vec2 v1, Vec2 v2, Color color) {
    // sort by y coordinate
    if (v0.y == v1.y && v0.y == v2.y) return;
    if (v0.y > v1.y) {
        std::swap(v0, v1);
    }
    if (v0.y > v2.y) {
        std::swap(v0, v2); 
    }
    if (v1.y > v2.y) {
        std::swap(v1, v2); 
    }
    int height = v2.y - v0.y; 
    for (int y = v0.y; y <= v1.y; y++) { 
        int segment_height = v1.y - v0.y + 1; 
        float alpha = (float)(y - v0.y) / height; 
        float beta  = (float)(y - v0.y) / segment_height; // be careful with divisions by zero 
        Vec2 A = v0 + (v2 - v0) * alpha; 
        Vec2 B = v0 + (v1 - v0) * beta; 
        if (A.x > B.x) {
            std::swap(A, B);
        }
        for (int x = A.x; x <= B.x; x++) { 
            framebuffer[y * width + x] = color;
        } 
    } 
    for (int y = v1.y; y <= v2.y; y++) { 
        int segment_height =  v2.y - v1.y + 1; 
        float alpha = (float)(y - v0.y) / height; 
        float beta  = (float)(y - v1.y) / segment_height; // be careful with divisions by zero 
        Vec2 A = v0 + (v2 - v0) * alpha; 
        Vec2 B = v1 + (v2 - v1) * beta; 
        if (A.x > B.x) {
            std::swap(A, B);
        }
        for (int x = A.x; x <= B.x; x++) { 
            framebuffer[y * width + x] = color;
        } 
    }
}

void render(Color* framebuffer) {
    auto mesh = load_mesh("head.obj");
    for (int i = 0; i < width * height; i++) {
        z_buffer[i] = 0;
    }
    for (int i = 0; i < mesh.faces.size(); i++) {
        const std::vector<int>& face = mesh.faces[i];
        /*Vec3 v0 = mesh.vertices[face[0]];
        v0.x = (v0.x + 1) * (width - 1) / 2.0;
        v0.y = (-v0.y + 1) * (height - 1) / 2.0;
        Vec3 v1 = mesh.vertices[face[1]];
        v1.x = (v1.x + 1) * (width - 1) / 2.0;
        v1.y = (-v1.y + 1) * (height - 1) / 2.0;
        Vec3 v2 = mesh.vertices[face[2]];
        v2.x = (v2.x + 1) * (width - 1) / 2.0;
        v2.y = (-v2.y + 1) * (height - 1) / 2.0;*/
        // sweep_triangle(framebuffer, v0, v1, v2, Color(0, 0, 0));
        Vec2 screen_coords[3];
        Vec3 triangle[3];
        for (int j = 0; j < face.size(); j++) {
            Vec3 v = mesh.vertices[face[j]];
            // Vec3 v1 = mesh.vertices[face[(j + 1) % face.size()]];
            int x = (v.x + 1.0) * (width - 1) / 2.0; 
            int y = (-v.y + 1.0) * (height - 1) / 2.0; 
            screen_coords[j] = Vec2(x, y);
            triangle[j] = v;
            // int x1 = (v1.x + 1.0) * width / 2.0; 
            // int y1 = (v1.y + 1.0) * height / 2.0; 
            // bresenham_line(framebuffer, x0, x1, y0, y1, Color(0, 0, 0));
        }
        Vec3 n = normalize(cross(triangle[2] - triangle[0], triangle[1] - triangle[0]));
        double light = dot(n, Vec3(0, 0, -1));
        sweep_triangle(framebuffer, screen_coords[0], screen_coords[1], screen_coords[2],
                       Color(light * 255, light * 255, light * 255));
    }
}

std::vector<std::string> split(const std::string& line, char delimiter) {
    std::vector<std::string> result;
    int i = 0;
    int j = 0;
    while (j < line.size()) {
        if (line[j] == delimiter) {
            if (i != j) {
                result.push_back(line.substr(i, j - i));
            }
            j++;
            i = j;
            continue;
        }
        j++;
    }
    if (i != j) {
        result.push_back(line.substr(i, j - i));
    }

    return result;
}

Mesh load_mesh(const std::string& mesh_file) {
    // TODO(Ben): should only support triangles, so potentially split quad faces into triangles
    std::ifstream file(mesh_file);
    Mesh mesh;
    if (!file.is_open()) {
        std::cout << "Failed to open " << mesh_file << std::endl;
    }
    std::string line;
    while (std::getline(file, line)) {
        std::vector<std::string> split_line = split(line);
        if (split_line.size() == 0) {
            continue;
        }
        if (split_line[0] == "v") {
            Vec3 vertex;
            vertex.x = std::stod(split_line[1]);
            vertex.y = std::stod(split_line[2]);
            vertex.z = std::stod(split_line[3]);
            mesh.vertices.push_back(vertex);
        } else if (split_line[0] == "f") {
            std::vector<int> face;
            for (int i = 1; i < split_line.size(); i++) {
                face.push_back(std::stoi(split(split_line[i], '/')[0]) - 1);
            }
            mesh.faces.push_back(face);
        } else {
            continue;
        }
    }
    return mesh;
}
