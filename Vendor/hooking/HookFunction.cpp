/*
* This file is part of the CitizenFX project - http://citizen.re/
*
* See LICENSE and MENTIONS in the root of the source tree for information
* regarding licensing.
*/

#include "stdafx.h"
#include "HookFunction.h"

static HookFunctionBase* g_hookFunctions;

void HookFunctionBase::Register()
{
	m_next = g_hookFunctions;
	g_hookFunctions = this;
}

void HookFunctionBase::RunAll()
{
	//int x = 0;
	//char string[128];
	//MessageBoxA(0, "arg", "arg", 0);
	for (auto func = g_hookFunctions; func; func = func->m_next)
	{
		func->Run();
		//sprintf(string, "%d", x);
		//MessageBoxA(0, "last called func id", string, 0);
	}
}

static RuntimeHookFunction* g_runtimeHookFunctions;

void RuntimeHookFunction::Register()
{
	m_next = g_runtimeHookFunctions;
	g_runtimeHookFunctions = this;
}

void RuntimeHookFunction::Run(const char* key)
{
	for (auto func = g_runtimeHookFunctions; func; func = func->m_next)
	{
		if (func->m_key == key)
		{
			func->m_function();
		}
	}
}