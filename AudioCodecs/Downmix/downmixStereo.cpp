#include "stdafx.h"
#include "createConverters.h"
#include "AVX/FloatStereoConverter.h"
#include "AVX/StereoInterleave.h"
#include "AVX/Stereo3.h"
#include "AVX/Stereo4.h"
#include "AVX/Stereo5.h"
#include "ChannelLayout.h"
#include "FloatBasic.h"

template<class Impl>
inline HRESULT createDownmixer( const ChannelLayout& layout, std::unique_ptr<PcmConverter>& result, bool makeInt16 )
{
	// Fetch layout information
	constexpr uint8_t requiredChans = Impl::requiredChans;
	std::array<uint8_t, requiredChans> selector;
	CHECK( layout.makeChannelMap( selector.data(), requiredChans ) );

	// Compute downmixing coefficients
	sDownmixWeights weights;
	constexpr bool stereo = true;
	CHECK( layout.downmixWeights( weights, stereo ) );

	// Create the kernel class
	Impl impl{ selector, weights };

	// Create the PcmConverter-derived template class on the heap
	if( makeInt16 )
	{
		using TConv = FloatStereoConverter<Impl, true>;
		result = std::make_unique<TConv>( layout.sourceChannels(), impl );
	}
	else
	{
		using TConv = FloatStereoConverter<Impl, false>;
		result = std::make_unique<TConv>( layout.sourceChannels(), impl );
	}
	return S_OK;
}

HRESULT createStereoDownmixer( const AVChannelLayout& avLayout, std::unique_ptr<PcmConverter>& result, bool makeInt16 )
{
	ChannelLayout layout;
	CHECK( layout.initialise( avLayout ) );

	switch( layout.getLayout() )
	{
	case eChannelLayout::Mono:
		return createPassthrough( layout, result, makeInt16 );
	case eChannelLayout::Stereo:
		return createDownmixer<StereoInterleave>( layout, result, makeInt16 );
	case eChannelLayout::StereoCentre:
		return createDownmixer<Stereo3>( layout, result, makeInt16 );
	case eChannelLayout::Quad:
		return createDownmixer<Stereo4>( layout, result, makeInt16 );
	case eChannelLayout::QuadCentre:
		return createDownmixer<Stereo5>( layout, result, makeInt16 );
	};

	return E_NOTIMPL;
}