#pragma once
#include <thread>
#include <chrono>
#include "SDK/SDK.hpp"
#include "interfaces.hpp"

class aimbot {
private:
	const float pi = 3.14159265358979323846f;
	tTraceRay TraceRay;
	Vec3 oldAngle = { 0.0f, 0.0f, 0.0f };
	Entity* aimTarget = nullptr;
public:
	aimbot();
	~aimbot();
	float mag3D(Vec3 src);
	Vec3 calcAngle3D(Vec3 src, Vec3 dst);
	Vec3 clamp(Vec3 angles);
	Vec3 normalize(Vec3 angles);
	float RandomFloat(float min, float max);

	bool isSpotted(Entity* localPlayer, Entity* target);
	bool isVisible(Entity* localPlayer, Entity* target);

	void rageAimbot(Entity* localPlayer, EntList* entityList, Vec3* viewAngles, float aimbotFOV, bool bRCSAimbot);
	void aimbotbyFOV(Entity* localPlayer, EntList* entityList, Vec3* viewAngles, float aimbotFOV, float aimbotSmooth, bool clearTarget, bool bRCSAimbot);
	Entity* getTarget(Entity* localPlayer, Vec3* viewAngles, EntList* entityList, float aimbotFOV);
	Vec3 calcTarget(Entity* localPlayer, Vec3* viewAngles, Entity* targetEnt);

	void RCS(Entity* localPlayer, Vec3* viewAngles);
};