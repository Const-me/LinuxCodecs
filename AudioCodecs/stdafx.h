#pragma once
#include <cassert>
#include <cstdint>
#include <cstring>
#include <immintrin.h>
#include <array>

#ifdef _MSC_VER
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Mferror.h>
#else

#define __stdcall
#define __forceinline inline  __attribute__((always_inline))
extern "C" void __debugbreak() noexcept;

namespace
{
	constexpr int MF_E_INVALIDMEDIATYPE = (int)0xC00D36B4u;
	constexpr int MF_E_END_OF_STREAM = (int)0xC00D3E84u;
	constexpr int MF_E_FORMAT_CHANGE_NOT_SUPPORTED = (int)0xC00D36FEu;
	constexpr int NTE_BUFFER_TOO_SMALL = (int)0x80090028;
}
#endif

#include "../../../CGML/ComLightLib/comLightCommon.h"