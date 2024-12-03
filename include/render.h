#ifndef RENDER_H
#define RENDER_H

#include <cmath>

constexpr int width = 800;
constexpr int height = 450;

constexpr float aspect_ratio = (float)width / height;

struct Color {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    Color(unsigned char r, unsigned char g, unsigned char b) : r(r), g(g), b(b) {}
    Color() {}
};

struct Vec3 {
    double x, y, z;
    Vec3(double x, double y, double z) : x(x), y(y), z(z) {}
    Vec3() {}
};

static Vec3 operator+(const Vec3& left, const Vec3& right) {
    return Vec3(left.x + right.x, left.y + right.y, left.z + right.z);
}

static Vec3 operator-(const Vec3& left, const Vec3& right) {
    return Vec3(left.x - right.x, left.y - right.y, left.z - right.z);
}

static Vec3 operator*(const Vec3& left, const Vec3& right) {
    return Vec3(left.x * right.x, left.y * right.y, left.z * right.z);
}

static Vec3 operator*(double scalar, const Vec3& vec) {
    return Vec3(scalar * vec.x, scalar * vec.y, scalar * vec.z);
}

static Vec3 operator/(const Vec3& vec, double scalar) {
    return Vec3(vec.x / scalar, vec.y / scalar, vec.z / scalar);
}

static double dot(const Vec3& left, const Vec3& right) {
    return left.x * right.x + left.y * right.y + left.z * right.z;
}

static Vec3 cross(const Vec3& left, const Vec3& right) {
    return Vec3(
        left.y * right.z - left.z * right.y,
        left.z * right.x - left.x * right.z,
        left.x * right.y - left.y * right.x
    );
}

static double magnitude(const Vec3& vec) {
    return std::sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
}

static Vec3 normalize(const Vec3& vec) {
    return vec / magnitude(vec);
}

struct Vec2 {
    double x, y;
    Vec2(double x, double y) : x(x), y(y) {}
    Vec2() {}
};

static Vec2 operator-(const Vec2 &left, const Vec2 &right) {
    return Vec2(left.x - right.x, left.y - right.y);
}

static Vec2 operator+(const Vec2 &left, const Vec2 &right) {
    return Vec2(left.x + right.x, left.y + right.y);
}

static Vec2 operator*(const Vec2 &left, double scalar) {
    return Vec2(left.x * scalar, left.y * scalar);
}

struct Camera {
};

#endif // !RENDER_H
