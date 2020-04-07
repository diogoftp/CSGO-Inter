#include "scorewall.hpp"
#include <WinUser.h>
#include <Windows.h>

float Dist3D(Vec3 myCoords, Vec3 enemyCoords) {
	return sqrt(
		pow((enemyCoords.x - myCoords.x), 2) +
		pow((enemyCoords.y - myCoords.y), 2) +
		pow((enemyCoords.z - myCoords.z), 2));
}

bool WorldToScreen(GUI::GUIStruct GUIProps, ViewMatrix_t matrix, Vec3 pos, float screen[]) {
	float clipCoords[16];
	clipCoords[0] = pos.x * matrix.matrix[0] + pos.y * matrix.matrix[1] + pos.z * matrix.matrix[2] + matrix.matrix[3];
	clipCoords[1] = pos.x * matrix.matrix[4] + pos.y * matrix.matrix[5] + pos.z * matrix.matrix[6] + matrix.matrix[7];
	clipCoords[2] = pos.x * matrix.matrix[8] + pos.y * matrix.matrix[9] + pos.z * matrix.matrix[10] + matrix.matrix[11];
	clipCoords[3] = pos.x * matrix.matrix[12] + pos.y * matrix.matrix[13] + pos.z * matrix.matrix[14] + matrix.matrix[15];

	if (clipCoords[3] < 0.1f) return false;

	Vec3 NDC;
	NDC.x = clipCoords[0] / clipCoords[3];
	NDC.y = clipCoords[1] / clipCoords[3];
	NDC.z = clipCoords[2] / clipCoords[3];

	screen[0] = (GUIProps.right / 2 * NDC.x) + (NDC.x + GUIProps.right / 2);
	screen[1] = -(GUIProps.bottom / 2 * NDC.y) + (NDC.y + GUIProps.bottom / 2);
	return true;
}

void scoreWall(GUI::GUIStruct GUIProps, Entity* localPlayer, Vec3* viewAngles, EntList* entityList) {
	float closestEntityDistance = 999999999.0f;
	float EnemyXY[2] = { 0 };
	ViewMatrix_t ViewMatrix;
	Vec3 closestEntity;
	ViewMatrix = *(ViewMatrix_t*)(Offsets::dwClient + Offsets::dwViewMatrix);
	bool achou = false;

	for (unsigned short int i = 0; i < 32; i++) {
		if (!entityList->entityListObjs[i].entity) continue;
		Entity* entity = entityList->entityListObjs[i].entity;
		if (entity == localPlayer || entity->dormant() || entity->lifeState() != 0 || entity->health() < 1 || entity->team() == localPlayer->team()) continue;
		Vec3 entityPosition = entity->origin() + entity->viewOffset();
		Vec3 myPosition = localPlayer->origin() + localPlayer->viewOffset();
		if (WorldToScreen(GUIProps, ViewMatrix, entityPosition, EnemyXY)) {
			float distance = Dist3D(myPosition, entityPosition);
			if (EnemyXY[0] > GUIProps.right || EnemyXY[1] > GUIProps.bottom || EnemyXY[0] < 0 || EnemyXY[1] < 0) continue;
			if (distance < closestEntityDistance) {
				closestEntityDistance = distance;
				closestEntity = entityPosition;
				achou = true;
			}
		}
	}
	if (achou == true && WorldToScreen(GUIProps, ViewMatrix, closestEntity, EnemyXY)) {
		if (EnemyXY[0] < GUIProps.right && EnemyXY[1] < GUIProps.bottom && EnemyXY[0] > 0 && EnemyXY[1] > 0) {
			SetCursorPos((int)EnemyXY[0], (int)EnemyXY[1]);
		}
	}
}