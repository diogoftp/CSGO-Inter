#include "aimbot.hpp"
#include "interfaces.hpp"
#include "vars.hpp"
#include "scorewall.hpp" //WorldToScreen
#include <thread>
#include <chrono>
#include <algorithm>
#include <vector>

extern Globals::myGlobals Vars;

const float pi = 3.14159265358979323846f;
Vec3 oldAngle = { 0.0f, 0.0f, 0.0f };
Entity* aimTarget = nullptr;
std::vector<TList> targetList;

namespace aimbot {
	float mag3D(Vec3 vec) {
		return sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
	}

	Vec3 calcAngle3D(Vec3 src, Vec3 dst) {
		Vec3 angles;
		Vec3 delta = src - dst;
		float hyp = mag3D(delta);
		angles.x = (float)(asinf(delta.z / hyp)) * 180.0f / pi;
		angles.y = (float)(atanf(delta.y / delta.x)) * 180.0f / pi;
		if (delta.x >= 0.0f) angles.y += 180.0f;
		return angles;
	}

	Vec3 clamp(Vec3 angles) {
		if (angles.y > 180.0f) angles.y = 180.0f;
		else if (angles.y < -180.0f) angles.y = -180.0f;
		if (angles.x > 89.0f) angles.x = 89.0f;
		else if (angles.x < -89.0f) angles.x = -89.0f;
		angles.z = 0.0f;
		return angles;
	}

	Vec3 normalize(Vec3 angles) {
		while (angles.x > 89.0f) angles.x -= 180.0f;
		while (angles.x < -89.0f) angles.x += 180.0f;
		while (angles.y > 180.0f) angles.y -= 360.0f;
		while (angles.y < -180.0f) angles.y += 360.0f;
		return angles;
	}

	float RandomFloat(float min, float max) {
		float random = ((float)rand()) / (float)RAND_MAX;
		float diff = max - min;
		float r = random * diff;
		return min + r;
	}

	/*bool isSpotted(Entity* localPlayer, Entity* target) {
		int mask;
		mask = *(int*)((uintptr_t)target + Offsets::m_bSpottedByMask);
		return (bool)(mask & (1 << (localPlayer->clientId() - 1)));
	}*/

	bool smokeCheck(Vec3 src, Vec3 dst) {
		typedef bool(__cdecl* td_LineGoesThroughSmoke)(Vec3, Vec3);
		static td_LineGoesThroughSmoke LineGoesThroughSmoke = (td_LineGoesThroughSmoke)Offsets::LineGoesThroughSmoke;
		return !LineGoesThroughSmoke(src, dst);
	}

	bool isVisible(Entity* localPlayer, Entity* target) {
		CTraceFilter tracefilter;
		Ray_t ray;
		CGameTrace trace;

		Vec3 me = localPlayer->origin() + localPlayer->viewOffset();
		Vec3 en = target->getBonePos(Vars.boneIndex);

		tracefilter.pSkip = (void*)localPlayer;
		ray.Init(me, en);

		Interfaces::g_EngineTrace->TraceRay(ray, MASK_SHOT | CONTENTS_GRATE, &tracefilter, &trace);
		if (target == trace.hit_entity && smokeCheck(me, en)) {
			return true;
		}
		else {
			return false;
		}
	}

	void aimbotFOV(Entity* localPlayer, EntList* entityList, Vec3* viewAngles, bool clearTarget, GUI::GUIStruct GUIProps) {
		//Clear target lock
		if (Vars.aimbotSmooth == 0) {
			doRageAimbot(localPlayer, viewAngles, entityList);
			std::this_thread::sleep_for(std::chrono::milliseconds(5));
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
				if (target == localPlayer || target->dormant() || target->lifeState() != 0 || target->health() < 1 || target->team() == localPlayer->team()) continue;
				float dist = abs(mag3D(calcTarget(localPlayer, viewAngles, target)));
				if (dist <= Vars.aimbotFOV) {
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
			dynFOV = localPlayer->shotsFired() * 0.65f; //0.65f is a magic number
			Vec3 targetPoint = calcTarget(localPlayer, viewAngles, aimTarget);
			if (targetPoint.y > 180) targetPoint.y -= 360;
			if (targetPoint.y < -180) targetPoint.y += 360;

			if (aimTarget->dormant() == 0 && aimTarget->lifeState() == 0 && aimTarget->health() > 1 && abs(mag3D(targetPoint)) <= (Vars.aimbotFOV + dynFOV) && Vars.aimbotFOV > 0 && Vars.aimbotSmooth > 0) {
				if (isVisible(localPlayer, aimTarget)) {
					//Anti shake
					float head[2] = { 0 };
					float neck[2] = { 0 };
					ViewMatrix_t ViewMatrix = *(ViewMatrix_t*)(Offsets::dwClient + Offsets::dwViewMatrix);
					WorldToScreen(GUIProps, ViewMatrix, aimTarget->getBonePos(8), head);
					WorldToScreen(GUIProps, ViewMatrix, aimTarget->getBonePos(7), neck);
					float dif = abs(head[0] - neck[0] + head[1] - neck[1]); //Head to neck distance aspect ratio
					if (abs(mag3D(targetPoint)) > dif * 0.1f || localPlayer->shotsFired() > 0) { //Anti shake, 0.1f is a magic number
						doAimbot(localPlayer, viewAngles, targetPoint);
					}
				}
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}

	void doAimbot(Entity* localPlayer, Vec3* viewAngles, Vec3 targetPoint) {
		Vec3 myAngle = *viewAngles;

		if (Vars.bRCSAimbot && localPlayer->shotsFired() > 1) {
			Vec3 currentPunch = localPlayer->aimPunchAngle() * 2;
			myAngle.x += (targetPoint.x - (currentPunch.x + RandomFloat(-1 * Vars.RCSSmoothRand, Vars.RCSSmoothRand))) / (Vars.aimbotSmooth * 5 + RandomFloat(0, Vars.aimbotSmoothRand * 0.2f));
			myAngle.y += (targetPoint.y - (currentPunch.y + RandomFloat(-1 * Vars.RCSSmoothRand, Vars.RCSSmoothRand))) / (Vars.aimbotSmooth * 5 + RandomFloat(0, Vars.aimbotSmoothRand * 0.2f));
			myAngle.x += RandomFloat(-1 * Vars.aimbotOver, Vars.aimbotOver);
			myAngle.y += RandomFloat(-1 * Vars.aimbotOver, Vars.aimbotOver);
		}
		else {
			myAngle.x += (targetPoint.x / (Vars.aimbotSmooth * 5 + RandomFloat(0, Vars.aimbotSmoothRand))) + RandomFloat(-1 * Vars.aimbotOver, Vars.aimbotOver);
			myAngle.y += (targetPoint.y / (Vars.aimbotSmooth * 5 + RandomFloat(0, Vars.aimbotSmoothRand))) + RandomFloat(-1 * Vars.aimbotOver, Vars.aimbotOver);
		}

		if (myAngle.y < -180.0f) myAngle.y = 179.99999f;
		if (myAngle.y > 180.0f) myAngle.y = -179.99999f;
		normalize(myAngle);
		clamp(myAngle);
		if (aimTarget->onGround()) viewAngles->x = myAngle.x; //Dont move crosshair up when target jumps
		viewAngles->y = myAngle.y;
	}

	void doRageAimbot(Entity* localPlayer, Vec3* viewAngles, EntList* entityList) {
		targetList.clear();
		for (unsigned short int i = 0; i < 32; i++) {
			if (entityList->entityListObjs[i].entity == NULL) continue;
			Entity* target = entityList->entityListObjs[i].entity;
			if (target == localPlayer || target->dormant() || target->lifeState() != 0 || target->health() < 1 || target->team() == localPlayer->team()) continue;
			float dist = abs(mag3D(calcTarget(localPlayer, viewAngles, target)));
			if (dist <= Vars.aimbotFOV) {
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

			if (aimTarget->dormant() == 0 && aimTarget->lifeState() == 0 && aimTarget->health() > 1 && abs(mag3D(targetPoint)) <= (Vars.aimbotFOV + dynFOV) && Vars.aimbotFOV > 0) {
				if (isVisible(localPlayer, aimTarget)) {
					Vec3 myAngle = *viewAngles;
					Vec3 target = calcTarget(localPlayer, viewAngles, aimTarget);
					if (target.y > 180) target.y -= 360;
					if (target.y < -180) target.y += 360;
					if (abs(target.x) <= (Vars.aimbotFOV + dynFOV) && abs(target.y) <= (Vars.aimbotFOV + dynFOV) && target.x != 0 && target.y != 0 && Vars.aimbotFOV > 0) {
						myAngle.x += target.x;
						myAngle.y += target.y;
						if (myAngle.y < -180.0f) myAngle.y = 179.99999f;
						if (myAngle.y > 180.0f) myAngle.y = -179.99999f;

						//RCS stuff
						if (Vars.bRCSAimbot) {
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
	}

	Vec3 calcTarget(Entity* localPlayer, Vec3* viewAngles, Entity* targetEnt) {
		Vec3 bones = targetEnt->getBonePos(Vars.boneIndex) + targetEnt->velocity() * 0.0078125f;
		Vec3 myeyes = localPlayer->origin() + localPlayer->viewOffset();
		Vec3 aimDest = calcAngle3D(myeyes, bones);

		Vec3 diff = aimDest - *viewAngles;
		if (diff.y > 180) diff.y -= 360;
		if (diff.y < -180) diff.y += 360;
		return diff;
	}

	/*void RCS(Entity* localPlayer, Vec3* viewAngles) {
		if (localPlayer->shotsFired() > 1) {
			Vec3 currentPunch = localPlayer->aimPunchAngle() * 2;
			Vec3 myAngle = *viewAngles + oldAngle - currentPunch;
			myAngle.x += RandomFloat(-1 * Vars.RCSSmoothRand, Vars.RCSSmoothRand);
			myAngle.y += RandomFloat(-1 * Vars.RCSSmoothRand, Vars.RCSSmoothRand);
			normalize(myAngle);
			clamp(myAngle);
			viewAngles->x = myAngle.x;
			viewAngles->y = myAngle.y;
			oldAngle = currentPunch;
		}
		else {
			oldAngle = { 0, 0, 0 };
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}*/
}