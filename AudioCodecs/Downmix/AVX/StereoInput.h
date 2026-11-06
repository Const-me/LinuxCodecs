#pragma once

// Base class to load vectors from 2 planar channels
class StereoInput
{
	std::array<uint8_t, 2> sourceIndices;

public:
	StereoInput( uint8_t idxLeft, uint8_t idxRight )
	{
		sourceIndices[ 0 ] = idxLeft;
		sourceIndices[ 1 ] = idxRight;
		assert( idxLeft < 8 && idxRight < 8 && idxLeft != idxRight );
	}

	class VectorLoader
	{
		const float* rsiLeft;
		const float* rsiRight;

	public:
		VectorLoader( const StereoInput& context, const AVFrame& frame ) noexcept
		{
			rsiLeft = (const float*)frame.data[ context.sourceIndices[ 0 ] ];
			rsiRight = (const float*)frame.data[ context.sourceIndices[ 1 ] ];
		}

		__forceinline StereoSamples load( size_t i ) const noexcept
		{
			return StereoSamples
			{
				_mm256_loadu_ps( rsiLeft + i ),
				_mm256_loadu_ps( rsiRight + i )
			};
		}

		__forceinline StereoSamples loadRem( size_t i, const __m256i mask ) const noexcept
		{
			return StereoSamples
			{
				_mm256_maskload_ps( rsiLeft + i, mask ),
				_mm256_maskload_ps( rsiRight + i, mask )
			};
		}
	};
};