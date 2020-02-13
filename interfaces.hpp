#pragma once
#include "SDK.hpp"

void* CaptureInterface(const char* moduleName, const char* interfaceName);
void* EasyInterface(const char* _Module, const char* _Object);
uintptr_t GetOffset(RecvTable* table, const char* tableName, const char* netvarName);
uintptr_t GetNetVarOffset(const char* tableName, const char* netvarName, ClientClass* clientClass);