#include "stdafx.h"
#include "createConverters.h"
#include "FloatBasic.h"
#include <Utils/logger.h>

HRESULT createFloatConverter( uint8_t chanOut, AudioCodecs::eSampleFormat fmtOut,
	const AVFrame& frame, std::unique_ptr<PcmConverter>& result )
{
	const AVSampleFormat sampleFormat = (AVSampleFormat)frame.format;
	if( sampleFormat == AVSampleFormat::AV_SAMPLE_FMT_NONE )
		return E_INVALIDARG;

	if( sampleFormat != AV_SAMPLE_FMT_FLTP )
	{
		logError( u8"Unsupported audio sample format" );
		return E_NOTIMPL;
	}

	bool makeInt16;
	switch( fmtOut )
	{
	case AudioCodecs::eSampleFormat::Fp32:
		makeInt16 = false;
		break;
	case AudioCodecs::eSampleFormat::Int16:
		makeInt16 = true;
		break;
	default:
		return E_INVALIDARG;
	}

	const AVChannelLayout& layout = frame.ch_layout;
	if( 1 == chanOut )
		return createMonoDownmixer( layout, result, makeInt16 );
	if( 2 == chanOut )
		return createStereoDownmixer( layout, result, makeInt16 );

	const int chans = layout.nb_channels;
	if( chans > 16 )
	{
		logError( u8"Unrealistically large count of channels in the audio" );
		return E_INVALIDARG;
	}
	result = std::make_unique<FloatInterleave>( (uint8_t)chans );
	return S_OK;
}