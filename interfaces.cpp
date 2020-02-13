#include "interfaces.hpp"

void* CaptureInterface(const char* moduleName, const char* interfaceName) {
	auto create_interface_fn = reinterpret_cast<void* (*)(const char* pName, int* pReturnCode)>(GetProcAddress(GetModuleHandleA(moduleName), "CreateInterface"));
	return create_interface_fn(interfaceName, nullptr);
}

typedef void* (*InstantiateInterface)();

class CInterface
{
public:
	InstantiateInterface Interface; //0x0000
	char* InterfaceName; //0x0004 
	CInterface* NextInterface; //0x0008 

};//Size=0x000C

void* EasyInterface(const char* _Module, const char* _Object) {
	/*
	1E6EC9D0 | 55                       | push ebp                                |
	1E6EC9D1 | 8B EC                    | mov ebp,esp                             |
	1E6EC9D3 | 56                       | push esi                                |
	1E6EC9D4 | 8B 35 B0 41 97 1E        | mov esi,dword ptr ds:[1E9741B0]         |

	1E6ECD20 | 55                       | push ebp                                |
	1E6ECD21 | 8B EC                    | mov ebp,esp                             |
	1E6ECD23 | 5D                       | pop ebp                                 |
	1E6ECD24 | E9 A7 FC FF FF           | jmp client.1E6EC9D0                     |
	*/

	ULONG CreateInterface = (ULONG)GetProcAddress(GetModuleHandle(_Module), "CreateInterface");
	ULONG ShortJump = (ULONG)CreateInterface + 5; //magic number shit explained above
	ULONG Jump = (((ULONG)CreateInterface + 5) + *(ULONG*)ShortJump) + 4;
	CInterface* List = **(CInterface***)(Jump + 6);
	do {
		if (List) {
			if (strstr(List->InterfaceName, _Object) && (strlen(List->InterfaceName) - strlen(_Object)) < 5 /*arbitrary number to see that both names aren't too different*/)
				return List->Interface();
		}

	} while (List = List->NextInterface);
	return 0;
}

uintptr_t GetOffset(RecvTable* table, const char* tableName, const char* netvarName) {
	for (int i = 0; i < table->m_nProps; i++) {
		RecvProp prop = table->m_pProps[i];
		if (!_stricmp(prop.m_pVarName, netvarName)) {
			return prop.m_Offset;
		}
		if (prop.m_pDataTable) {
			uintptr_t offset = GetOffset(prop.m_pDataTable, tableName, netvarName);
			if (offset) {
				return offset + prop.m_Offset;
			}
		}
	}
	return 0;
}

uintptr_t GetNetVarOffset(const char* tableName, const char* netvarName, ClientClass* clientClass) {
	ClientClass* currNode = clientClass;
	for (auto currNode = clientClass; currNode; currNode = currNode->m_pNext) {
		if (!_stricmp(tableName, currNode->m_pRecvTable->m_pNetTableName)) {
			return GetOffset(currNode->m_pRecvTable, tableName, netvarName);
		}
	}
	return 0;
}