#pragma once
#include <cmath>
#include <limits>

const float PI = 3.141592654f;

class Vec3 {
public:
    Vec3() {
        x = 0;
        y = 0;
        z = 0;
    }
    //Vec3(void) {
    //    Invalidate();
    //}
    Vec3(float X, float Y, float Z) {
        x = X;
        y = Y;
        z = Z;
    }
    Vec3(const float* clr) {
        x = clr[0];
        y = clr[1];
        z = clr[2];
    }

    void Init(float ix = 0.0f, float iy = 0.0f, float iz = 0.0f) {
        x = ix; y = iy; z = iz;
    }
    bool IsValid() const {
        return std::isfinite(x) && std::isfinite(y) && std::isfinite(z);
    }
    void Invalidate() {
        x = y = z = std::numeric_limits<float>::infinity();
    }

    float& operator[](int i) {
        return ((float*)this)[i];
    }
    float operator[](int i) const {
        return ((float*)this)[i];
    }

    void Zero() {
        x = y = z = 0.0f;
    }

    bool operator==(const Vec3& src) const {
        return (src.x == x) && (src.y == y) && (src.z == z);
    }
    bool operator!=(const Vec3& src) const {
        return (src.x != x) || (src.y != y) || (src.z != z);
    }

    Vec3& operator+=(const Vec3& v) {
        x += v.x; y += v.y; z += v.z;
        return *this;
    }
    Vec3& operator-=(const Vec3& v) {
        x -= v.x; y -= v.y; z -= v.z;
        return *this;
    }
    Vec3& operator*=(float fl) {
        x *= fl;
        y *= fl;
        z *= fl;
        return *this;
    }
    Vec3& operator*=(const Vec3& v) {
        x *= v.x;
        y *= v.y;
        z *= v.z;
        return *this;
    }
    Vec3& operator/=(const Vec3& v) {
        x /= v.x;
        y /= v.y;
        z /= v.z;
        return *this;
    }
    Vec3& operator+=(float fl) {
        x += fl;
        y += fl;
        z += fl;
        return *this;
    }
    Vec3& operator/=(float fl) {
        x /= fl;
        y /= fl;
        z /= fl;
        return *this;
    }
    Vec3& operator-=(float fl) {
        x -= fl;
        y -= fl;
        z -= fl;
        return *this;
    }

    void NormalizeInPlace() {
        *this = Normalized();
    }
    Vec3 Normalized() const {
        Vec3 res = *this;
        float l = res.Length();
        if (l != 0.0f) {
            res /= l;
        }
        else {
            res.x = res.y = res.z = 0.0f;
        }
        return res;
    }

    float DistTo(const Vec3& vOther) const {
        Vec3 delta;

        delta.x = x - vOther.x;
        delta.y = y - vOther.y;
        delta.z = z - vOther.z;

        return delta.Length();
    }
    float DistToSqr(const Vec3& vOther) const {
        Vec3 delta;

        delta.x = x - vOther.x;
        delta.y = y - vOther.y;
        delta.z = z - vOther.z;

        return delta.LengthSqr();
    }
    float Dot(const Vec3& vOther) const {
        return (x * vOther.x + y * vOther.y + z * vOther.z);
    }
    float Length() const {
        return sqrt(x * x + y * y + z * z);
    }
    float LengthSqr(void) const {
        return (x * x + y * y + z * z);
    }
    float Length2D() const {
        return sqrt(x * x + y * y);
    }

    Vec3& operator=(const Vec3& vOther) {
        x = vOther.x; y = vOther.y; z = vOther.z;
        return *this;
    }

    Vec3 operator-(void) const {
        return Vec3(-x, -y, -z);
    }
    Vec3 operator+(const Vec3& v) const {
        return Vec3(x + v.x, y + v.y, z + v.z);
    }
    Vec3 operator-(const Vec3& v) const {
        return Vec3(x - v.x, y - v.y, z - v.z);
    }
    Vec3 operator*(float fl) const {
        return Vec3(x * fl, y * fl, z * fl);
    }
    Vec3 operator*(const Vec3& v) const {
        return Vec3(x * v.x, y * v.y, z * v.z);
    }
    Vec3 operator/(float fl) const {
        return Vec3(x / fl, y / fl, z / fl);
    }
    Vec3 operator/(const Vec3& v) const {
        return Vec3(x / v.x, y / v.y, z / v.z);
    }

    float x, y, z;
};

inline Vec3 operator*(float lhs, const Vec3& rhs) {
    return rhs * lhs;
}
inline Vec3 operator/(float lhs, const Vec3& rhs) {
    return rhs / lhs;
}

float DegreeToRadian(float degree);

void MoveInCameraDirection(Vec3 currAngle, Vec3& src, float dist);