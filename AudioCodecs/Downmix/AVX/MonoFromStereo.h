#pragma once
#include "StereoInput.h"
#include "../sDownmixWeights.h"

// Mix 2 source channels with equal weight 0.5
class MonoFromStereo: public StereoInput
{
	const float mul = 0.5f;

public:
	MonoFromStereo( const std::array<uint8_t, 2>& selector, const sDownmixWeights& weights ) noexcept:
		StereoInput( selector[ 0 ], selector[ 1 ] )
	{
		assert( weights.front == 0.5f );
		assert( weights.surround == 0 );
		assert( weights.centre == 0 );
	}
	MonoFromStereo( const MonoFromStereo& ) noexcept = default;

	class State: public VectorLoader
	{
		__m256 multiplier;
	public:
		State( const MonoFromStereo& context, const AVFrame& frame ) noexcept:
			VectorLoader( context, frame )
		{
			multiplier = _mm256_broadcast_ss( &context.mul );
		}

		__forceinline __m256 compute( const StereoSamples& src ) const noexcept
		{
			__m256 v = _mm256_add_ps( src.left, src.right );
			return _mm256_mul_ps( v, multiplier );
		}
	};

	State createContext( const AVFrame& frame ) const noexcept
	{
		return State{ *this, frame };
	}

	static constexpr uint8_t requiredChans = 2;
};