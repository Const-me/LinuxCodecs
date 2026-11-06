#pragma once
#include "StereoInput.h"
#include "../sDownmixWeights.h"

class StereoInterleave: public StereoInput
{
public:
	StereoInterleave( const std::array<uint8_t, 2>& selector, const sDownmixWeights& weights ) noexcept:
		StereoInput( selector[ 0 ], selector[ 1 ] )
	{
		assert( weights.front == 1 );
		assert( weights.surround == 0 );
		assert( weights.centre == 0 );
	}

	class State: public VectorLoader
	{
	public:
		State( const StereoInterleave& context, const AVFrame& frame ) noexcept:
			VectorLoader( context, frame )
		{}

		__forceinline const StereoSamples& compute( const StereoSamples& src ) const noexcept
		{
			return src;
		}
	};

	State createContext( const AVFrame& frame ) const noexcept
	{
		return State{ *this, frame };
	}

	static constexpr uint8_t requiredChans = 2;
};