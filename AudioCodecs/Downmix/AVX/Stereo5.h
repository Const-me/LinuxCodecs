#pragma once
#include "MultiInput.h"

// Downmix 5 input channels to mono; the formula is ( LF + RF ) * w.front + ( LS + RS ) * w.surround + C * w.centre
class Stereo5: public MultiInput<5>
{
	const sDownmixWeights weights;

public:
	// The selector order is [ LF, RF, LS, RS, C ]
	Stereo5( const std::array<uint8_t, 5>& selector, const sDownmixWeights& dw ) noexcept:
		MultiInput<5>( selector ),
		weights( dw )
	{}

	class State: public VectorLoader
	{
		__m256 front, surround, centre;

	public:
		State( const Stereo5& context, const AVFrame& frame ) noexcept:
			VectorLoader( context, frame )
		{
			front = _mm256_broadcast_ss( &context.weights.front );
			surround = _mm256_broadcast_ss( &context.weights.surround );
			centre = _mm256_broadcast_ss( &context.weights.centre );
		}

		__forceinline StereoSamples compute( const std::array<__m256, 5>& src ) const noexcept
		{
			const __m256 tmp = _mm256_mul_ps( src[ 4 ], centre );
			__m256 left = _mm256_fmadd_ps( src[ 2 ], surround, tmp );
			__m256 right = _mm256_fmadd_ps( src[ 3 ], surround, tmp );
			left = _mm256_fmadd_ps( src[ 0 ], front, left );
			right = _mm256_fmadd_ps( src[ 1 ], front, right );
			return StereoSamples{ left, right };
		}
	};

	State createContext( const AVFrame& frame ) const noexcept
	{
		return State{ *this, frame };
	}
};