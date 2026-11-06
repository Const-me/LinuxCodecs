#pragma once
#include "MultiInput.h"

// Downmix 4 input channels to mono; the formula is [ LF + RF ] * weightFront + [ LB + RB ] * weightBack
class Stereo4: public MultiInput<4>
{
	float mulFront, mulBack;

public:
	// The selector order is [ LF, RF, LB, RB ]
	Stereo4( const std::array<uint8_t, 4>& selector, const sDownmixWeights& weights ) noexcept:
		MultiInput<4>( selector )
	{
		assert( 0 == weights.centre );
		mulFront = weights.front;
		mulBack = weights.surround;
	}

	class State: public VectorLoader
	{
		__m256 mulFront, mulBack;

	public:
		State( const Stereo4& context, const AVFrame& frame ) noexcept:
			VectorLoader( context, frame )
		{
			const __m256d tmp64 = _mm256_broadcast_sd( (const double*)( &context.mulFront ) );
			const __m256 tmp = _mm256_castpd_ps( tmp64 );
			mulFront = _mm256_moveldup_ps( tmp );
			mulBack = _mm256_movehdup_ps( tmp );
		}

		__forceinline StereoSamples compute( const std::array<__m256, 4>& src ) const noexcept
		{
			__m256 left = _mm256_mul_ps( src[ 2 ], mulBack );
			__m256 right = _mm256_mul_ps( src[ 3 ], mulBack );
			left = _mm256_fmadd_ps( src[ 0 ], mulFront, left );
			right = _mm256_fmadd_ps( src[ 1 ], mulFront, right );
			return StereoSamples{ left, right };
		}
	};

	State createContext( const AVFrame& frame ) const noexcept
	{
		return State{ *this, frame };
	}
};