#pragma once
#include "SDK/SDK.hpp"

const float pi = 3.14159265358979323846f;

float mag3D(Vec3 src);
Vec3 calcAngle3D(Vec3 src, Vec3 dst);
Vec3 clamp(Vec3 angles);
Vec3 normalize(Vec3 angles);
float RandomFloat(float min, float max);

int isSpotted(Entity* localPlayer, Entity* target);

void aimbotbyFOV(Entity* localPlayer, EntList* entityList, Vec3* viewAngles, float aimbotFOV, float aimbotSmooth, bool clearTarget, bool bRCSAimbot);
Entity* getTarget(Entity* localPlayer, Vec3* viewAngles, EntList* entityList, float aimbotFOV);
Vec3 calcTarget(Entity* localPlayer, Vec3* viewAngles, Entity* targetEnt);

void RCS(Entity* localPlayer, EntList* entityList, Vec3* viewAngles);

//Vec3 getBestFOV(Entity* localPlayer, Vec3* viewAngles, EntList* entityList, float aimbotFOV);
//void aimbotRCS(Entity* localPlayer, EntList* entityList, Vec3* viewAngles, float aimbotFOV, float aimbotSmooth, bool clearTarget);