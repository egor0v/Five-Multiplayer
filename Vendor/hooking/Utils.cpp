/*
 * This file is part of the CitizenFX project - http://citizen.re/
 *
 * See LICENSE and MENTIONS in the root of the source tree for information
 * regarding licensing.
 */

#include <stdafx.h>
#include "StdInc.h"
#include "Utils.h"
#include <sstream>
#include <iomanip>
#include <mutex>

static STATIC InitFunctionBase* g_initFunctions;

InitFunctionBase::InitFunctionBase(int order /* = 0 */)
	: m_order(order)
{

}

void InitFunctionBase::Register()
{
	if (!g_initFunctions)
	{
		m_next = nullptr;
		g_initFunctions = this;
	}
	else
	{
		InitFunctionBase* cur = g_initFunctions;
		InitFunctionBase* last = nullptr;

		while (cur && m_order >= cur->m_order)
		{
			last = cur;
			cur = cur->m_next;
		}

		m_next = cur;

		(!last ? g_initFunctions : last->m_next) = this;
	}
}

void InitFunctionBase::RunAll()
{
	for (InitFunctionBase* func = g_initFunctions; func; func = func->m_next)
	{
		func->Run();
	}
}

#define BUFFER_COUNT 8
#define BUFFER_LENGTH 32768
const char* va(const char* string, ...)
{
	static __thread int currentBuffer;
	static std::vector<char> buffer;

	if (!buffer.size())
	{
		buffer.resize(BUFFER_COUNT * BUFFER_LENGTH);
	}

	int thisBuffer = currentBuffer;

	va_list ap;
	va_start(ap, string);
	int length = vsnprintf(&buffer[thisBuffer * BUFFER_LENGTH], BUFFER_LENGTH, string, ap);
	va_end(ap);

	if (length >= BUFFER_LENGTH)
	{
		//GlobalError("Attempted to overrun string in call to va()!");
		exit(1);
	}

	buffer[(thisBuffer * BUFFER_LENGTH) + BUFFER_LENGTH - 1] = '\0';

	currentBuffer = (currentBuffer + 1) % BUFFER_COUNT;

	return &buffer[thisBuffer * BUFFER_LENGTH];
}
const wchar_t* va(const wchar_t* string, ...)
{
	static __thread int currentBuffer;
	static std::vector<wchar_t> buffer;

	if (!buffer.size())
	{
		buffer.resize(BUFFER_COUNT * BUFFER_LENGTH);
	}

	int thisBuffer = currentBuffer;

	va_list ap;
	va_start(ap, string);
	int length = vswprintf(&buffer[thisBuffer * BUFFER_LENGTH], BUFFER_LENGTH, string, ap);
	va_end(ap);

	if (length >= BUFFER_LENGTH)
	{
		//GlobalError("Attempted to overrun string in call to va()!");
		exit(1);
	}

	buffer[(thisBuffer * BUFFER_LENGTH) + BUFFER_LENGTH - 1] = '\0';

	currentBuffer = (currentBuffer + 1) % BUFFER_COUNT;

	return &buffer[thisBuffer * BUFFER_LENGTH];
}
#ifdef _WIN32
#include <winternl.h>

void DoNtRaiseException(EXCEPTION_RECORD* record)
{
	typedef NTSTATUS(*NtRaiseExceptionType)(PEXCEPTION_RECORD record, PCONTEXT context, BOOL firstChance);

	bool threw = false;

	CONTEXT context;
	RtlCaptureContext(&context);

	static NtRaiseExceptionType NtRaiseException = (NtRaiseExceptionType)GetProcAddress(GetModuleHandle(L"ntdll.dll"), "NtRaiseException");

	// where will this function return to? hint: it'll return RIGHT AFTER RTLCAPTURECONTEXT, as that's what's in context->Rip!
	// therefore, check if 'threw' is false first...
	if (!threw)
	{
		threw = true;
		NtRaiseException(record, &context, TRUE);

		// force 'threw' to be stack-allocated by messing with it from here (where it won't execute)
		OutputDebugStringA((char*)&threw);
	}
}
#endif

static void PerformFileLog(const char* string)
{
	FILE* logFile = _wfopen(MakeRelativeCitPath(L"CitizenFX.log").c_str(), L"a");

	if (logFile)
	{
		fprintf(logFile, "[%lld] %s", GetTickCount64(), string);
		fclose(logFile);
	}
}

void trace(const char* string, ...)
{
	//static thread_local std::vector<char> buffer;
	static std::vector<char> buffer;

	if (!buffer.size())
	{
		buffer.resize(BUFFER_LENGTH);
	}

	static CRITICAL_SECTION dbgCritSec;

	if (!dbgCritSec.DebugInfo)
	{
		InitializeCriticalSectionAndSpinCount(&dbgCritSec, 100);
	}

	va_list ap;
	va_start(ap, string);
	int length = vsnprintf(&buffer[0], BUFFER_LENGTH, string, ap);
	va_end(ap);

	if (length >= BUFFER_LENGTH)
	{
		//GlobalError("Attempted to overrun string in call to trace()!");
		exit(1);
	}

#ifdef _WIN32
#if 0
	if (CoreIsDebuggerPresent())
	{
		// thanks to anti-debug workarounds (IsBeingDebugged == FALSE), we'll have to raise the exception to the debugger ourselves.
		// sadly, RaiseException (and, by extension, RtlRaiseException) won't raise a first-chance exception, so we'll have to do such by hand...
		// again, it may appear things 'work' if using a non-first-chance exception (i.e. the debugger will catch it), but VS doesn't like that and lets some
		// cases of the exception fall through.

		__try
		{
			EXCEPTION_RECORD record;
			record.ExceptionAddress = reinterpret_cast<PVOID>(_ReturnAddress());
			record.ExceptionCode = DBG_PRINTEXCEPTION_C;
			record.ExceptionFlags = 0;
			record.NumberParameters = 2;
			record.ExceptionInformation[0] = length + 1;
			record.ExceptionInformation[1] = reinterpret_cast<ULONG_PTR>(&buffer[0]);
			record.ExceptionRecord = &record;

			DoNtRaiseException(&record);
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			OutputDebugStringA(&buffer[0]);
		}
	}
	else
	{
		OutputDebugStringA(&buffer[0]);
	}

	if (!CoreIsDebuggerPresent() && !getenv("CitizenFX_ToolMode"))
	{
		printf("%s", &buffer[0]);
	}
#endif
#else
	printf("%s", &buffer[0]);
#endif

	PerformFileLog(&buffer[0]);
}

uint32_t HashRageString(const char* string)
{
	uint32_t hash = 0;
	size_t len = strlen(string);

	for (size_t i = 0; i < len; i++)
	{
		hash += string[i];
		hash += (hash << 10);
		hash ^= (hash >> 6);
	}

	hash += (hash << 3);
	hash ^= (hash >> 11);
	hash += (hash << 15);

	return hash;
}

uint32_t HashString(const char* string)
{
	uint32_t hash = 0;
	size_t len = strlen(string);

	for (size_t i = 0; i < len; i++)
	{
		hash += tolower(string[i]);
		hash += (hash << 10);
		hash ^= (hash >> 6);
	}

	hash += (hash << 3);
	hash ^= (hash >> 11);
	hash += (hash << 15);

	return hash;
}

fwString url_encode(const fwString &value)
{
	std::ostringstream escaped;
	escaped.fill('0');
	escaped << std::hex;

	for (std::string::const_iterator i = value.begin(), n = value.end(); i != n; ++i)
	{
		std::string::value_type c = (*i);
		if (((c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '-' || c == '_' || c == '.' || c == '~') && c != '+')
		{
			escaped << c;
		}
		else if (c == ' ')
		{
			escaped << '+';
		}
		else
		{
			escaped << '%' << std::setw(2) << ((int)(uint8_t)c) << std::setw(0);
		}
	}

	return fwString(escaped.str().c_str());
}

bool UrlDecode(const std::string& in, std::string& out)
{
	out.clear();
	out.reserve(in.size());
	for (std::size_t i = 0; i < in.size(); ++i)
	{
		if (in[i] == '%')
		{
			if (i + 3 <= in.size())
			{
				int value = 0;
				std::istringstream is(in.substr(i + 1, 2));
				if (is >> std::hex >> value)
				{
					out += static_cast<char>(value);
					i += 2;
				}
				else
				{
					return false;
				}
			}
			else
			{
				return false;
			}
		}
		else if (in[i] == '+')
		{
			out += ' ';
		}
		else
		{
			out += in[i];
		}
	}
	return true;
}
std::string ExplodeAndReturn(std::string stringToExplode, int field, std::string delimiter) {
	std::vector<std::string> v = explode(delimiter, stringToExplode);
	std::string xcontent = v[field];
	char myArray[512 + 1];
	return strcpy(myArray, xcontent.c_str());
}
std::vector<std::string> explode(const std::string &delimiter, const std::string &str) {
	std::vector<std::string> arr;
	int strleng = str.length();
	int delleng = delimiter.length();
	if (delleng == 0)
		return arr;//no change

	int i = 0;
	int k = 0;
	while (i<strleng) {
		int j = 0;
		while (i + j<strleng && j<delleng && str[i + j] == delimiter[j])
			j++;
		if (j == delleng) { //found delimiter
			arr.push_back(str.substr(k, i - k));
			i += delleng;
			k = i;
		}
		else {
			i++;
		}
	}
	arr.push_back(str.substr(k, i - k));
	return arr;
}
void OutputToConsole(const wchar_t* szFormat, ...)
{
	WCHAR szBuff[1024];
	va_list arg;
	va_start(arg, szFormat);
	_vsnwprintf(szBuff, sizeof(szBuff), szFormat, arg);
	va_end(arg);

	OutputDebugString(szBuff);
}
const char* char_replace(char* string_rep, char match, char replace) {
	int changes = 0;
	char *newStr = new char[strlen(string_rep) + 1];
	int n = 0;
	while (*string_rep != '\0') {
		if (*string_rep == match) {
			newStr[n] = replace;
			changes++;
		} else {
			newStr[n] = *string_rep;
		}
		string_rep++;
		n++;
	}
	return newStr;
}

void SetThreadName(int dwThreadID, char* threadName)
{
	THREADNAME_INFO info;
	info.dwType = 0x1000;
	info.szName = threadName;
	info.dwThreadID = dwThreadID;
	info.dwFlags = 0;

	if (CoreIsDebuggerPresent())
	{
		EXCEPTION_RECORD record;
		record.ExceptionAddress = reinterpret_cast<PVOID>(_ReturnAddress());
		record.ExceptionCode = MS_VC_EXCEPTION;
		record.ExceptionFlags = 0;
		record.NumberParameters = sizeof(info) / sizeof(ULONG_PTR);
		memcpy(record.ExceptionInformation, &info, sizeof(info));
		record.ExceptionRecord = &record;

		DoNtRaiseException(&record);
	}
}

Vector3 getCoordsFromCam(float distance) //For now it's here but we should honestly just make a camera class and move it over.. TODO
{
	Vector3 camRot = CAM::GET_GAMEPLAY_CAM_ROT(2);
	Vector3 camCoords = CAM::GET_GAMEPLAY_CAM_COORD();
	float tZ = camRot.z * 0.0174532924F;
	float tX = camRot.x * 0.0174532924F;
	float absX = abs(cos(tX));
	return{ camCoords.x + (-sin(tZ) * absX) * distance, 0,
		camCoords.y + (cos(tZ) * absX) * distance, 0,
		camCoords.z + (sin(tX)) * distance, 0 };
	/*A Vector3 actually has six parts to it, not three: x, _paddingx, y, _paddingy, z, and _paddingz.
	So you can declare a Vector3 inside curly braces like this if you know to separate the real XYZ values from the padding values (I just use 0 for them)*/
}