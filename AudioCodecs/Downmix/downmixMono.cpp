#include "stdafx.h"
#include "createConverters.h"
#include "AVX/FloatMonoConverter.h"
#include "AVX/MonoFromStereo.h"
#include "AVX/Mono3.h"
#include "AVX/Mono4.h"
#include "AVX/Mono5.h"
#include "FloatBasic.h"
#include "ChannelLayout.h"

namespace
{
	template<class Impl>
	inline HRESULT createDownmixer( const ChannelLayout& layout, std::unique_ptr<PcmConverter>& result, bool makeInt16 )
	{
		// Fetch layout information
		constexpr uint8_t requiredChans = Impl::requiredChans;
		std::array<uint8_t, requiredChans> selector;
		CHECK( layout.makeChannelMap( selector.data(), requiredChans ) );

		// Compute downmixing coefficients
		sDownmixWeights weights;
		constexpr bool stereo = false;
		CHECK( layout.downmixWeights( weights, stereo ) );

		// Create the kernel class
		Impl impl{ selector, weights };

		// Create the PcmConverter-derived template class on the heap
		if( makeInt16 )
		{
			using TConv = FloatMonoConverter<Impl, true>;
			result = std::make_unique<TConv>( layout.sourceChannels(), impl );
		}
		else
		{
			using TConv = FloatMonoConverter<Impl, false>;
			result = std::make_unique<TConv>( layout.sourceChannels(), impl );
		}
		return S_OK;
	}
}

HRESULT createMonoDownmixer( const AVChannelLayout& avLayout, std::unique_ptr<PcmConverter>& result, bool makeInt16 )
{
	ChannelLayout layout;
	CHECK( layout.initialise( avLayout ) );

	switch( layout.getLayout() )
	{
	case eChannelLayout::Mono:
		return createPassthrough( layout, result, makeInt16 );
	case eChannelLayout::Stereo:
		return createDownmixer<MonoFromStereo>( layout, result, makeInt16 );
	case eChannelLayout::StereoCentre:
		return createDownmixer<Mono3>( layout, result, makeInt16 );
	case eChannelLayout::Quad:
		return createDownmixer<Mono4>( layout, result, makeInt16 );
	case eChannelLayout::QuadCentre:
		return createDownmixer<Mono5>( layout, result, makeInt16 );
	default:
		assert( false );
		return E_UNEXPECTED;
	}
}