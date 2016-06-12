#include "stdafx.h"
#include "StdInc.h"
#include "sysAllocator.h"
#include "Hooking.Patterns.h"

namespace rage
{
	void* sysUseAllocator::operator new(size_t size)
	{
		return GetAllocator()->allocate(size, 16, 0);
	}

		void sysUseAllocator::operator delete(void* memory)
	{
		GetAllocator()->free(memory);
	}

	static uint32_t g_tlsOffset;

	uint32_t sysMemAllocator::GetAllocatorTlsOffset()
	{
		return g_tlsOffset;
	}
}

static HookFunction hookFunction([]()
{
	//Glorious copy paste from https://github.com/gallexme/Captain-Hook/blob/master/CaptainHook/Scripting/ScriptEngine.cpp, old signature makes everything crash in 757, unsure on what changed in the game except for a table being different.
	auto activeThreadTlsOffsetPattern = hook::pattern("48 8B 04 D0 4A 8B 14 00 48 8B 01 F3 44 0F 2C 42 20");
	rage::g_tlsOffset = *activeThreadTlsOffsetPattern.count(1).get(0).get<uint32_t>(-4);
	//rage::g_tlsOffset = *hook::pattern("B9 ? ? ? ? 48 8B 0C 01 45 33 C9 49 8B D2 48").get(0).get<uint32_t>(1);
});