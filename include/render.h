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

struct Vec4 {
    double x, y, z, w;
    Vec4(double x, double y, double z, double w) : x(x), y(y), z(z), w(w) {}
    Vec4() {}
};

struct Mat4 {
    double m00, m01, m02, m03;
    double m10, m11, m12, m13;
    double m20, m21, m22, m23;
    double m30, m31, m32, m33;
    Mat4() {
        m00 = 0;
        m01 = 0;
        m02 = 0;
        m03 = 0;
        m10 = 0;
        m11 = 0;
        m12 = 0;
        m13 = 0;
        m20 = 0;
        m21 = 0;
        m22 = 0;
        m23 = 0;
        m30 = 0;
        m31 = 0;
        m32 = 0;
        m33 = 0;
    }
};

// NOTE(Ben): Hopefully this is right
static Vec4 operator*(const Mat4& mat, const Vec4& vec) {
    return Vec4(
        mat.m00 * vec.x + mat.m01 * vec.y + mat.m02 * vec.z + mat.m03 * vec.w,
        mat.m10 * vec.x + mat.m11 * vec.y + mat.m12 * vec.z + mat.m13 * vec.w,
        mat.m20 * vec.x + mat.m21 * vec.y + mat.m22 * vec.z + mat.m23 * vec.w,
        mat.m30 * vec.x + mat.m31 * vec.y + mat.m32 * vec.z + mat.m33 * vec.w
    );
}

struct Camera {
};

#endif // !RENDER_H
