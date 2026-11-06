#pragma once
#include "MultiInput.h"

// Downmix 5 input channels to mono; the formula is ( LF + RF ) * w.front + ( LS + RS ) * w.surround + C * w.centre
class Mono5: public MultiInput<5>
{
	const sDownmixWeights weights;

public:
	// The selector order is [ LF, RF, LS, RS, C ]
	Mono5( const std::array<uint8_t, 5>& selector, const sDownmixWeights& dw ) noexcept:
		MultiInput<5>( selector ),
		weights( dw )
	{}

	class State: public VectorLoader
	{
		__m256 front, surround, centre;

	public:
		State( const Mono5& context, const AVFrame& frame ) noexcept:
			VectorLoader( context, frame )
		{
			front = _mm256_broadcast_ss( &context.weights.front );
			surround = _mm256_broadcast_ss( &context.weights.surround );
			centre = _mm256_broadcast_ss( &context.weights.centre );
		}

		__forceinline __m256 compute( const std::array<__m256, 5>& src ) const noexcept
		{
			__m256 acc = _mm256_mul_ps( src[ 4 ], centre );
			__m256 sumBack = _mm256_add_ps( src[ 2 ], src[ 3 ] );
			__m256 sumFront = _mm256_add_ps( src[ 0 ], src[ 1 ] );
			acc = _mm256_fmadd_ps( sumBack, surround, acc );
			acc = _mm256_fmadd_ps( sumFront, front, acc );
			return acc;
		}
	};

	State createContext( const AVFrame& frame ) const noexcept
	{
		return State{ *this, frame };
	}
};