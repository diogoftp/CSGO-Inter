#pragma once
#include <Windows.h>
#include <d3d9.h>
#include <d3dx9.h>

bool Detour32(char* src, char* dst, const intptr_t len);
char* TrampHook32(char* src, char* dst, const intptr_t len);
int startHook();
int endHook();