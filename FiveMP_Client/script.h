#pragma once

NativeHandler GetNativeHandler(UINT64 hash);
void SpawnScriptHook();
void BypassOnlineModelRequestBlock();
void NoIntro();
void WAIT(DWORD ms);
eThreadState Trampoline(GtaThread* This);

extern HANDLE mainFiber;
extern DWORD wakeAt;
extern GtaThread_VTable gGtaThreadOriginal;
extern GtaThread_VTable gGtaThreadNew;
extern BlipList* pBlipList;
extern GtaThread* pThreadCopy;