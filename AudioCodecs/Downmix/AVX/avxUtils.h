#pragma once
#include <array>
#include <API/sPcmDesc.h>

#ifdef __AVX2__
extern const std::array<int8_t, 16> s_remainderMask;
#else
extern const std::array<int, 16> s_remainderMask;
#endif

__forceinline __m256i remainderLoadMask8( size_t rem ) noexcept
{
	assert( rem >= 0 && rem <= 8 );
#ifdef __AVX2__
	const int8_t* rsi = &s_remainderMask[ 8 - rem ];
	__m128i v = _mm_loadu_si64( rsi );
	return _mm256_cvtepi8_epi32( v );
#else
	const int* rsi = &s_remainderMask[ 8 - rem ];
	return _mm256_loadu_si256( ( const __m256i* )rsi );
#endif
}

struct StereoSamples
{
	__m256 left, right;

	// Store interleaved samples; writes exactly 64 bytes to the pointer.
	__forceinline void storeInterleaved( float* rdi ) const noexcept
	{
		const __m256 t0 = _mm256_unpacklo_ps( left, right );
		const __m256 t1 = _mm256_unpackhi_ps( left, right );
		const __m256 r1 = _mm256_permute2f128_ps( t0, t1, 0x31 );
		const __m256 r0 = _mm256_insertf128_ps( t0, _mm256_castps256_ps128( t1 ), 1 );
		_mm256_storeu_ps( rdi + 8, r1 );
		_mm256_storeu_ps( rdi, r0 );
	}
};

template<bool makeInt16>
struct ConverterTraits;

template<>
struct ConverterTraits<false>
{
	using E = float;
	static constexpr AudioCodecs::eSampleFormat outputFormat = AudioCodecs::eSampleFormat::Fp32;
	static constexpr uint32_t cbElement = sizeof( E );

	// Store 8 fp32 PCM mono samples
	static __forceinline void storeMono( float* rdi, __m256 v ) noexcept
	{
		_mm256_storeu_ps( rdi, v );
	}

	// Store 8 fp32 PCM stereo samples, i.e. interleave
	static __forceinline void storeStereo( float* rdi, const StereoSamples& source ) noexcept
	{
		// l0, r0, l1, r1, l4, r4, l5, r5
		const __m256 t0 = _mm256_unpacklo_ps( source.left, source.right );
		// l2, r2, l3, r3, l6, r6, l7, r7
		const __m256 t1 = _mm256_unpackhi_ps( source.left, source.right );
		// l4, r4, l5, r5, l6, r6, l7, r7
		const __m256 r1 = _mm256_permute2f128_ps( t0, t1, 0x31 );
		// l0, r0, l1, r1, l2, r2, l3, r3
		const __m256 r0 = _mm256_insertf128_ps( t0, _mm256_castps256_ps128( t1 ), 1 );
		_mm256_storeu_ps( rdi + 8, r1 );
		_mm256_storeu_ps( rdi, r0 );
	}
};

template<>
struct ConverterTraits<true>
{
	using E = int16_t;
	static constexpr AudioCodecs::eSampleFormat outputFormat = AudioCodecs::eSampleFormat::Int16;
	static constexpr uint32_t cbElement = sizeof( E );

	// Store 8 int16 PCM mono samples: scale, round, and saturate
	static __forceinline void storeMono( int16_t* rdi, __m256 v ) noexcept
	{
		const __m256 mul = _mm256_set1_ps( 0x7FFF );
		v = _mm256_mul_ps( v, mul );
		v = _mm256_round_ps( v, _MM_FROUND_NINT );
		__m256i iv = _mm256_cvtps_epi32( v );
		__m128i low = _mm256_castsi256_si128( iv );
		__m128i high = _mm256_extracti128_si256( iv, 1 );
		__m128i result = _mm_packs_epi32( low, high );
		_mm_storeu_si128( ( __m128i* )rdi, result );
	}

	// Store 8 int16 PCM stereo samples: scale, round, saturate, and interleave
	static __forceinline void storeStereo( int16_t* rdi, const StereoSamples& source ) noexcept
	{
		const __m256 mul = _mm256_set1_ps( 0x7FFF );
		__m256 left = _mm256_mul_ps( source.left, mul );
		__m256 right = _mm256_mul_ps( source.right, mul );
		left = _mm256_round_ps( left, _MM_FROUND_NINT );
		right = _mm256_round_ps( right, _MM_FROUND_NINT );

		// l0, r0, l1, r1, l4, r4, l5, r5
		const __m256 t0 = _mm256_unpacklo_ps( left, right );
		// l2, r2, l3, r3, l6, r6, l7, r7
		const __m256 t1 = _mm256_unpackhi_ps( left, right );

		// l0, r0, l1, r1, l4, r4, l5, r5
		__m256i i0 = _mm256_cvtps_epi32( t0 );
		// l2, r2, l3, r3, l6, r6, l7, r7
		__m256i i1 = _mm256_cvtps_epi32( t1 );

		// The order of these numbers is already good; no expensive full-vector shuffles needed here
		__m256i iv = _mm256_packs_epi32( i0, i1 );

		_mm256_storeu_si256( ( __m256i* )rdi, iv );
	}
};