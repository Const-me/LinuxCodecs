#pragma once
#include "../sDownmixWeights.h"
#include "avxUtils.h"

// Base class to load vectors from 2 planar channels
template<size_t len>
class MultiInput
{
	std::array<uint8_t, len> sourceIndices;

public:
	MultiInput( const std::array<uint8_t, len>& arr ):
		sourceIndices( arr )
	{}

	class VectorLoader
	{
		std::array<const float*, len> pointers;

	public:
		VectorLoader( const MultiInput& context, const AVFrame& frame ) noexcept
		{
			for( size_t i = 0; i < len; i++ )
			{
				const float* rsi = (const float*)frame.data[ context.sourceIndices[ i ] ];
				assert( nullptr != rsi );
				pointers[ i ] = rsi;
			}
		}

		__forceinline std::array<__m256, len> load( size_t i ) const noexcept;
		__forceinline std::array<__m256, len> loadRem( size_t i, const __m256i mask ) const noexcept;
	};

	static constexpr uint8_t requiredChans = (uint8_t)len;
};

template<>
__forceinline std::array<__m256, 3> MultiInput<3>::VectorLoader::load( size_t i ) const noexcept
{
	return
	{
		_mm256_loadu_ps( pointers[ 0 ] + i ),
		_mm256_loadu_ps( pointers[ 1 ] + i ),
		_mm256_loadu_ps( pointers[ 2 ] + i ),
	};
}

template<>
__forceinline std::array<__m256, 3> MultiInput<3>::VectorLoader::loadRem( size_t i, const __m256i mask ) const noexcept
{
	return
	{
		_mm256_maskload_ps( pointers[ 0 ] + i, mask ),
		_mm256_maskload_ps( pointers[ 1 ] + i, mask ),
		_mm256_maskload_ps( pointers[ 2 ] + i, mask ),
	};
}

template<>
__forceinline std::array<__m256, 4> MultiInput<4>::VectorLoader::load( size_t i ) const noexcept
{
	return
	{
		_mm256_loadu_ps( pointers[ 0 ] + i ),
		_mm256_loadu_ps( pointers[ 1 ] + i ),
		_mm256_loadu_ps( pointers[ 2 ] + i ),
		_mm256_loadu_ps( pointers[ 3 ] + i ),
	};
}

template<>
__forceinline std::array<__m256, 4> MultiInput<4>::VectorLoader::loadRem( size_t i, const __m256i mask ) const noexcept
{
	return
	{
		_mm256_maskload_ps( pointers[ 0 ] + i, mask ),
		_mm256_maskload_ps( pointers[ 1 ] + i, mask ),
		_mm256_maskload_ps( pointers[ 2 ] + i, mask ),
		_mm256_maskload_ps( pointers[ 3 ] + i, mask ),
	};
}

template<>
__forceinline std::array<__m256, 5> MultiInput<5>::VectorLoader::load( size_t i ) const noexcept
{
	return
	{
		_mm256_loadu_ps( pointers[ 0 ] + i ),
		_mm256_loadu_ps( pointers[ 1 ] + i ),
		_mm256_loadu_ps( pointers[ 2 ] + i ),
		_mm256_loadu_ps( pointers[ 3 ] + i ),
		_mm256_loadu_ps( pointers[ 4 ] + i ),
	};
}

template<>
__forceinline std::array<__m256, 5> MultiInput<5>::VectorLoader::loadRem( size_t i, const __m256i mask ) const noexcept
{
	return
	{
		_mm256_maskload_ps( pointers[ 0 ] + i, mask ),
		_mm256_maskload_ps( pointers[ 1 ] + i, mask ),
		_mm256_maskload_ps( pointers[ 2 ] + i, mask ),
		_mm256_maskload_ps( pointers[ 3 ] + i, mask ),
		_mm256_maskload_ps( pointers[ 4 ] + i, mask ),
	};
}