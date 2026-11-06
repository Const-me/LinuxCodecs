#include "stdafx.h"
#include "avxUtils.h"

#ifdef __AVX2__
alignas( 16 ) const std::array<int8_t, 16> s_remainderMask =
#else
alignas( 64 ) const std::array<int, 16> s_remainderMask =
#endif
{
	-1, -1, -1, -1, -1, -1, -1, -1,
	0, 0, 0, 0, 0, 0, 0, 0
};