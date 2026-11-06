#include "stdafx.h"
#include "createConverters.h"

HRESULT PcmConverter::create( uint8_t chanOut, AudioCodecs::eSampleFormat fmtOut,
	const AVFrame& frame, std::unique_ptr<PcmConverter>& result ) noexcept
{
	const int chan = frame.ch_layout.nb_channels;
	if( chan < 1 || chan > 16 )
		return E_INVALIDARG;
	const AVSampleFormat sampleFormat = (AVSampleFormat)frame.format;
	if( sampleFormat == AVSampleFormat::AV_SAMPLE_FMT_NONE )
		return E_INVALIDARG;

	try
	{
		if( sampleFormat == AV_SAMPLE_FMT_FLTP )
			return createFloatConverter( chanOut, fmtOut, frame, result );

		return E_NOTIMPL;
	}
	catch( const std::bad_alloc& )
	{
		return E_OUTOFMEMORY;
	}
}