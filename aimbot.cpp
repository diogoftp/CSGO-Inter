#include "aimbot.hpp"
#include "offsets.hpp"

Vec3 oldAngle;
Entity* aimTarget = nullptr;

float mag3D(Vec3 vec) {
	return sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
}

Vec3 calcAngle3D(Vec3 src, Vec3 dst) {
	Vec3 angles;
	Vec3 delta = src - dst;
	float hyp = mag3D(delta);
	angles.x = (float)(asinf(delta.z / hyp)) * 180.0f / pi;
	angles.y = (float)(atanf(delta.y / delta.x)) * 180.0f / pi;
	//angles.x = atanf(delta.z / hyp) * 180.0f / pi;
	//angles.y = atanf(delta.y / delta.x) * 180.0f / pi;
	angles.z = 0.0f;
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

int isSpotted(Entity* localPlayer, Entity* target) {
	int mask;
	mask = *(int*)((uintptr_t)target + offsets::m_bSpottedByMask);
	return (bool)(mask & (1 << (localPlayer->clientId() - 1)));
}

void aimbotbyFOV(Entity* localPlayer, EntList* entityList, Vec3* viewAngles, float aimbotFOV, float aimbotSmooth, bool clearTarget, bool bRCSAimbot) {
	float aimbotOver = 0.14f;
	float aimbotSmoothRand = 2.5f;
	float RCSSmoothRand = 0.35f;
	float dynFOV = 0.0f;
	dynFOV = *(int*)((uintptr_t)localPlayer + offsets::m_iShotsFired) * 0.7f;
	Vec3 myAngle = *viewAngles;

	//Clear target lock
	if (clearTarget == true) {
		aimTarget = nullptr;
	}

	if (aimTarget == nullptr) {
		aimTarget = getTarget(localPlayer, viewAngles, entityList, aimbotFOV + dynFOV);
	}
	else {
		Vec3 target = calcTarget(localPlayer, viewAngles, aimTarget);
		if (target.y > 180) target.y -= 360;
		if (target.y < -180) target.y += 360;
		if (abs(target.x) <= (aimbotFOV + dynFOV) && abs(target.y) <= (aimbotFOV + dynFOV) && target.x != 0 && target.y != 0 && aimbotFOV > 0 && aimbotSmooth >= 0) {
			if (aimbotSmooth == 0) {
				myAngle.x += target.x;
				myAngle.y += target.y;
				myAngle.z = 0.0f;
				normalize(myAngle);
				clamp(myAngle);
				viewAngles->x = myAngle.x;
				viewAngles->y = myAngle.y;
				viewAngles->z = myAngle.z;
			}
			else {
				myAngle.x += (target.x / (aimbotSmooth + RandomFloat(0, aimbotSmoothRand))) + RandomFloat(-1 * aimbotOver, aimbotOver);
				myAngle.y += (target.y / (aimbotSmooth + RandomFloat(0, aimbotSmoothRand))) + RandomFloat(-1 * aimbotOver, aimbotOver);
				if (myAngle.y < -180.0f) myAngle.y = 179.99999f;
				if (myAngle.y > 180.0f) myAngle.y = -179.99999f;
				myAngle.z = 0.0f;

				//RCS stuff
				if (bRCSAimbot) {
					if (*(int*)((uintptr_t)localPlayer + offsets::m_iShotsFired) > 3) {
						Vec3 currentPunch = *(Vec3*)((uintptr_t)localPlayer + offsets::m_viewPunchAngle);
						myAngle.x -= (currentPunch.x * 2.5f) + RandomFloat(-1 * RCSSmoothRand, RCSSmoothRand);
						myAngle.y -= (currentPunch.y * 2.0f) + RandomFloat(-1 * RCSSmoothRand, RCSSmoothRand);
					}
				}

				normalize(myAngle);
				clamp(myAngle);
				viewAngles->x = myAngle.x;
				viewAngles->y = myAngle.y;
				viewAngles->z = myAngle.z;
			}
		}
	}
	Sleep(50);
}

Entity* getTarget(Entity* localPlayer, Vec3* viewAngles, EntList* entityList, float aimbotFOV) {
	float oldDistance = FLT_MAX;
	float newDistance = 0;
	Entity* targetEnt = nullptr;

	for (unsigned short int i = 0; i < 32; i++) {
		if (entityList->entityListObjs[i].entity && isSpotted(localPlayer, entityList->entityListObjs[i].entity) && entityList->entityListObjs[i].entity->clientId() != localPlayer->clientId() && entityList->entityListObjs[i].entity->lifeState() == 0 && entityList->entityListObjs[i].entity->health() > 0 && entityList->entityListObjs[i].entity->team() != localPlayer->team() && entityList->entityListObjs[i].entity->dormant() != TRUE) {
			uintptr_t BaseAddress = (uintptr_t)(entityList->entityListObjs[i].entity);
			uintptr_t BoneBase = *(uintptr_t*)(BaseAddress + offsets::m_dwBoneMatrix);
			Vec3 bones;
			bones.x = *(float*)(BoneBase + 0x30 * 7 + 0x0C);
			bones.y = *(float*)(BoneBase + 0x30 * 7 + 0x1C);
			bones.z = *(float*)(BoneBase + 0x30 * 7 + 0x2C);
			Vec3 lpView = localPlayer->viewOffset();
			Vec3 myeyes = localPlayer->origin() + lpView;
			Vec3 aimDest = calcAngle3D(myeyes, bones);

			Vec3 diff = aimDest - *viewAngles;
			if (diff.y > 180) diff.y -= 360;
			if (diff.y < -180) diff.y += 360;
			if (abs(diff.x) <= aimbotFOV && abs(diff.y) <= aimbotFOV) {
				newDistance = abs(mag3D(diff));
				if (newDistance < oldDistance) {
					oldDistance = newDistance;
					targetEnt = entityList->entityListObjs[i].entity;
				}
			}
		}
	}
	return targetEnt;
}

Vec3 calcTarget(Entity* localPlayer, Vec3* viewAngles, Entity* targetEnt) {
	Vec3 target;
	if (targetEnt && isSpotted(localPlayer, targetEnt) && targetEnt->clientId() != localPlayer->clientId() && targetEnt->lifeState() == 0 && targetEnt->health() > 0 && targetEnt->team() != localPlayer->team() && targetEnt->dormant() != TRUE) {
		uintptr_t BaseAddress = (uintptr_t)(targetEnt);
		uintptr_t BoneBase = *(uintptr_t*)(BaseAddress + offsets::m_dwBoneMatrix);
		Vec3 bones;
		bones.x = *(float*)(BoneBase + 0x30 * 7 + 0x0C);
		bones.y = *(float*)(BoneBase + 0x30 * 7 + 0x1C);
		bones.z = *(float*)(BoneBase + 0x30 * 7 + 0x2C);
		Vec3 lpView = localPlayer->viewOffset();
		Vec3 myeyes = localPlayer->origin() + lpView;
		Vec3 aimDest = calcAngle3D(myeyes, bones);

		Vec3 diff = aimDest - *viewAngles;
		if (diff.y > 180) diff.y -= 360;
		if (diff.y < -180) diff.y += 360;
		target = diff;
	}
	return target;
}

void RCS(Entity* localPlayer, EntList* entityList, Vec3* viewAngles) {
	if (*(int*)((uintptr_t)localPlayer + offsets::m_iShotsFired) > 1) {
		Vec3 currentPunch = *(Vec3*)((uintptr_t)localPlayer + offsets::m_aimPunchAngle);
		Vec3 myAngle = *viewAngles;
		myAngle.x += oldAngle.x;
		myAngle.y += oldAngle.y;

		normalize(myAngle);
		clamp(myAngle);
		viewAngles->x = myAngle.x - currentPunch.x * 2;
		viewAngles->y = myAngle.y - currentPunch.y * 2;
		oldAngle.x = currentPunch.x * 2;
		oldAngle.y = currentPunch.y * 2;
	}
	else {
		oldAngle.x = 0.0f;
		oldAngle.y = 0.0f;
	}
}

/*bool IsVisible(Entity* pLocal, Entity* pEnt) {
		Ray_t ray;
		trace_t tr;
		Vector localEye, entEye;
		if (GetBonePosition(pLocal, localEye, 10) && GetBonePosition(pEnt, entEye, 10)) {
			ray.Init(localEye, entEye);
			CTraceFilter traceFilter;
			traceFilter.pSkip = pLocal;
			g_pTrace->TraceRay(ray, 0x4600400B, &traceFilter, &tr);

			if (tr.allsolid || tr.startsolid)
				return false;

			return tr.fraction > 0.97f;
		}
	return false;
}*/

/*Vec3 getBestFOV(Entity* localPlayer, Vec3* viewAngles, EntList* entityList, float aimbotFOV) {
	float oldDistance = FLT_MAX;
	float newDistance = 0;
	Vec3 target;

	for (unsigned short int i = 0; i < 32; i++) {
		if (entityList->entityListObjs[i].entity && isSpotted(localPlayer, entityList->entityListObjs[i].entity) && entityList->entityListObjs[i].entity->clientId() != localPlayer->clientId() && entityList->entityListObjs[i].entity->lifeState() == 0 && entityList->entityListObjs[i].entity->health() > 0 && entityList->entityListObjs[i].entity->team() != localPlayer->team() && entityList->entityListObjs[i].entity->dormant() != TRUE) {
			uintptr_t BaseAddress = (uintptr_t)(entityList->entityListObjs[i].entity);
			uintptr_t BoneBase = *(uintptr_t*)(BaseAddress + offsets::m_dwBoneMatrix);
			Vec3 bones;
			bones.x = *(float*)(BoneBase + 0x30 * 7 + 0x0C);
			bones.y = *(float*)(BoneBase + 0x30 * 7 + 0x1C);
			bones.z = *(float*)(BoneBase + 0x30 * 7	 + 0x2C);
			Vec3 lpView = localPlayer->viewOffset();
			Vec3 myeyes = localPlayer->origin() + lpView;
			Vec3 aimDest = calcAngle3D(myeyes, bones);

			Vec3 diff = aimDest - *viewAngles;
			if (diff.y > 180) diff.y -= 360;
			if (diff.y < -180) diff.y += 360;
			if (abs(diff.x) <= aimbotFOV && abs(diff.y) <= aimbotFOV) {
				newDistance = abs(mag3D(diff));
				if (newDistance < oldDistance) {
					oldDistance = newDistance;
					target = diff;
				}
			}
		}
	}
	return target;
}*/

/*void aimbotRCS(Entity* localPlayer, EntList* entityList, Vec3* viewAngles, float aimbotFOV, float aimbotSmooth, bool clearTarget) {
	float aimbotOver = 0.14f;
	float aimbotSmoothRand = 2.5f;
	float RCSSmoothRand = 0.35f;
	float dynFOV = 0.0f;
	dynFOV = *(int*)((uintptr_t)localPlayer + offsets::m_iShotsFired) * 0.7f;
	Vec3 myAngle = *viewAngles;

	//Clear target lock
	if (clearTarget == true) {
		aimTarget = nullptr;
	}

	if (aimTarget == nullptr) {
		aimTarget = getTarget(localPlayer, viewAngles, entityList, aimbotFOV + dynFOV);
	}
	else {
		Vec3 target = calcTarget(localPlayer, viewAngles, aimTarget);
		if (target.y > 180) target.y -= 360;
		if (target.y < -180) target.y += 360;
		if (abs(target.x) <= (aimbotFOV + dynFOV) && abs(target.y) <= (aimbotFOV + dynFOV) && target.x != 0 && target.y != 0 && aimbotFOV > 0 && aimbotSmooth >= 0) {
			if (aimbotSmooth == 0) {
				myAngle.x += target.x;
				myAngle.y += target.y;
				myAngle.z = 0.0f;
				normalize(myAngle);
				clamp(myAngle);
				viewAngles->x = myAngle.x;
				viewAngles->y = myAngle.y;
				viewAngles->z = myAngle.z;
			}
			else {
				myAngle.x += (target.x / (aimbotSmooth + RandomFloat(0, aimbotSmoothRand))) + RandomFloat(-1 * aimbotOver, aimbotOver);
				myAngle.y += (target.y / (aimbotSmooth + RandomFloat(0, aimbotSmoothRand))) + RandomFloat(-1 * aimbotOver, aimbotOver);
				if (myAngle.y < -180.0f) myAngle.y = 179.99999f;
				if (myAngle.y > 180.0f) myAngle.y = -179.99999f;
				myAngle.z = 0.0f;

				//RCS stuff
				if (*(int*)((uintptr_t)localPlayer + offsets::m_iShotsFired) > 3) {
					Vec3 currentPunch = *(Vec3*)((uintptr_t)localPlayer + offsets::m_viewPunchAngle);
					myAngle.x -= (currentPunch.x * 2.5f) + RandomFloat(-1 * RCSSmoothRand, RCSSmoothRand);
					myAngle.y -= (currentPunch.y * 2.0f) + RandomFloat(-1 * RCSSmoothRand, RCSSmoothRand);
				}

				normalize(myAngle);
				clamp(myAngle);
				viewAngles->x = myAngle.x;
				viewAngles->y = myAngle.y;
				viewAngles->z = myAngle.z;
			}
		}
	}
	Sleep(50);
}*/