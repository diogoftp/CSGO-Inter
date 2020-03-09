#include "vec3.hpp"

Vec3 Add(Vec3 src, Vec3 dst) {
    Vec3 sum;
    sum.x = src.x + dst.x;
    sum.y = src.y + dst.y;
    sum.z = src.z + dst.z;
    return sum;
}

Vec3 Sub(Vec3 src, Vec3 dst) {
    Vec3 sum;
    sum.x = src.x - dst.x;
    sum.y = src.y - dst.y;
    sum.z = src.z - dst.z;
    return sum;
}

float DegreeToRadian(float degree) {
    return degree * (PI / 180);
}

void MoveInCameraDirection(Vec3 currAngle, Vec3& src, float dist) {
    Vec3 d;

    d.x = cosf(DegreeToRadian(currAngle.x - 90)) * dist;
    d.y = sinf(DegreeToRadian(currAngle.x - 90)) * dist;
    d.z = sinf(DegreeToRadian(currAngle.y)) * dist;

    src = Add(src, d);
}