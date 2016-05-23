/*
* This file is part of the CitizenFX project - http://citizen.re/
*
* See LICENSE and MENTIONS in the root of the source tree for information
* regarding licensing.
*/

// This file contains code adapted from the original GTA IV script hook, the 
// copyright notice for which follows below.

/*****************************************************************************\

Copyright (C) 2009, Aru <oneforaru at gmail dot com>

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
claim that you wrote the original software. If you use this software
in a product, an acknowledgment in the product documentation would be
appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.

\*****************************************************************************/
#ifndef SCRENGINE_H
#define SCRENGINE_H

#pragma once
#include "stdafx.h"
#include "EventCore.h"
#include "pgCollection.h"
#include "scrThread.h"

namespace rage
{
	class
#ifdef COMPILING_RAGE_SCRIPTING_FIVE
		__declspec(dllexport)
#else
		__declspec(dllimport)
#endif
		scrEngine
	{
	public:
		static pgPtrCollection<GtaThread>* GetThreadCollection();

		static uint32_t IncrementThreadId();

		// gets the active thread
		static scrThread* GetActiveThread();

		// sets the currently running thread
		static void SetActiveThread(scrThread* thread);

		// adds a precreated custom thread to the runtime and starts it
		static void CreateThread(GtaThread* thread);

		// native function handler type
		typedef void(__cdecl * NativeHandler)(scrNativeCallContext* context);

		// gets a native function handler
		static NativeHandler GetNativeHandler(uint64_t hash);

		// queues the registration of a custom native function handler
		static void RegisterNativeHandler(const char* nativeName, NativeHandler handler);

		// queues the registration of a custom native function handler with an identifier
		static void RegisterNativeHandler(uint64_t nativeIdentifier, NativeHandler handler);

	public:
		static fwEvent<> OnScriptInit;

		static fwEvent<bool&> CheckNativeScriptAllowed;
	};

	class scriptHandlerMgr
	{
	public:
		virtual inline ~scriptHandlerMgr() {}

		virtual void m1() = 0;

		virtual void m2() = 0;

		virtual void m3() = 0;

		virtual void m4() = 0;

		virtual void m5() = 0;

		virtual void m6() = 0;

		virtual void m7() = 0;

		virtual void m8() = 0;

		virtual void m9() = 0;

		virtual void AttachScript(scrThread* thread) = 0;

		virtual void DetachScript(scrThread* thread) = 0;
	};
}
#endif