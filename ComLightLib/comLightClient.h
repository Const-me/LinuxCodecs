#pragma once
#include "comLightCommon.h"
#include "client/CComPtr.hpp"
#include "utils/typeTraits.hpp"

namespace ComLight
{
	namespace details
	{
		template<typename T>
		inline constexpr void** castDoublePointerToVoid( T** pp )
		{
			static_assert( pointersAssignable<IUnknown, T>(), "IID_PPV_ARGS macro should be used with IUnknown interfaces" );
			return reinterpret_cast<void**>( pp );
		}
	}
}

#define IID_PPV_CL( pp ) decltype( **pp )::iid, ::ComLight::details::castDoublePointerToVoid( pp )