#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>

#include "rasterizer.h"
#include "render.h"

float z_buffer[width * height];

Vec3 barycentric(Vec3 v0, Vec3 v1, Vec3 v2, Vec3 p) {
    Vec3 edge0 = Vec3(v2.x - v0.x, v1.x - v0.x, v0.x - p.x);
    Vec3 edge1 = Vec3(v2.y - v0.y, v1.y - v0.y, v0.y - p.y);
    Vec3 u = cross(edge0, edge1);
    if (std::abs(u.z) < 1) {
        return Vec3(-1, -1, -1);
    }
    return Vec3(1.0 - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
}

void sweep_triangle(Color* framebuffer, Vec3 v0, Vec3 v1, Vec3 v2, Color color) {
    Vec2 bboxmin;
    Vec2 bboxmax;
    bboxmin.x = std::min(v0.x, std::min(v1.x, v2.x));
    bboxmin.y = std::min(v0.y, std::min(v1.y, v2.y));
    bboxmax.x = std::max(v0.x, std::max(v1.x, v2.x));
    bboxmax.y = std::max(v0.y, std::max(v1.y, v2.y));

    bboxmin.x = std::max(0.0, std::min(width - 1.0, bboxmin.x));
    bboxmin.y = std::max(0.0, std::min(height - 1.0, bboxmin.y));
    bboxmax.x = std::min(width - 1.0, std::max(0.0, bboxmax.x));
    bboxmax.y = std::min(height - 1.0, std::max(0.0, bboxmax.y));

    for (int x = bboxmin.x; x < bboxmax.x; x++) {
        for (int y = bboxmin.y; y < bboxmax.y; y++) {
            Vec3 barycentric_coords = barycentric(v0, v1, v2, Vec3(x, y, 0));
            if (barycentric_coords.x < 0 || barycentric_coords.y < 0 || barycentric_coords.z < 0) {
                continue;
            }
            double z = v0.z * barycentric_coords.x + v1.z * barycentric_coords.y + v2.z * barycentric_coords.z;
            if (z < z_buffer[y * width + x]) {
                z_buffer[y * width + x] = z;
                framebuffer[y * width + x] = color;
            }
        }
    }
}

void render(Color* framebuffer, const Camera& camera, Model models[], int model_count) {
    // NOTE(Ben): weird white artifacts/pixels near mesh edges
    // I dont think so anymore - Justin
    Mesh mesh = load_mesh("head.obj");
    for (int i = 0; i < width * height; i++) {
        z_buffer[i] = INFINITY;
        framebuffer[i] = Color(0, 0, 0);
    }
    double tangent = tan(45.0 / 2.0 * (3.1415926535 / 180));
    double top = 0.1 * tangent;
    double right = top * aspect_ratio;

    // Justin todo: probably move this out
    Mat4 projection;
    projection.m00 = 0.1 / right;
    projection.m11 = 0.1 / top;
    projection.m22 = (0.1 - 1000) / (1000 - 0.1);
    projection.m23 = - (2 * 1000 * 0.1) / (1000 - 0.1);
    projection.m32 = -1;

    Mat4 view = look_at(camera.position, camera.position + camera.direction, Vec3(0, 1, 0));

    for (int i = 0; i < model_count; i++) {
        Model model = models[i];
        
        Mat4 model_mat = translate(model.position);

        for (int i = 0; i < mesh.faces.size(); i++) {
            const std::vector<int>& face = mesh.faces[i];
            Vec3 screen_coords[3];
            Vec3 triangle[3];
            for (int j = 0; j < face.size(); j++) {
                Vec3 v = mesh.vertices[face[j]];
                Vec4 result = projection * (view * (model_mat * Vec4(v.x, v.y, v.z, 1)));
                Vec3 v1 = Vec3(result.x / result.w, result.y / result.w, result.z / result.w);
                int x = (v1.x + 1.0) * width  / 2.0;
                int y = (-v1.y + 1.0) * height / 2.0;
                screen_coords[j] = Vec3(x, y, v1.z);
                triangle[j] = v;
            }
            Vec3 n = normalize(cross(triangle[2] - triangle[0], triangle[1] - triangle[0]));
            double light = dot(n, Vec3(0, 0, -1));
            if (light > 0) {
                sweep_triangle(framebuffer, screen_coords[0], screen_coords[1], screen_coords[2],
                               Color(light * 255, light * 255, light * 255));
            }
        }
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

Mat4 look_at(Vec3 position, Vec3 target, Vec3 up) {
    Vec3 direction = normalize(position - target);
    Vec3 right = normalize(cross(up, direction));
    Mat4 change_of_basis;
    change_of_basis.m00 = right.x;
    change_of_basis.m01 = right.y;
    change_of_basis.m02 = right.z;
    change_of_basis.m10 = up.x;
    change_of_basis.m11 = up.y;
    change_of_basis.m12 = up.z;
    change_of_basis.m20 = direction.x;
    change_of_basis.m21 = direction.y;
    change_of_basis.m22 = direction.z;
    change_of_basis.m33 = 1;
    Mat4 camera_shift;
    camera_shift.m00 = 1;
    camera_shift.m11 = 1;
    camera_shift.m22 = 1;
    camera_shift.m33 = 1;
    camera_shift.m03 = -position.x;
    camera_shift.m13 = -position.y;
    camera_shift.m23 = -position.z;
    return change_of_basis * camera_shift;
}

Mat4 translate(Vec3 translation) {
    Mat4 result;
    result.m00 = 1;
    result.m11 = 1;
    result.m22 = 1;
    result.m33 = 1;
    result.m03 = translation.x;
    result.m13 = translation.x;
    result.m23 = translation.x;
    return result;
}
