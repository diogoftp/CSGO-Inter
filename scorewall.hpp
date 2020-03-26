#pragma once
#include "SDK/SDK.hpp"
#include "gui.hpp"

typedef struct {
	float matrix[16];
} ViewMatrix_t;


float Dist3D(Vec3 myCoords, Vec3 enemyCoords);
bool WorldToScreen(GUI::GUIStruct GUIProps, ViewMatrix_t matrix, Vec3 pos, float screen[]);
void scoreWall(GUI::GUIStruct GUIProps, Entity* localPlayer, Vec3* viewAngles, EntList* entityList);