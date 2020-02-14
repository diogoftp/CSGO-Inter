#pragma once
#include <Windows.h>
#include <Psapi.h>
#include "SDK/IVEngineClient.hpp"
#include "../offsets.hpp"

class Vec3 {
public:
	float x;
	float y;
	float z;

	Vec3() {
		x = 0;
		y = 0;
		z = 0;
	}

	Vec3(float x, float y, float z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}

	Vec3 operator + (Vec3& vec) {
		Vec3 res;
		res.x = this->x + vec.x;
		res.y = this->y + vec.y;
		res.z = this->z + vec.z;
		return res;
	}

	Vec3 operator - (Vec3& vec) {
		Vec3 res;
		res.x = this->x - vec.x;
		res.y = this->y - vec.y;
		res.z = this->z - vec.z;
		return res;
	}
};

class Entity {
public:
	int32_t clientId() {
		return *(int32_t*)((uintptr_t)this + 0x64);
	}
	bool dormant() {
		return *(bool*)((uintptr_t)this + offsets::m_bDormant);
	}
	int32_t health() {
		return *(uint32_t*)((uintptr_t)this + offsets::m_iHealth);
	}
	int32_t team() {
		return *(uint32_t*)((uintptr_t)this + offsets::m_iTeamNum);
	}
	Vec3 origin() {
		return *(Vec3*)((uintptr_t)this + offsets::m_vecOrigin);
	}

	int8_t lifeState() {
		return *(int8_t*)((uintptr_t)this + offsets::m_lifeState);
	}
	Vec3 viewOffset() {
		return *(Vec3*)((uintptr_t)this + offsets::m_vecViewOffset);
	}
};

class EntityListObj {
public:
	class Entity* entity;	//0x0
	char pad_0004[12];		//0x4
};							//0x10

class EntList {
public:
	EntityListObj entityListObjs[32];	//0x0
};										//0x200



class CBaseHandle;

class IHandleEntity
{
public:
	virtual ~IHandleEntity() {}
	virtual void SetRefEHandle(const CBaseHandle& handle) = 0;
	virtual const CBaseHandle& GetRefEHandle() const = 0;
};

class IClientNetworkable;
class IClientEntity;

class IClientEntityList
{
public:
	virtual IClientNetworkable* GetClientNetworkable(int entnum) = 0;
	virtual void* vtablepad0x1(void) = 0;
	virtual void* vtablepad0x2(void) = 0;
	virtual Entity* GetClientEntity(int entNum) = 0;
	virtual Entity* GetClientEntityFromHandle(CBaseHandle hEnt) = 0;
	virtual int                   NumberOfEntities(bool bIncludeNonNetworkable) = 0;
	virtual int                   GetHighestEntityIndex(void) = 0;
	virtual void                  SetMaxEntities(int maxEnts) = 0;
	virtual int                   GetMaxEntities() = 0;
};

enum ClientFrameStage_t
{
	FRAME_UNDEFINED = -1,
	FRAME_START,
	FRAME_NET_UPDATE_START,
	FRAME_NET_UPDATE_POSTDATAUPDATE_START,
	FRAME_NET_UPDATE_POSTDATAUPDATE_END,
	FRAME_NET_UPDATE_END,
	FRAME_RENDER_START,
	FRAME_RENDER_END
};

// Used by RenderView
enum RenderViewInfo_t
{
	RENDERVIEW_UNSPECIFIED = 0,
	RENDERVIEW_DRAWVIEWMODEL = (1 << 0),
	RENDERVIEW_DRAWHUD = (1 << 1),
	RENDERVIEW_SUPPRESSMONITORRENDERING = (1 << 2),
};

class ClientClass;
class IClientNetworkable;

typedef IClientNetworkable* (*CreateClientClassFn)(int entnum, int serialNum);
typedef IClientNetworkable* (*CreateEventFn)();

enum SendPropType
{
	DPT_Int = 0,
	DPT_Float,
	DPT_Vector,
	DPT_VectorXY,
	DPT_String,
	DPT_Array,
	DPT_DataTable,
	DPT_Int64,
	DPT_NUMSendPropTypes
};

class DVariant
{
public:
	union
	{
		float	   m_Float;
		long	   m_Int;
		char* m_pString;
		void* m_pData;
		float	   m_Vector[3];
		__int64  m_Int64;
	};
	SendPropType  m_Type;
};

class RecvTable;
class RecvProp;

class CRecvProxyData
{
public:
	const RecvProp* m_pRecvProp;        // The property it's receiving.
	DVariant		    m_Value;            // The value given to you to store.
	int				    m_iElement;         // Which array element you're getting.
	int				    m_ObjectID;         // The object being referred to.
};

//-----------------------------------------------------------------------------
// pStruct = the base structure of the datatable this variable is in (like C_BaseEntity)
// pOut    = the variable that this this proxy represents (like C_BaseEntity::m_SomeValue).
//
// Convert the network-standard-type value in m_Value into your own format in pStruct/pOut.
//-----------------------------------------------------------------------------
typedef void(*RecvVarProxyFn)(const CRecvProxyData* pData, void* pStruct, void* pOut);

// ------------------------------------------------------------------------ //
// ArrayLengthRecvProxies are optionally used to Get the length of the 
// incoming array when it changes.
// ------------------------------------------------------------------------ //
typedef void(*ArrayLengthRecvProxyFn)(void* pStruct, int objectID, int currentArrayLength);

// NOTE: DataTable receive proxies work differently than the other proxies.
// pData points at the object + the recv table's offset.
// pOut should be Set to the location of the object to unpack the data table into.
// If the parent object just contains the child object, the default proxy just does *pOut = pData.
// If the parent object points at the child object, you need to dereference the pointer here.
// NOTE: don't ever return null from a DataTable receive proxy function. Bad things will happen.
typedef void(*DataTableRecvVarProxyFn)(const RecvProp* pProp, void** pOut, void* pData, int objectID);

class RecvProp
{
public:
	char* m_pVarName;
	SendPropType            m_RecvType;
	int                     m_Flags;
	int                     m_StringBufferSize;
	int                     m_bInsideArray;
	const void* m_pExtraData;
	RecvProp* m_pArrayProp;
	ArrayLengthRecvProxyFn  m_ArrayLengthProxy;
	RecvVarProxyFn          m_ProxyFn;
	DataTableRecvVarProxyFn m_DataTableProxyFn;
	RecvTable* m_pDataTable;
	int                     m_Offset;
	int                     m_ElementStride;
	int                     m_nElements;
	const char* m_pParentArrayPropName;

	RecvVarProxyFn			GetProxyFn() const;
	void					SetProxyFn(RecvVarProxyFn fn);
	DataTableRecvVarProxyFn	GetDataTableProxyFn() const;
	void					SetDataTableProxyFn(DataTableRecvVarProxyFn fn);

};

class RecvTable
{
public:
	RecvProp* m_pProps;
	int                     m_nProps;
	void* m_pDecoder;
	char* m_pNetTableName;
	bool                    m_bInitialized;
	bool                    m_bInMainList;
};

inline RecvVarProxyFn RecvProp::GetProxyFn() const
{
	return m_ProxyFn;
}

inline void RecvProp::SetProxyFn(RecvVarProxyFn fn)
{
	m_ProxyFn = fn;
}

inline DataTableRecvVarProxyFn RecvProp::GetDataTableProxyFn() const
{
	return m_DataTableProxyFn;
}

inline void RecvProp::SetDataTableProxyFn(DataTableRecvVarProxyFn fn)
{
	m_DataTableProxyFn = fn;
}

class CGlobalVarsBase
{
public:
	float     realtime;                     // 0x0000
	int       framecount;                   // 0x0004
	float     absoluteframetime;            // 0x0008
	float     absoluteframestarttimestddev; // 0x000C
	float     curtime;                      // 0x0010
	float     frametime;                    // 0x0014
	int       maxClients;                   // 0x0018
	int       tickcount;                    // 0x001C
	float     interval_per_tick;            // 0x0020
	float     interpolation_amount;         // 0x0024
	int       simTicksThisFrame;            // 0x0028
	int       network_protocol;             // 0x002C
	void* pSaveData;                    // 0x0030
	bool      m_bClient;                    // 0x0031
	bool      m_bRemoteClient;              // 0x0032

private:
	// 100 (i.e., tickcount is rounded down to this base and then the "delta" from this base is networked
	int       nTimestampNetworkingBase;
	// 32 (entindex() % nTimestampRandomizeWindow ) is subtracted from gpGlobals->tickcount to Set the networking basis, prevents
	//  all of the entities from forcing a new PackedEntity on the same tick (i.e., prevents them from getting lockstepped on this)
	int       nTimestampRandomizeWindow;
};

class ClientClass
{
public:
	CreateClientClassFn      m_pCreateFn;
	CreateEventFn            m_pCreateEventFn;
	char* m_pNetworkName;
	RecvTable* m_pRecvTable;
	ClientClass* m_pNext;
	int                  m_ClassID;
};

class IBaseClientDLL
{
public:
	//virtual int              Connect(CreateInterfaceFn appSystemFactory, CGlobalVarsBase* pGlobals) = 0;
	virtual int vtablepad0x6(void) = 0;
	virtual int              Disconnect(void) = 0;
	//virtual int              Init(CreateInterfaceFn appSystemFactory, CGlobalVarsBase* pGlobals) = 0;
	virtual int vtablepad0x7(void) = 0;
	virtual void             PostInit() = 0;
	virtual void             Shutdown(void) = 0;
	virtual void             LevelInitPreEntity(char const* pMapName) = 0;
	virtual void             LevelInitPostEntity() = 0;
	virtual void             LevelShutdown(void) = 0;
	virtual ClientClass* GetAllClasses(void) = 0;

	/*bool DispatchUserMessage(int messageType, int arg, int arg1, void* data) {
		using DispatchUserMessage_t = bool* (__thiscall*)(void*, int, int, int, void*);
		return CallVFunction<DispatchUserMessage_t>(this, 38)(this, messageType, arg, arg1, data);
	}*/
};

class C_BasePlayer {
public:
	virtual bool				IsPlayerDead() = 0;
};