#include "stdafx.h"
#include "GameInit.h"
#include "ICoreGameInit.h"
#include "atArray.h"
#include "sysAllocator.h"
#include "scrEngine.h"

fwEvent<> OnGameFrame;
fwEvent<> OnGameFinalizeLoad;
fwEvent<> OnGameRequestLoad;
fwEvent<> OnMsgConfirm;
fwEvent<const char*> OnKillNetwork;

static bool g_showWarningMessage;
FiveGameInit g_gameInit;
static int g_stackIdx;
static void** g_unsafePointerLoc;

static char* g_boundsStreamingModule;
template<typename T>
class fwPool
{
private:
	char* m_data;
	int8_t* m_flags;
	uint32_t m_count;
	uint32_t m_entrySize;

public:
	T* GetAt(int index) const
	{
		return reinterpret_cast<T*>(m_data + (index * m_entrySize));
	}

	void Clear()
	{
		for (int i = 0; i < m_count; i++)
		{
			if (m_flags[i] >= 0)
			{
				delete GetAt(i);
			}
		}
	}
};

class CInteriorProxy
{
public:
	virtual ~CInteriorProxy() = 0;
};

static fwPool<CInteriorProxy>** g_interiorProxyPool;

static std::unordered_map<int, std::vector<uintptr_t>> g_stacks;

static atArray<CInteriorProxy*>* g_vehicleReflEntityArray;

static void*(*oldInitCall)(void*, uint32_t, uint32_t);

static void* PostScriptInit(void* arg, uint32_t a2, uint32_t a3)
{
	void* retval = oldInitCall(arg, a2, a3);

	rage::scrEngine::OnScriptInit(); //This was disabled

	return retval;
}

template<typename T>
class allocWrap : public rage::sysUseAllocator
{
private:
	T m_value;

public:
	allocWrap()
		: m_value()
	{

	}

	allocWrap(const T& value)
		: m_value(value)
	{

	}

	inline T& Get() const
	{
		return m_value;
	}

	inline void Set(const T& value)
	{
		m_value = value;
	}

	operator T&() const
	{
		return m_value;
	}
};
static atArray<allocWrap<uint32_t>>* g_cacheArray;

#ifdef HOOKS_ENABLED
static hook::cdecl_stub<void(int unk, uint32_t* titleHash, uint32_t* messageHash, uint32_t* subMessageHash, int flags, bool, int8_t, void*, void*, bool, bool)> setWarningMessage([]()
{
	return hook::get_call<void*>(hook::get_call(hook::pattern("57 41 56 41 57 48 83 EC 50 4C 63 F2").count(1).get(0).get<char>(0xAC)) + 0x6D);
});

static hook::cdecl_stub<int(bool, int)> getWarningResult([]()
{
	return hook::get_call(hook::pattern("33 D2 33 C9 E8 ? ? ? ? 48 83 F8 04 0F 84").count(1).get(0).get<void>(4));
});
static void(*g_deinitLevel)();
static hook::cdecl_stub<void(int)> initModelInfo([]()
{
	return hook::pattern("48 83 EC 20 83 F9 01 0F 85 A5 06 00 00").count(1).get(0).get<void>(-0x15);
});

static hook::cdecl_stub<void(int)> shutdownModelInfo([]()
{
	return hook::pattern("48 83 EC 20 83 F9 01 0F 85 77 01 00 00").count(1).get(0).get<void>(-0x10);
});

static hook::cdecl_stub<void(void*)> clearSceneLinkedList([]()
{
	return hook::pattern("48 8B F9 EB 38 48 8B 17 48 8B CB 48 8B").count(1).get(0).get<void>(-0xD);
});

static hook::cdecl_stub<void(void*)> extraContentMgr__doScanInt([]()
{
	return hook::get_call(hook::pattern("48 83 EC 20 80 A1 7A 01 00 00 FE 41 8A D8").count(1).get(0).get<void>(17));
});

// a3: skip mount?
static hook::cdecl_stub<void(void*, bool, bool)> extraContentMgr__doScanPost([]()
{
	//return hook::pattern("48 83 EC 20 80 A1 7A 01 00 00 FE 41 8A D8").count(1).get(0).get<void>(-0x6);
	return hook::get_call(hook::pattern("48 83 EC 20 80 A1 7A 01 00 00 FE 41 8A D8").count(1).get(0).get<void>(30));
});

static hook::cdecl_stub<void()> lookAlive([]()
{
	return hook::pattern("48 8D 6C 24 A0 48 81 EC 60 01 00 00 E8").count(1).get(0).get<void>(-0xC);
});

static hook::cdecl_stub<void()> loadHudColor([]()
{
	return hook::pattern("45 33 F6 41 8D 56 27 44 89").count(1).get(0).get<void>(-0x23);
});
#endif

static bool(*g_callBeforeLoad)();
static bool g_launchedGame = false;
static CRITICAL_SECTION g_allocCS;
static int* g_initState;
static bool g_isDigitalDistrib = false;
bool g_isInInitLoop = false;
#if HOOKS_ENABLED
static void WaitForInitLoopWrap();
static void WaitForInitLoop();
#endif
static void(*g_lookAlive)();
static bool loadedHudColor = false;
bool g_shouldSetState;
static bool* disable_gta_ui = nullptr;
static void** g_extraContentMgr;
static void* g_sceneLinkedList;
static bool g_didLevelFree;
static bool g_inLevelFree;

static void(*g_runInitFunctions)(void*, int);

#ifdef HOOKS_ENABLED
static bool LoadHudColorWrap()
{
	__try
	{
		loadHudColor();
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return false;
	}

	return true;
}
#endif

static void RunInitFunctionsWrap(void* skel, int type)
{
	if (g_callBeforeLoad)
	{
		while (!g_callBeforeLoad())
		{
			g_lookAlive();

			OnGameFrame();
		}
	}

	g_runInitFunctions(skel, type);
}

template<int Value>
int ReturnInt()
{
	return Value;
}

static bool ThisIsActuallyLaunchery() {
	return true;
}

static inline int MapInitState(int initState)
{
	if (initState >= 7)
	{
		if (g_isDigitalDistrib)
		{
			initState += 1;
		}
	}

	return initState;
}

template<typename T>
struct LinkNode : public rage::sysUseAllocator
{
	T* value;
	LinkNode* next;
};

static void ClearInteriorProxyList(void* list)
{
	auto linkedList = *(LinkNode<CInteriorProxy>**)list;

	if (linkedList)
	{
		do
		{
			delete linkedList->value;

			auto next = linkedList->next;
			delete linkedList;

			linkedList = next;
		} while (linkedList);
	}

	*(void**)list = nullptr;
}

int(*LoadGameNow)(char);
static bool gameLoading = false;

#if HOOKS_ENABLED
static void WaitForInitLoopWrap()
{
	// certain executables may recheck activation after connection, and want to perform this state change after 12 - ignore those cases
	// draw_menu_line("WaitForInitLoopWrap called", 15.0f, 50.0f, 570.0f, 6.0f, 5.0f, false, false, false); //drawrect set to true
	//*g_initState = MapInitState(6);

	//WaitForInitLoop();
	if (!gameLoading) {
		//*g_initState = 7;

		LoadGameNow(0);
		gameLoading = true;
	}
}
static void DeinitLevel()
{
	static bool initedLateHook = false;

	if (!initedLateHook)
	{
		// late hook to prevent scenario manager from reinitializing on map load - it messes things up, a lot, and I doubt custom maps use scenarios anyway. :)
		hook::put<uint16_t>(hook::pattern("83 F9 04 0F 85 F9 00 00 00").count(1).get(0).get<void>(3), 0xE990); // shutdown
		hook::put<uint16_t>(hook::pattern("83 F9 02 0F 85 C6 01 00 00").count(1).get(0).get<void>(3), 0xE990); // init

		// don't load mounted ped (personality) metadata anymore (temp dbg-y?)
		hook::nop(hook::pattern("48 8B DA 83 E9 14 74 5B").count(1).get(0).get<void>(0x72 - 12), 5);

		initedLateHook = true;
	}

	// stuff
	g_inLevelFree = true;

	g_deinitLevel();

	shutdownModelInfo(1);
	initModelInfo(1);
	initStreamingInterface();

	// extra content manager shutdown session removes these, and we want these before init session, so we scan them right here, right now.
	extraContentMgr__doScanInt(*g_extraContentMgr);
	extraContentMgr__doScanPost(*g_extraContentMgr, false, false);

	//shutdownPhysics(1);
	//initPhysics(1);

	// unknown value in the bounds streaming module, doesn't get cleared on 'after map loaded' shutdown
	*(uint32_t*)(g_boundsStreamingModule + 5664) = 0;

	// bounds streaming module, 'has preloading bounds completed' value
	*(uint8_t*)(g_boundsStreamingModule + 255) = 0;

	// clear the 'loaded cache hashes' list
	*g_cacheArray = atArray<allocWrap<uint32_t>>(16);

	// free one CScene list of all the bad influences from the last session
	ClearInteriorProxyList(g_sceneLinkedList);

	// also clear the interior proxy pool out, as it might contain garbage references to static bounds, still
	(*g_interiorProxyPool)->Clear();

	// and some global vehicle audio entity also houses... interior proxies.
	*g_vehicleReflEntityArray = atArray<CInteriorProxy*>();

	g_inLevelFree = false;

	if (!g_didLevelFree)
	{
		for (auto& stack : g_stacks)
		{
			FILE* f = fopen(va("D:\\dev\\stacks\\%p.txt", ((stack.first / 256) * 256)), "a");

			if (f)
			{
				fprintf(f, "--- %p ---\n", stack.first);

				for (auto& entry : stack.second)
				{
					fprintf(f, "%p\n", entry);
				}

				fprintf(f, "--- --- ---\n");

				fclose(f);
			}
		}

		g_stackIdx = 0;
		g_stacks.clear();

		g_didLevelFree = true;
	}
}
#endif

static void(*g_origError)(uint32_t, void*);

static void ErrorDo(uint32_t error)
{
	char errorstr[256];
	sprintf(errorstr, "error function called from %p for code 0x%08x\n", _ReturnAddress(), error);
	MessageBoxA(0, errorstr, "Error", 0);

	g_origError(error, 0);
}

static const char* typeMap[] = {
	nullptr,
	"system",
	"beforeMapLoaded",
	nullptr,
	"afterMapLoaded",
	nullptr,
	nullptr,
	nullptr,
	"session"
};

struct InitFunctionStub : public jitasm::Frontend
{
	static uintptr_t LogStub(uintptr_t stub, int type)
	{
		trace("Running shutdown %s function: %p\n", typeMap[type], stub);
		return stub;
	}

	virtual void InternalMain() override
	{
		push(r14);

		mov(rcx, qword_ptr[rax + rdx * 8 + 8]);
		mov(edx, r14d);

		// scratch space!
		sub(rsp, 0x20);

		mov(rax, (uintptr_t)LogStub);
		call(rax);

		mov(ecx, r14d);
		call(rax);

		add(rsp, 0x20);

		pop(r14);

		ret();
	}
};

struct InitFunctionStub2 : public jitasm::Frontend
{
	static uintptr_t LogStub(uintptr_t stub, int type)
	{
		trace("Running init %s function: %p\n", typeMap[type], stub);
		return stub;
	}

	virtual void InternalMain() override
	{
		push(r14);

		mov(rcx, qword_ptr[rax + rdx * 8]);

		mov(edx, r14d);

		// scratch space!
		sub(rsp, 0x20);

		mov(rax, (uintptr_t)LogStub);
		call(rax);

		mov(ecx, r14d);
		call(rax);

		add(rsp, 0x20);

		pop(r14);

		ret();
	}
};

static void(*g_origFreeMapTypes)(void* store, void* entry, void* a3, bool a4);

void DoFreeMapTypes(void* store, void* entry, void* a3, bool a4)
{
	if ((uintptr_t)entry != 16)
	{
		g_origFreeMapTypes(store, entry, a3, a4);
	}
}

void ReloadGame()
{
	OnGameRequestLoad();

	*g_initState = MapInitState(14);
}

#include "scrThread.h"
#include "GameInit.h"

#include <winternl.h>

class UserLibrary
{
private:
	std::vector<uint8_t> m_libraryBuffer;

public:
	UserLibrary(const wchar_t* fileName);

	const uint8_t* GetExportCode(const char* name) const;

	const uint8_t* GetOffsetPointer(uint32_t offset) const;
};

static void* origCloseHandle;

typedef struct _OBJECT_HANDLE_ATTRIBUTE_INFORMATION
{
	BOOLEAN Inherit;
	BOOLEAN ProtectFromClose;
} OBJECT_HANDLE_ATTRIBUTE_INFORMATION, *POBJECT_HANDLE_ATTRIBUTE_INFORMATION;

#pragma comment(lib, "ntdll.lib")

struct NtCloseHook : public jitasm::Frontend
{
	NtCloseHook()
	{

	}

	static NTSTATUS ValidateHandle(HANDLE handle)
	{
		char info[16];

		if (NtQueryObject(handle, (OBJECT_INFORMATION_CLASS)4, &info, sizeof(OBJECT_HANDLE_ATTRIBUTE_INFORMATION), nullptr) >= 0)
		{
			return 0;
		}
		else
		{
			return STATUS_INVALID_HANDLE;
		}
	}

	void InternalMain()
	{
		push(rcx);

		mov(rax, (uint64_t)&ValidateHandle);
		call(rax);

		pop(rcx);

		cmp(eax, STATUS_INVALID_HANDLE);
		je("doReturn");

		mov(rax, (uint64_t)origCloseHandle);
		push(rax); // to return here, as there seems to be no jump-to-rax in jitasm

		L("doReturn");
		ret();
	}
};

class NtdllHooks
{
private:
	UserLibrary m_ntdll;

private:
	void HookHandleClose();

	void HookQueryInformationProcess();

public:
	NtdllHooks(const wchar_t* ntdllPath);

	void Install();
};

NtdllHooks::NtdllHooks(const wchar_t* ntdllPath)
	: m_ntdll(ntdllPath)
{
}

void NtdllHooks::Install()
{
	HookHandleClose();
	HookQueryInformationProcess();
}

void NtdllHooks::HookHandleClose()
{
	// hook NtClose (STATUS_INVALID_HANDLE debugger detection)
	uint8_t* code = (uint8_t*)GetProcAddress(GetModuleHandle(L"ntdll.dll"), "NtClose");

	origCloseHandle = VirtualAlloc(nullptr, 1024, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	memcpy(origCloseHandle, m_ntdll.GetExportCode("NtClose"), 1024);

	NtCloseHook* hook = new NtCloseHook;
	hook->Assemble();

	DWORD oldProtect;
	VirtualProtect(code, 15, PAGE_EXECUTE_READWRITE, &oldProtect);

	*(uint8_t*)code = 0x48;
	*(uint8_t*)(code + 1) = 0xb8;

	*(uint64_t*)(code + 2) = (uint64_t)hook->GetCode();

	*(uint16_t*)(code + 10) = 0xE0FF;
}

UserLibrary::UserLibrary(const wchar_t* fileName)
{
	FILE* f = _wfopen(fileName, L"rb");

	if (f)
	{
		fseek(f, 0, SEEK_END);
		m_libraryBuffer.resize(ftell(f));

		fseek(f, 0, SEEK_SET);
		fread(&m_libraryBuffer[0], 1, m_libraryBuffer.size(), f);

		fclose(f);
	}
}

const uint8_t* UserLibrary::GetOffsetPointer(uint32_t offset) const
{
	// get the DOS header
	const IMAGE_DOS_HEADER* header = (const IMAGE_DOS_HEADER*)&m_libraryBuffer[0];

	// get the NT header
	const IMAGE_NT_HEADERS* ntHeader = (const IMAGE_NT_HEADERS*)&m_libraryBuffer[header->e_lfanew];

	// loop through each sections to find where our offset is
	const IMAGE_SECTION_HEADER* sections = IMAGE_FIRST_SECTION(ntHeader);

	for (int i = 0; i < ntHeader->FileHeader.NumberOfSections; i++)
	{
		uint32_t curRaw = sections[i].PointerToRawData;
		uint32_t curVirt = sections[i].VirtualAddress;

		if (offset >= curVirt && offset < curVirt + sections[i].SizeOfRawData)
		{
			offset -= curVirt;
			offset += curRaw;

			return &m_libraryBuffer[offset];
		}
	}

	return nullptr;
}

const uint8_t* UserLibrary::GetExportCode(const char* getName) const
{
	// get the DOS header
	IMAGE_DOS_HEADER* header = (IMAGE_DOS_HEADER*)&m_libraryBuffer[0];

	if (header->e_magic != IMAGE_DOS_SIGNATURE)
	{
		return nullptr;
	}

	// get the NT header
	const IMAGE_NT_HEADERS* ntHeader = (const IMAGE_NT_HEADERS*)&m_libraryBuffer[header->e_lfanew];

	// find the export directory
	auto exportDirectoryData = ntHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];

	// get the export directory
	const IMAGE_EXPORT_DIRECTORY* exportDirectory = (const IMAGE_EXPORT_DIRECTORY*)GetOffsetPointer(exportDirectoryData.VirtualAddress);

	const uint32_t* names = (const uint32_t*)GetOffsetPointer(exportDirectory->AddressOfNames);
	const uint16_t* ordinals = (const uint16_t*)GetOffsetPointer(exportDirectory->AddressOfNameOrdinals);
	const uint32_t* functions = (const uint32_t*)GetOffsetPointer(exportDirectory->AddressOfFunctions);

	for (int i = 0; i < exportDirectory->NumberOfNames; i++)
	{
		const char* name = (const char*)GetOffsetPointer(names[i]);

		if (_stricmp(name, getName) == 0)
		{
			return GetOffsetPointer(functions[ordinals[i]]);
		}
	}

	return nullptr;
}
#include <ntstatus.h>

static bool g_isDebuggerPresent;

bool CoreIsDebuggerPresent()
{
	return g_isDebuggerPresent;
}

void CoreSetDebuggerPresent()
{
	g_isDebuggerPresent = IsDebuggerPresent();
}

static void* origQIP;
static DWORD explorerPid;
typedef NTSTATUS(*NtQueryInformationProcessType)(IN HANDLE ProcessHandle, IN PROCESSINFOCLASS ProcessInformationClass, OUT PVOID ProcessInformation, IN ULONG ProcessInformationLength, OUT PULONG ReturnLength OPTIONAL);
static NTSTATUS NtQueryInformationProcessHook(IN HANDLE ProcessHandle, IN PROCESSINFOCLASS ProcessInformationClass, OUT PVOID ProcessInformation, IN ULONG ProcessInformationLength, OUT PULONG ReturnLength OPTIONAL)
{
	NTSTATUS status = ((NtQueryInformationProcessType)origQIP)(ProcessHandle, ProcessInformationClass, ProcessInformation, ProcessInformationLength, ReturnLength);

	if (NT_SUCCESS(status))
	{
		if (ProcessInformationClass == ProcessBasicInformation)
		{
			((PPROCESS_BASIC_INFORMATION)ProcessInformation)->Reserved3 = (PVOID)explorerPid;
		}
		else if (ProcessInformationClass == 30) // ProcessDebugObjectHandle
		{
			*(HANDLE*)ProcessInformation = 0;

			return STATUS_PORT_NOT_SET;
		}
		else if (ProcessInformationClass == 7) // ProcessDebugPort
		{
			*(HANDLE*)ProcessInformation = 0;
		}
		else if (ProcessInformationClass == 31)
		{
			*(ULONG*)ProcessInformation = 1;
		}
	}

	return status;
}

void NtdllHooks::HookQueryInformationProcess()
{
	uint8_t* code = (uint8_t*)GetProcAddress(GetModuleHandle(L"ntdll.dll"), "NtQueryInformationProcess");

	HWND shellWindow = GetShellWindow();
	GetWindowThreadProcessId(shellWindow, &explorerPid);

	origQIP = VirtualAlloc(nullptr, 1024, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	memcpy(origQIP, m_ntdll.GetExportCode("NtQueryInformationProcess"), 1024);

	/*NtQueryInformationProcessHook* hook = new NtQueryInformationProcessHook;
	hook->Assemble();*/

	DWORD oldProtect;
	VirtualProtect(code, 15, PAGE_EXECUTE_READWRITE, &oldProtect);

	*(uint8_t*)code = 0x48;
	*(uint8_t*)(code + 1) = 0xb8;

	*(uint64_t*)(code + 2) = (uint64_t)NtQueryInformationProcessHook;

	*(uint16_t*)(code + 10) = 0xE0FF;
}

static LONG NTAPI HandleVariant(PEXCEPTION_POINTERS exceptionInfo)
{
	return (exceptionInfo->ExceptionRecord->ExceptionCode == STATUS_INVALID_HANDLE) ? EXCEPTION_CONTINUE_EXECUTION : EXCEPTION_CONTINUE_SEARCH;
}

void ExceptionDebuggerThing() {

	CoreSetDebuggerPresent();
	// set BeingDebugged
	PPEB peb = (PPEB)__readgsqword(0x60);
	peb->BeingDebugged = false;

	// set GlobalFlags
	*(DWORD*)((char*)peb + 0xBC) &= ~0x70;

	{
		// user library stuff ('safe' ntdll hooking callbacks)
		wchar_t ntdllPath[MAX_PATH];
		GetModuleFileName(GetModuleHandle(L"ntdll.dll"), ntdllPath, _countof(ntdllPath));

		NtdllHooks hooks(ntdllPath);
		hooks.Install();
	}

	if (CoreIsDebuggerPresent())
	{
		// NOP OutputDebugStringA; the debugger doesn't like multiple async exceptions
		uint8_t* func = (uint8_t*)OutputDebugStringA;

		DWORD oldProtect;
		VirtualProtect(func, 1, PAGE_EXECUTE_READWRITE, &oldProtect);

		*func = 0xC3;

		VirtualProtect(func, 1, oldProtect, &oldProtect);
	}
	AddVectoredExceptionHandler(0, HandleVariant);
	//g_launcher = launcher;
}

static bool(*g_origLookAlive)();

#include <mutex>
#include <mmsystem.h>

static uint32_t g_lastGameFrame;
static std::mutex g_gameFrameMutex;
static DWORD g_mainThreadId;
static bool g_executedOnMainThread;

static void DoGameFrame()
{
	if (g_gameFrameMutex.try_lock())
	{
		OnGameFrame();

		g_gameFrameMutex.unlock();
	}

	if (GetCurrentThreadId() == g_mainThreadId)
	{
		g_executedOnMainThread = true;
	}

	g_lastGameFrame = timeGetTime();
}

static bool OnLookAlive()
{
	DoGameFrame();

	return g_origLookAlive();
}

static void(*g_origFrameFunc)();

void DoLoadsFrame()
{
	g_origFrameFunc();

	int timeout = (g_executedOnMainThread) ? 500 : 50;

	if ((timeGetTime() - g_lastGameFrame) > timeout)
	{
		g_executedOnMainThread = false;

		DoGameFrame();
	}
}

static void WaitThing(int i)
{
	trace("waiting from %p", _ReturnAddress());

	Sleep(i);
}

static int(*g_appState)(void* fsm, int state, void* unk, int type);
fwEvent<> OnFirstLoadCompleted;

int DoAppState(void* fsm, int state, void* unk, int type)
{
	static bool firstLoadCompleted = false;

	if (!firstLoadCompleted && state == 2 && type == 1)
	{
		OnFirstLoadCompleted();

		firstLoadCompleted = true;
	}

	return g_appState(fsm, state, unk, type);
}
#if HOOKS_ENABLED
static void WaitForInitLoop()
{
	// run our loop
	g_isInInitLoop = true;
	//draw_menu_line("WaitForInitLoop called", 15.0f, 50.0f, 570.0f, 6.0f, 5.0f, false, false, false); //drawrect set to true
	while (*g_initState <= MapInitState(6))
	{
		lookAlive();

		if (*g_initState <= MapInitState(6))
		{
			Sleep(15);
		}
	}

	//*g_initState = 7;
}

namespace rage
{
	class fwArchetype
	{
	public:
		virtual ~fwArchetype() = 0;

		virtual void m1() = 0;

		virtual void m2() = 0;

		virtual void m3() = 0;

		virtual void m4() = 0;

		virtual void m5() = 0;

		virtual void m6() = 0;

		virtual void m7() = 0;

		virtual void m8() = 0;

		virtual void _CleanUp() = 0;
	};
}

static void DestructMI(rage::fwArchetype* archetype)
{
	MessageBoxA(0, "DestructMi called", "Dest...", 0);
	archetype->_CleanUp();
	archetype->~fwArchetype(); // note: we can't delete this; that'll cause a double-free later on
}

static ULONG g_pendSendVar;

struct AutoIdDescriptor
{
	uintptr_t vtable;
	int unk;
	int id;
	int id2;
	const char* name;
	AutoIdDescriptor* parent;
	AutoIdDescriptor* next;
};

static AutoIdDescriptor** g_netMessage;

static void __stdcall LogDescriptorDo(int netType)
{
	if (g_netMessage)
	{
		for (AutoIdDescriptor* descriptor = (*g_netMessage)->next; descriptor->next; descriptor = descriptor->next)
		{
			if (descriptor->id == netType)
			{
				g_pendSendVar++;

				if (g_pendSendVar > 40)
				{
					// help! too many pending packets!
					//__debugbreak();
				}

				return;
			}
		}
	}
}

struct ItemHook : public jitasm::Frontend
{
public:
	virtual void InternalMain() override
	{
		push(rcx);
		push(rdx);

		sub(rsp, 32);

		mov(rax, (uintptr_t)LogDescriptorDo);
		call(rax);

		add(rsp, 32);

		pop(rdx);
		pop(rcx);

		mov(qword_ptr[rsp + 8 + 8], rbx);

		ret();
	}
};

static ItemHook itemHook;

//This simply won't work
static HookFunction hookFunction2([] ()
{
	InitializeCriticalSectionAndSpinCount(&g_allocCS, 1000);

	// NOP out any code that sets the 'entering state 2' (2, 0) FSM internal state to '7' (which is 'load game'), UNLESS it's digital distribution with standalone auth...
	char* p = hook::pattern("BA 07 00 00 00 8D 41 FC 83 F8 01").count(1).get(0).get<char>(14);
	////4C 4F 41 44 47 41 4D 45

	char* varPtr = p + 2;
	g_initState = (int*)(varPtr + *(int32_t*)varPtr + 4);

	// check the pointer to see if it's digital distribution
	g_isDigitalDistrib = (p[-26] == 3);

	// this is also a comparison point to find digital distribution type... this function will also set '3' if it's digital distrib with standalone auth
	// and if this *is* digital distribution, we want to find a completely different place that sets the value to 8 (i.e. BA 08 ...)
	if (g_isDigitalDistrib)
	{
		p = hook::pattern("BA 08 00 00 00 8D 41 FC 83 F8 01").count(1).get(0).get<char>(14);
	}

	// nop the right pointer
	//hook::nop(p, 6);

	// and call our little internal loop function from there
	//hook::call(p, WaitForInitLoop);
	// also add a silly loop to state 6 ('wait for landing page'?) (Implement cef?)
	p = hook::pattern("C7 05 ? ? ? ? 06 00 00 00 EB 3F").count(1).get(0).get<char>(0);

	hook::nop(p, 10);
	hook::call(p, WaitForInitLoopWrap);

	//Send load game
	char* func = hook::pattern("33 C9 E8 ? ? ? ? 8B 0D ? ? ? ? 48 8B 5C 24 ? 8D 41 FC 83 F8 01 0F 47 CF 89 0D ? ? ? ?").count(1).get(0).get<char>(2);
	hook::set_call(&LoadGameNow, func);
	
	//Disable auto game saving screen prompt, duh annoying R* please...
	char* location = hook::pattern("8B 49 10 85 C9 74 1B").count(1).get(0).get<char>(5);
	hook::nop(location, 2);
	char* location2 = hook::pattern("FF C9 0F 85 ? ? ? ? B1 01 E8 ? ? ? ? 85 C0 75 6C").count(1).get(0).get<char>(2);
	void* jmpAddress = hook::pattern("F7 43 ? ? ? ? ? 74 04 32 C0 EB 34 48 8D 0D ? ? ? ? E8 ? ? ? ?").count(1).get(0).get<void>(0);
	hook::jump(location2, jmpAddress);

	//Disable traffic and probably parked cars too, doesn't do anything? probs wrong func
	char* locationCar = hook::pattern("74 2B 48 8D 0D ? ? ? ? 48 89 08 48 89 50 10").count(1).get(0).get<char>(0);
	void* jmpAddressCar = hook::pattern("48 8B C2 48 83 C4 28 C3 48 83 EC 28 B9 ? ? ? ? E8 ? ? ? ? 33 C9 48 85 C0 74 1F").count(1).get(0).get<void>(0);
	hook::jump(locationCar, jmpAddressCar);

#if 0 
	//For this to compile, rage scripting needs to be enabled in the preprocessor thing
	//I guess killing scripts is for a future project
	//OnInitScripts something changed during the updates, this doesn't work anymore
	auto match = hook::pattern("BA 2F 7B 2E 30 41 B8 0A").count(1).get(0);
	oldInitCall = (decltype(oldInitCall))hook::get_call(match.get<void>(/*23*/11));
	hook::call(match.get<void>(/*23*/11), PostScriptInit);
#endif
});
#if 0 //Couldn't make descriptors work :(
static HookFunction hookFuncNet([]()
{
	//Networking and stuff
	//We need the descriptors..
	char* location;
	itemHook.Assemble();
	hook::call(hook::pattern("48 83 EC 20 48 8B DA BE 01 00 00 00 32 D2 8B F9").count(1).get(0).get<void>(-0xB), itemHook.GetCode());

	// find autoid descriptors
	auto matches = hook::pattern("48 89 03 8B 05 ? ? ? ? A8 01 75 21 83 C8 01 48 8D 0D");

	for (int i = 0; i < matches.size(); i++)
	{
		location = matches.get(i).get<char>(-4);
		void** vt = (void**)(location + *(int32_t*)location + 4);

		// RTTI locator
		vt--;

		struct RttiLocator
		{
			int signature;
			int offset;
			int cdOffset;
			uint32_t pTypeDescriptor;
		};

		RttiLocator* locator = (RttiLocator*)*vt;
		if (locator->signature == 1)
		{
			char* namePtr = (char*)(0x140000000 + locator->pTypeDescriptor + 16);

			if (strcmp(namePtr, ".?AV?$AutoIdDescriptor_T@VnetMessage@rage@@@rage@@") == 0)
			{
				location = matches.get(i).get<char>(19);

				g_netMessage = (AutoIdDescriptor**)(location + *(int32_t*)location + 4);

				break;
			}
		}
	}
});
#endif
static HookFunction hookFunction3([]()
{
	g_mainThreadId = GetCurrentThreadId();

	/*
	void* lookAliveFrameCall = hook::pattern("48 81 EC 60 01 00 00 E8 ? ? ? ? 33 F6 48 8D").count(1).get(0).get<void>(7);

	hook::set_call(&g_origLookAlive, lookAliveFrameCall);
	hook::call(lookAliveFrameCall, OnLookAlive);

	auto waits = hook::pattern("EB 0F B9 21 00 00 00 E8").count(3);

	for (int i = 0; i < waits.size(); i++)
	{
		hook::call(waits.get(i).get<void>(7), WaitThing);
	}
	*/
	char* location = hook::pattern("66 89 41 16 83 C8 FF 4C 89 49 0C 44 89").count(1).get(0).get<char>(0xC - 0x2E);

	location = (char*)(location + *(int32_t*)location + 4);

	location += 32;

	void** vt = (void**)location;

	g_appState = (decltype(g_appState))vt[0];
	vt[0] = DoAppState;

	// temp nop of script handler mgr shutdown because of curiosity (was disabled by me before)
	//void* fpt = hook::pattern("48 8B CB FF 50 48 84 C0 74 23 48").count(1).get(0).get<void>(-0x24);
	//hook::return_function(fpt);

	// loading screen render thread function, to 'safely' handle game frames while loading (as a kind of watchdog)
	void* func = hook::pattern("83 FB 0A 0F 85 80 00 00 00 8B").count(1).get(0).get<void>(-17);

	hook::set_call(&g_origFrameFunc, func);
	hook::call(func, DoLoadsFrame);
	//__debugbreak();
});

//void AddCustomText(const char* key, const char* value);

void FiveGameInit::KillNetwork(const wchar_t* errorString)
{
	if (errorString == (wchar_t*)1)
	{
		OnKillNetwork(nullptr);
	}
	else
	{
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> converter;
		std::string smallReason = converter.to_bytes(errorString);

		if (!g_showWarningMessage)
		{
			//AddCustomText("CFX_NETERR", smallReason.c_str());
			//AddCustomText("CFX_NETERR_TITLE", "\xD0\x9E\xD0\xA8\xD0\x98\xD0\x91\xD0\x9A\xD0\x90"); // Oshibka!

			g_showWarningMessage = false;

			OnKillNetwork(smallReason.c_str());
		}
	}
}

bool FiveGameInit::GetGameLoaded()
{
	return m_gameLoaded;
}

void FiveGameInit::SetGameLoaded()
{
	m_gameLoaded = true;
}

void FiveGameInit::SetPreventSavePointer(bool* preventSaveValue)
{

}

bool FiveGameInit::TryDisconnect()
{
	return true;
}

void FiveGameInit::LoadGameFirstLaunch(bool(*callBeforeLoad)())
{
	g_callBeforeLoad = callBeforeLoad;

	g_launchedGame = true;

	OnGameFrame.Connect([=]()
	{
		if (g_shouldSetState)
		{
			if (*g_initState == MapInitState(6))
			{
				*g_initState = MapInitState(7);

				g_shouldSetState = false;
			}
		}

		static bool isLoading = false;

		if (isLoading)
		{
			if (*g_initState == 0)
			{
				OnGameFinalizeLoad();
				isLoading = false;
			}
		}
		else
		{
			if (*g_initState != 0)
			{
				isLoading = true;
			}
		}
	});

	OnGameRequestLoad();

	// stuff
	if (*g_initState == MapInitState(6))
	{
		*g_initState = MapInitState(7);
	}
	else
	{
		if (*g_initState == MapInitState(20))
		{
			*g_initState = MapInitState(11);
		}

		g_shouldSetState = true;
	}
}

void FiveGameInit::ReloadGame()
{
	OnGameRequestLoad();

	*g_initState = MapInitState(14);
}

static InitFunction initFunctionStartGame([]()
{
	OnGameFrame.Connect([]()
	{
		if (g_showWarningMessage)
		{
			MessageBoxA(0, "I just crapped myself", "I just crapped myself", 0);
			uint32_t titleHash = HashString("CFX_NETERR_TITLE");
			uint32_t messageHash = HashString("CFX_NETERR");
			uint32_t noneHash = 0;

			setWarningMessage(0, &titleHash, &messageHash, &noneHash, 2, 0, -1, 0, 0, 1, 0);

			if (getWarningResult(0, 0) == 2)
			{
				g_showWarningMessage = false;

				OnMsgConfirm();
			}
		}
	});
	Instance<ICoreGameInit>::Set(&g_gameInit);
});
#endif