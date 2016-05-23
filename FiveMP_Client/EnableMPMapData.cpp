/*
* This file is part of the CitizenFX project - http://citizen.re/
*
* See LICENSE and MENTIONS in the root of the source tree for information
* regarding licensing.
*/ 

/*
* Copyright (C) GTA:Multiplayer Team (https://wiki.gta-mp.net/index.php/Team)
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are
* met:
*
*     * Redistributions of source code must retain the above copyright
*		notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above
*		copyright notice, this list of conditions and the following disclaimer
*		in the documentation and/or other materials provided with the
*		distribution.
*     * Neither the name of GTA-Network nor the names of its
*		contributors may be used to endorse or promote products derived from
*		this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
* A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
* OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES INCLUDING, BUT NOT
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION HOWEVER CAUSED AND ON ANY
* THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* INCLUDING NEGLIGENCE OR OTHERWISE ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <StdInc.h>
#include <Hooking.h>
#include "EnableMPMapData.h"

#if 0 //unsure if we should use this tbh
static HookFunction hookFunction([]()
{
	// replace default loading of GROUP_MAP_SP DLC in some cases with consistent loading of GROUP_MAP (MP) DLC.
	char* location = hook::pattern("75 0D BA E2 99 8F 57").count(1).get(0).get<char>(0);

	hook::nop(location, 2);
	hook::put(location + 3, 0xBCC89179); // GROUP_MAP
});
#endif

void RevealFullMap(bool bToggle) {
	// GTA:Online Full Map
	static auto checkMultiplayerDrawMapFrame = FindPattern("\x40\x8A\x35\x00\x00\x00\x00\x84\xC0\x74\x05\x45\x84\xFF", "xxx????xxxxxxx");
	static uint8_t original[7] = { 0 };

	// Check if pattern is still valid
	if (!checkMultiplayerDrawMapFrame) return;

	// Initialize
	static bool bInitialized = false;
	if (!bInitialized) {
		bInitialized = true;

		// Unprotect, Original Memory
		VirtualProtect((void*)checkMultiplayerDrawMapFrame, 7, PAGE_EXECUTE_READWRITE, nullptr);
		memcpy(&original, (void*)checkMultiplayerDrawMapFrame, 7);
	}

	// Toggle
	if (bToggle) {
		// Nop
		memset((void*)checkMultiplayerDrawMapFrame, 0x90, 7);

		// Always draw full map
		*(uint16_t *)checkMultiplayerDrawMapFrame = 0xB640; // mov sil, 
		*(uint8_t *)(checkMultiplayerDrawMapFrame + 2) = 1; // 1 (mov sil, 1)
	} else {
		// Copy
		memcpy((void*)checkMultiplayerDrawMapFrame, &original, 7);
	}
}