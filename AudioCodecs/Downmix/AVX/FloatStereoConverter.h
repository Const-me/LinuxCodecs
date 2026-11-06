#pragma once
#include "../PcmConverter.h"
#include "avxUtils.h"

// Template class which downmixes planar FP32 audio into a single channel,
// with some metaprogramming black magic to generate decent machine code
template<class T, bool makeInt16 = false>
class FloatStereoConverter final: public PcmConverter
{
	const T implementation;
	using Traits = ConverterTraits<makeInt16>;

public:
	FloatStereoConverter( uint8_t chanIn, const T& impl ) noexcept:
		PcmConverter( 2, Traits::outputFormat, chanIn, AV_SAMPLE_FMT_FLTP ),
		implementation( impl )
	{}
	~FloatStereoConverter() noexcept = default;

protected:
	uint32_t samplesFromBytes( uint32_t bytes ) const noexcept final
	{
		return bytes / ( Traits::cbElement * 2 );
	}

	HRESULT convertImpl( const AVFrame& frame ) noexcept final
	{
		const size_t elements = (uint32_t)frame.nb_samples;
		using E = typename Traits::E;
		E* const rdi = (E*)bufferPointer();
		const auto ctx = implementation.createContext( frame );

		constexpr size_t maskAlign8 = ~(size_t)7;
		const size_t endAligned = ( elements & maskAlign8 );
		size_t i;
		for( i = 0; i < endAligned; i += 8 )
		{
			auto src = ctx.load( i );
			Traits::storeStereo( rdi + ( i * 2 ), ctx.compute( src ) );
		}

		const size_t rem = elements % 8;
		if( 0 != rem )
		{
			const __m256i mask = remainderLoadMask8( rem );
			auto src = ctx.loadRem( i, mask );
			Traits::storeStereo( rdi + ( i * 2 ), ctx.compute( src ) );
		}

		return S_OK;
	}
};