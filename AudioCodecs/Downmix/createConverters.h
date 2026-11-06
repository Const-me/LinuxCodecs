#pragma once
#include "PcmConverter.h"

HRESULT createFloatConverter( uint8_t chanOut, AudioCodecs::eSampleFormat fmtOut,
	const AVFrame& frame, std::unique_ptr<PcmConverter>& result );

HRESULT createMonoDownmixer( const AVChannelLayout& avLayout, std::unique_ptr<PcmConverter>& result, bool makeInt16 );
HRESULT createStereoDownmixer( const AVChannelLayout& avLayout, std::unique_ptr<PcmConverter>& result, bool makeInt16 );