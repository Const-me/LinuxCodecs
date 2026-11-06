#include <stdafx.h>
#ifdef _MSC_VER
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

BOOL APIENTRY DllMain( HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved )
{
	switch( ul_reason_for_call )
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
#else
extern "C" void __debugbreak() noexcept
{
	__builtin_trap();
}
#endif // DEBUG