#include "stdafx.h"
#include "StdInc.h"
#include "Hooking.h"
#include "Hooking.Patterns.h"
#include "Hooking.Invoke.h"
#include "fiDevice.h"

namespace rage
{
	hook::cdecl_stub<rage::fiDevice*(const char*, bool)> fiDevice__GetDevice([]()
	{
		return hook::pattern("41 B8 07 00 00 00 48 8B F1 E8").count(1).get(0).get<void>(-0x1F);
	});

	fiDevice* fiDevice::GetDevice(const char* path, bool allowRoot) { return fiDevice__GetDevice(path, allowRoot); }

	hook::cdecl_stub<bool(const char*, fiDevice*, bool)> fiDevice__MountGlobal([]()
	{
		return hook::pattern("41 8A F0 48 8B F9 E8 ? ? ? ? 33 DB 85 C0").count(1).get(0).get<void>(-0x28);
	});

	bool fiDevice::MountGlobal(const char* mountPoint, fiDevice* device, bool allowRoot)
	{
		return fiDevice__MountGlobal(mountPoint, device, allowRoot);
	}

	// DCEC20
	hook::cdecl_stub<void(const char*)> fiDevice__Unmount([]()
	{
		return hook::pattern("E8 ? ? ? ? 85 C0 75 23 48 83").count(1).get(0).get<void>(-0x22);
	});

	void fiDevice::Unmount(const char* rootPath) { fiDevice__Unmount(rootPath); }

	rage::fiDevice::~fiDevice() {}

	__declspec(dllexport) fwEvent<> fiDevice::OnInitialMount;
}