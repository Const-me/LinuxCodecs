#pragma once
#include "MultiInput.h"

// Downmix 3 input channels to mono; the formula is ( LF + RF ) * weightSides + C * weightCentre
class Mono3: public MultiInput<3>
{
	float mulSides, mulCentre;

public:
	// The selector order is [ LF, RF, C ]
	Mono3( const std::array<uint8_t, 3>& selector, const sDownmixWeights& weights ) noexcept:
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
		State( const Mono3& context, const AVFrame& frame ) noexcept:
			VectorLoader( context, frame )
		{
			const __m256d tmp64 = _mm256_broadcast_sd( (const double*)( &context.mulSides ) );
			const __m256 tmp = _mm256_castpd_ps( tmp64 );
			mulSides = _mm256_moveldup_ps( tmp );
			mulCentre = _mm256_movehdup_ps( tmp );
		}

		__forceinline __m256 compute( const std::array<__m256, 3>& src ) const noexcept
		{
			__m256 acc = _mm256_mul_ps( src[ 2 ], mulCentre );
			__m256 sum = _mm256_add_ps( src[ 0 ], src[ 1 ] );
			return _mm256_fmadd_ps( sum, mulSides, acc );
		}
	};

	State createContext( const AVFrame& frame ) const noexcept
	{
		return State{ *this, frame };
	}
};