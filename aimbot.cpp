#include "aimbot.hpp"
#include "offsets.hpp"
#include <iostream>
#include <algorithm>
#include <vector>

aimbot::aimbot() {

}
aimbot::~aimbot() {

}

float aimbot::mag3D(Vec3 vec) {
	return sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
}

Vec3 aimbot::calcAngle3D(Vec3 src, Vec3 dst) {
	Vec3 angles;
	Vec3 delta = src - dst;
	float hyp = mag3D(delta);
	angles.x = (float)(asinf(delta.z / hyp)) * 180.0f / pi;
	angles.y = (float)(atanf(delta.y / delta.x)) * 180.0f / pi;
	if (delta.x >= 0.0f) angles.y += 180.0f;
	return angles;
}

Vec3 aimbot::clamp(Vec3 angles) {
	if (angles.y > 180.0f) angles.y = 180.0f;
	else if (angles.y < -180.0f) angles.y = -180.0f;
	if (angles.x > 89.0f) angles.x = 89.0f;
	else if (angles.x < -89.0f) angles.x = -89.0f;
	angles.z = 0.0f;
	return angles;
}

Vec3 aimbot::normalize(Vec3 angles) {
	while (angles.x > 89.0f) angles.x -= 180.0f;
	while (angles.x < -89.0f) angles.x += 180.0f;
	while (angles.y > 180.0f) angles.y -= 360.0f;
	while (angles.y < -180.0f) angles.y += 360.0f;
	return angles;
}

float aimbot::RandomFloat(float min, float max) {
	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = max - min;
	float r = random * diff;
	return min + r;
}

/*bool aimbot::isSpotted(Entity* localPlayer, Entity* target) {
	int mask;
	mask = *(int*)((uintptr_t)target + Offsets::m_bSpottedByMask);
	return (bool)(mask & (1 << (localPlayer->clientId() - 1)));
}*/

bool aimbot::isVisible(Entity* localPlayer, Entity* target) {
	CTraceFilter tracefilter;
	Ray_t ray;
	CGameTrace trace;

	Vec3 me = localPlayer->origin() + localPlayer->viewOffset();
	Vec3 en = target->getBonePos(7);

	tracefilter.pSkip = (void*)localPlayer;
	ray.Init(me, en);
	
	Interfaces::g_EngineTrace->TraceRay(ray, MASK_SHOT | CONTENTS_GRATE, &tracefilter, &trace);
	if (target == trace.hit_entity) {
		return true;
	}
	else {
		return false;
	}
}

void aimbot::aimbotFOV(Entity* localPlayer, EntList* entityList, Vec3* viewAngles, float aimbotFOV, float aimbotSmooth, bool clearTarget, bool bRCSAimbot) {
	//Clear target lock
	if (aimbotSmooth == 0) {
		doRageAimbot(localPlayer, viewAngles, entityList, aimbotFOV, bRCSAimbot);
		return;
	}
	if (clearTarget == true) {
		aimTarget = nullptr;
	}
	if (aimTarget == nullptr) {
		//Get new best target based on FOV
		targetList.clear();
		for (unsigned short int i = 0; i < 32; i++) {
			if (entityList->entityListObjs[i].entity == NULL) continue;
			Entity* target = entityList->entityListObjs[i].entity;
			if (target->dormant() || target->lifeState() != 0 || target->health() < 1 || target->team() == localPlayer->team()) continue;
			float dist = abs(mag3D(calcTarget(localPlayer, viewAngles, target)));
			if (dist <= aimbotFOV) {
				TList potentialTarget(target, dist);
				targetList.push_back(potentialTarget);
			}
		}
		std::sort(targetList.begin(), targetList.end());
		for (std::vector<TList>::iterator it = targetList.begin(); it != targetList.end(); it++) {
			if (!isVisible(localPlayer, it->target)) continue;
			aimTarget = it->target;
			break;
		}
	}
	if (aimTarget) {
		float dynFOV = 0.0f;
		dynFOV = localPlayer->shotsFired() * 0.5f;
		Vec3 targetPoint = calcTarget(localPlayer, viewAngles, aimTarget);
		if (targetPoint.y > 180) targetPoint.y -= 360;
		if (targetPoint.y < -180) targetPoint.y += 360;

		if (aimTarget->dormant() == 0 && aimTarget->lifeState() == 0 && aimTarget->health() > 1 && abs(mag3D(targetPoint)) <= (aimbotFOV + dynFOV) && aimbotFOV > 0 && aimbotSmooth > 0) {
			if (isVisible(localPlayer, aimTarget)) {
				doAimbot(localPlayer, viewAngles, targetPoint, aimbotSmooth, bRCSAimbot);
			}
		}
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(50));
}

void aimbot::doAimbot(Entity* localPlayer, Vec3* viewAngles, Vec3 targetPoint, float aimbotSmooth, bool bRCSAimbot) {
	float aimbotSmoothRand = 2.5f;
	float aimbotOver = 0.14f;
	Vec3 myAngle = *viewAngles;

	if (bRCSAimbot && localPlayer->shotsFired() > 2) {
		float aimbotSmoothRandRCS = 1.0f;
		float RCSSmoothRand = 0.1f;
		Vec3 currentPunch = localPlayer->aimPunchAngle() * 2;
		myAngle.x += (targetPoint.x - (currentPunch.x + RandomFloat(-1 * RCSSmoothRand, RCSSmoothRand))) / (aimbotSmooth + RandomFloat(0, aimbotSmoothRandRCS));
		myAngle.y += (targetPoint.y - (currentPunch.y + RandomFloat(-1 * RCSSmoothRand, RCSSmoothRand))) / (aimbotSmooth + RandomFloat(0, aimbotSmoothRandRCS));
		myAngle.x += RandomFloat(-1 * aimbotOver, aimbotOver);
		myAngle.y += RandomFloat(-1 * aimbotOver, aimbotOver);
	}
	else {
		myAngle.x += (targetPoint.x / (aimbotSmooth + RandomFloat(0, aimbotSmoothRand))) + RandomFloat(-1 * aimbotOver, aimbotOver);
		myAngle.y += (targetPoint.y / (aimbotSmooth + RandomFloat(0, aimbotSmoothRand))) + RandomFloat(-1 * aimbotOver, aimbotOver);
	}

	if (myAngle.y < -180.0f) myAngle.y = 179.99999f;
	if (myAngle.y > 180.0f) myAngle.y = -179.99999f;
	normalize(myAngle);
	clamp(myAngle);
	viewAngles->x = myAngle.x;
	viewAngles->y = myAngle.y;
}

void aimbot::doRageAimbot(Entity* localPlayer, Vec3* viewAngles, EntList* entityList, float aimbotFOV, bool bRCSAimbot) {
	targetList.clear();
	for (unsigned short int i = 0; i < 32; i++) {
		if (entityList->entityListObjs[i].entity == NULL) continue;
		Entity* target = entityList->entityListObjs[i].entity;
		if (target->dormant() || target->lifeState() != 0 || target->health() < 1 || target->team() == localPlayer->team()) continue;
		float dist = abs(mag3D(calcTarget(localPlayer, viewAngles, target)));
		if (dist <= aimbotFOV) {
			TList potentialTarget(target, dist);
			targetList.push_back(potentialTarget);
		}
	}
	std::sort(targetList.begin(), targetList.end());
	for (std::vector<TList>::iterator it = targetList.begin(); it != targetList.end(); it++) {
		if (!isVisible(localPlayer, it->target)) continue;
		aimTarget = it->target;
		break;
	}
	if (aimTarget) {
		float dynFOV = 0.0f;
		dynFOV = localPlayer->shotsFired() * 0.5f;
		Vec3 targetPoint = calcTarget(localPlayer, viewAngles, aimTarget);
		if (targetPoint.y > 180) targetPoint.y -= 360;
		if (targetPoint.y < -180) targetPoint.y += 360;

		if (aimTarget->dormant() == 0 && aimTarget->lifeState() == 0 && aimTarget->health() > 1 && abs(mag3D(targetPoint)) <= (aimbotFOV + dynFOV) && aimbotFOV > 0) {
			if (isVisible(localPlayer, aimTarget)) {
				Vec3 myAngle = *viewAngles;
				Vec3 target = calcTarget(localPlayer, viewAngles, aimTarget);
				if (target.y > 180) target.y -= 360;
				if (target.y < -180) target.y += 360;
				if (abs(target.x) <= (aimbotFOV) && abs(target.y) <= (aimbotFOV) && target.x != 0 && target.y != 0 && aimbotFOV > 0) {
					myAngle.x += target.x;
					myAngle.y += target.y;
					if (myAngle.y < -180.0f) myAngle.y = 179.99999f;
					if (myAngle.y > 180.0f) myAngle.y = -179.99999f;

					//RCS stuff
					if (bRCSAimbot) {
						Vec3 currentPunch = localPlayer->aimPunchAngle() * 2;
						if (localPlayer->shotsFired() > 1) {
							myAngle = myAngle - currentPunch;
						}
					}
					normalize(myAngle);
					clamp(myAngle);
					viewAngles->x = myAngle.x;
					viewAngles->y = myAngle.y;
				}
			}
		}
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(50));
}

Vec3 aimbot::calcTarget(Entity* localPlayer, Vec3* viewAngles, Entity* targetEnt) {
	Vec3 bones = targetEnt->getBonePos(7);
	Vec3 myeyes = localPlayer->origin() + localPlayer->viewOffset();
	Vec3 aimDest = calcAngle3D(myeyes, bones);

	Vec3 diff = aimDest - *viewAngles;
	if (diff.y > 180) diff.y -= 360;
	if (diff.y < -180) diff.y += 360;
	return diff;
}

void aimbot::RCS(Entity* localPlayer, Vec3* viewAngles) {
	if (localPlayer->shotsFired() > 1) {
		float RCSSmooth = 0.1f;
		Vec3 currentPunch = localPlayer->aimPunchAngle() * 2;
		Vec3 myAngle = *viewAngles + oldAngle - currentPunch;
		myAngle.x += RandomFloat(-1 * RCSSmooth, RCSSmooth);
		myAngle.y += RandomFloat(-1 * RCSSmooth, RCSSmooth);
		normalize(myAngle);
		clamp(myAngle);
		viewAngles->x = myAngle.x;
		viewAngles->y = myAngle.y;
		oldAngle = currentPunch;
	}
	else {
		oldAngle = { 0, 0, 0 };
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(50));
}