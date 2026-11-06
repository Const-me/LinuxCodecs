#pragma once
#include "MultiInput.h"

// Downmix 3 input channels to stereo; the formula is [ LF, RF ] * weightSides + C * weightCentre
class Stereo3: public MultiInput<3>
{
	float mulSides, mulCentre;

public:
	// The selector order is [ LF, RF, C ]
	Stereo3( const std::array<uint8_t, 3>& selector, const sDownmixWeights& weights ) noexcept:
		MultiInput<3>( selector )
	{
		mulSides = weights.front;
		mulCentre = weights.centre;
		assert( weights.surround == 0 );
	}

	class State: public VectorLoader
	{
		__m256 mulSides, mulCentre;

	public:
		State( const Stereo3& context, const AVFrame& frame ) noexcept:
			VectorLoader( context, frame )
		{
			const __m256d tmp64 = _mm256_broadcast_sd( (const double*)( &context.mulSides ) );
			const __m256 tmp = _mm256_castpd_ps( tmp64 );
			mulSides = _mm256_moveldup_ps( tmp );
			mulCentre = _mm256_movehdup_ps( tmp );
		}

		__forceinline StereoSamples compute( const std::array<__m256, 3>& src ) const noexcept
		{
			const __m256 tmp = _mm256_mul_ps( src[ 2 ], mulCentre );
			return StereoSamples
			{
				_mm256_fmadd_ps( src[ 0 ], mulSides, tmp ),
				_mm256_fmadd_ps( src[ 1 ], mulSides, tmp ),
			};
		}
	};

	State createContext( const AVFrame& frame ) const noexcept
	{
		return State{ *this, frame };
	}
};