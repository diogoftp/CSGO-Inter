#pragma once
#include "SDK/SDK.hpp"

namespace Interfaces {
	void Initialize();
}
//inline IBaseClientDLL* g_BaseClientDLL = nullptr;
//inline IVEngineClient* g_EngineClient = nullptr;
//inline IClientEntityList* g_EntityList = nullptr;
//inline IEngineTrace* g_EngineTrace = nullptr;

void* CaptureInterface(const char* moduleName, const char* interfaceName);
void* EasyInterface(const char* _Module, const char* _Object);
uintptr_t GetOffset(RecvTable* table, const char* tableName, const char* netvarName);
uintptr_t GetNetVarOffset(const char* tableName, const char* netvarName, ClientClass* clientClass);