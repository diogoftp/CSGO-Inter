#pragma once
#include "SDK/SDK.hpp"
#include "SDK/IEngineTrace.hpp"

namespace Interfaces {
	void Setup();

	//inline IBaseClientDLL* g_BaseClientDLL = nullptr;
	//inline IVEngineClient* g_EngineClient = nullptr;
	//inline IClientEntityList* g_EntityList = nullptr;
	inline IEngineTrace* g_EngineTrace = nullptr;

	void* CaptureInterface(const char* moduleName, const char* interfaceName);
	void* EasyInterface(const char* _Module, const char* _Object);
	uintptr_t GetOffset(RecvTable* table, const char* tableName, const char* netvarName);
	uintptr_t GetNetVarOffset(const char* tableName, const char* netvarName, ClientClass* clientClass);

	typedef void* (*InstantiateInterface)();
}

class CInterface {
public:
	Interfaces::InstantiateInterface Interface; //0x0000
	char* InterfaceName; //0x0004
	CInterface* NextInterface; //0x0008
};//Size=0x000C