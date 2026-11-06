#include "stdafx.h"
#include "PcmConverter.h"

namespace
{
	using namespace AudioCodecs;

	inline uint16_t bytesPerSample( uint16_t chanOut, eSampleFormat fmtOut )
	{
		switch( fmtOut )
		{
		case eSampleFormat::Int16:
			return chanOut * sizeof( int16_t );
		case eSampleFormat::Fp32:
			return chanOut * sizeof( float );
		default:
			assert( false );
			__debugbreak();
			return 0;
		}
	}
}

PcmConverter::PcmConverter( uint8_t chanOut, eSampleFormat fmtOut,
	uint8_t chanIn, AVSampleFormat fmtIn ) noexcept:
	channels( chanOut ), sampleFormat( fmtOut ), sourceChannels( chanIn ), sourceFormat( fmtIn ),
	cbSample( bytesPerSample( chanOut, fmtOut ) )
{}

HRESULT PcmConverter::copyOutput( void* rdi, const int requested, int& delivered ) const noexcept
{
	if( requested <= 0 )
	{
		delivered = 0;
		return S_OK;
	}

	const size_t cbOffset = bytesFromSamples( readOffset );
	const ptrdiff_t cbAvailable = buffer.size() - cbOffset;
	if( cbAvailable <= 0 )
	{
		delivered = 0;
		return S_FALSE;
	}

	const size_t cbReq = bytesFromSamples( (uint32_t)requested );
	const uint8_t* const rsi = buffer.data() + cbOffset;
	if( cbReq > (size_t)cbAvailable )
	{
		std::memcpy( rdi, rsi, cbAvailable );
		const uint32_t samples = samplesFromBytes( (uint32_t)cbAvailable );
		delivered = (int)samples;
		readOffset += samples;
	}
	else
	{
		std::memcpy( rdi, rsi, cbReq );
		delivered = requested;
		readOffset += (uint32_t)requested;
	}
	return S_OK;
}

HRESULT PcmConverter::convertSample( const AVFrame& frame ) noexcept
{
	const AVSampleFormat avsf = (AVSampleFormat)frame.format;
	const int chan = frame.ch_layout.nb_channels;
	if( avsf != sourceFormat || chan != sourceChannels )
		return MF_E_FORMAT_CHANGE_NOT_SUPPORTED;

	const int length = frame.nb_samples;
	if( length < 0 )
		return E_UNEXPECTED;
	if( length > ( 4 << 20 ) )
		return DISP_E_OVERFLOW;

	try
	{
		const size_t cb = bytesFromSamples( (uint32_t)length );
		// Round up to the next multiple of 64 bytes: much easier to implement the virtual method with AVX
		constexpr size_t maskAlign64 = ~(size_t)63;
		buffer.reserve( ( cb + 63 ) & maskAlign64 );
		buffer.resize( cb );
	}
	catch( const std::bad_alloc& )
	{
		return E_OUTOFMEMORY;
	}

	readOffset = 0;
	if( 0 == length )
		return S_FALSE;

	return convertImpl( frame );
}