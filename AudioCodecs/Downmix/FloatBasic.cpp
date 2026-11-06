#include "stdafx.h"
#include <array>
#include "FloatBasic.h"
#include "ChannelLayout.h"
#include <Utils/logger.h>
#include "AVX/avxUtils.h"

template<bool makeInt16>
FloatPassthrough<makeInt16>::FloatPassthrough( uint8_t chanIn, uint8_t sourceChanIdx ) noexcept:
	PcmConverter( 1, ConverterTraits<makeInt16>::outputFormat, chanIn, AV_SAMPLE_FMT_FLTP ),
	chanIndex( sourceChanIdx )
{
	assert( sourceChanIdx <= chanIn );
}

template<bool makeInt16>
uint32_t FloatPassthrough<makeInt16>::samplesFromBytes( uint32_t bytes ) const noexcept
{
	using Traits = ConverterTraits<makeInt16>;
	return bytes / Traits::cbElement;
}

template<>
HRESULT FloatPassthrough<false>::convertImpl( const AVFrame& frame ) noexcept
{
	const size_t elements = (uint32_t)frame.nb_samples;
	float* const rdi = (float*)bufferPointer();
	const float* const rsi0 = (const float*)frame.data[ chanIndex ];
	std::memcpy( rdi, rsi0, elements * 4 );
	return S_OK;
}

template<>
HRESULT FloatPassthrough<true>::convertImpl( const AVFrame& frame ) noexcept
{
	const size_t elements = (uint32_t)frame.nb_samples;
	int16_t* const rdi = (int16_t*)bufferPointer();
	const float* const rsi0 = (const float*)frame.data[ chanIndex ];

	constexpr size_t maskAlign8 = ~(size_t)7;
	const size_t endAligned = ( elements & maskAlign8 );

	size_t i;
	using Traits = ConverterTraits<true>;
	for( i = 0; i < endAligned; i += 8 )
	{
		__m256 v = _mm256_loadu_ps( rsi0 + i );
		Traits::storeMono( rdi + i, v );
	}

	const size_t rem = elements % 8;
	if( 0 != rem )
	{
		const __m256i mask = remainderLoadMask8( rem );
		__m256 v = _mm256_maskload_ps( rsi0 + i, mask );
		Traits::storeMono( rdi + i, v );
	}
	return S_OK;
}

FloatInterleave::FloatInterleave( uint8_t count ) noexcept:
	PcmConverter( std::min( count, (uint8_t)8 ), AudioCodecs::eSampleFormat::Fp32, count, AV_SAMPLE_FMT_FLTP )
{
	if( count > 8 )
		printWarning( u8"FloatInterleave.ctor: the input has too many channels, %i; will only deliver the first 8 of them.", count );
}

HRESULT FloatInterleave::convertImpl( const AVFrame& frame ) noexcept
{
	// Very inefficient, but this class is a fallback
	std::array<const float*, 8> sourcePointers;
	const size_t channels = this->channels;
	for( size_t i = 0; i < channels; i++ )
	{
		const float* rsi = (const float*)frame.data[ i ];
		if( nullptr == rsi )
			return E_POINTER;
		sourcePointers[ i ] = rsi;
	}

	float* rdi = (float*)bufferPointer();
	const size_t elements = (uint32_t)frame.nb_samples;
	const float* rsi = sourcePointers[ 0 ];
	const float* const rsiEnd = rsi + elements;
	while( rsi < rsiEnd )
	{
		*( rdi++ ) = *( rsi++ );
		for( size_t i = 1; i < channels; i++ )
			*( rdi++ ) = *( sourcePointers[ i ]++ );
	}
	return S_OK;
}

HRESULT createPassthrough( const ChannelLayout& layout, std::unique_ptr<PcmConverter>& result, bool makeInt16 )
{
	uint8_t idx;
	CHECK( layout.makeChannelMap( &idx, 1 ) );
	const uint8_t sourceChans = layout.sourceChannels();
	if( makeInt16 )
		result = std::make_unique<FloatPassthrough<true>>( sourceChans, idx );
	else
		result = std::make_unique<FloatPassthrough<false>>( sourceChans, idx );
	return S_OK;
}