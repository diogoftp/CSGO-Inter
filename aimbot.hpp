#pragma once
#include "SDK/SDK.hpp"

const float pi = 3.14159265358979323846f;

//Vec3 add3D(Vec3 src, Vec3 dst);
//Vec3 sub3D(Vec3 src, Vec3 dst);
float mag3D(Vec3 src);
Vec3 calcAngle3D(Vec3 src, Vec3 dst);
Vec3 clamp(Vec3 angles);
Vec3 normalize(Vec3 angles);
void aimbotbyFOV(Entity* localPlayer, EntList* entityList, Vec3* viewAngles, float aimbotFOV, float aimbotSmooth);
int isSpotted(Entity* localPlayer, Entity* target);
float RandomFloat(float min, float max);
void RCS(Entity* localPlayer, EntList* entityList, Vec3* viewAngles);
void aimbotRCS(Entity* localPlayer, EntList* entityList, Vec3* viewAngles, float aimbotFOV, float aimbotSmooth);