#pragma once
#include "SDK/SDK.hpp"
#include "gui.hpp"

struct TList {
	Entity* target = nullptr;
	float dist = 0.0f;

	TList(Entity* t, const float& d) : target(t), dist(d) {}

	bool operator< (const TList& cs) const {
		return (dist < cs.dist);
	}
};

namespace aimbot {
	float mag3D(Vec3 src);
	Vec3 calcAngle3D(Vec3 src, Vec3 dst);
	Vec3 clamp(Vec3 angles);
	Vec3 normalize(Vec3 angles);
	float RandomFloat(float min, float max);

	//bool isSpotted(Entity* localPlayer, Entity* target);
	bool isVisible(Entity* localPlayer, Entity* target);
	bool smokeCheck(Vec3 src, Vec3 dst);
	void aimbotFOV(Entity* localPlayer, EntList* entityList, Vec3* viewAngles, bool clearTarget, GUI::GUIStruct GUIProps);
	void doAimbot(Entity* localPlayer, Vec3* viewangles, Vec3 targetPoint);
	void doRageAimbot(Entity* localPlayer, Vec3* viewAngles, EntList* entityList);
	Vec3 calcTarget(Entity* localPlayer, Vec3* viewAngles, Entity* targetEnt);
	//void RCS(Entity* localPlayer, Vec3* viewAngles);
}