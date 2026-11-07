#include "stdafx.h"
#include <span>
#include "applyConfig.h"
#include <Utils/logger.h>
#include <Utils/loggerUtils.h>

namespace
{
	using namespace AudioCodecs;
	using ReadOnlySpan = std::span<const uint8_t>;

	inline ReadOnlySpan getConfigBlob( const sDecoderInit& decInit ) noexcept
	{
		if( decInit.hasConfigBlob() )
			return ReadOnlySpan{ decInit.configBlob, decInit.configBlobBytes };
		return {};
	}

	HRESULT setExtraData( AVCodecContext* ctx, ReadOnlySpan span ) noexcept
	{
		uint8_t* const blobCopy = (uint8_t*)av_mallocz( AV_INPUT_BUFFER_PADDING_SIZE + span.size() );
		if( nullptr == blobCopy )
			return E_OUTOFMEMORY;

		memcpy( blobCopy, span.data(), span.size() );
		ctx->extradata_size = (int)span.size();
		ctx->extradata = blobCopy;
		return S_OK;
	}

#pragma pack( push, 1 )
	struct WaveFormatEx
	{
		uint16_t formatTag;
		uint16_t chans;
		uint32_t samplesPerSec;
		uint32_t avgBytesPerSec;
		uint16_t blockAlign;
		uint16_t bitsPerSample;
		uint16_t cbSize;
	};
#pragma pack( pop )

	HRESULT waveFormatConfig( AVCodecContext* ctx, ReadOnlySpan span, eAudioCodec codec ) noexcept
	{
		if( span.empty() )
		{
			printError( u8"%s decoder requires a config blob.", cstr( codec ) );
			return E_INVALIDARG;
		}

		constexpr int cbWaveFormat = sizeof( WaveFormatEx );
		if( span.size() < cbWaveFormat )
		{
			printError( u8"%s decoder requires a config blob of at least %i bytes.", cstr( codec ), cbWaveFormat );
			return E_INVALIDARG;
		}

		const WaveFormatEx& waveFormat = *(const WaveFormatEx*)( span.data() );

		av_channel_layout_default( &ctx->ch_layout, waveFormat.chans );
		if( ctx->sample_rate != waveFormat.samplesPerSec )
		{
			logWarning( u8"Inconsistent configuration data for the sample rate" );
			ctx->sample_rate = waveFormat.samplesPerSec;
		}

		ctx->block_align = waveFormat.blockAlign;
		ctx->bit_rate = waveFormat.avgBytesPerSec * 8ll;
		ctx->bits_per_coded_sample = waveFormat.bitsPerSample;

		if( waveFormat.cbSize == 0 )
			return S_OK;
		if( span.size() < cbWaveFormat + waveFormat.cbSize )
		{
			logWarning( u8"The codec configuration blob is incomplete" );
			return S_OK;
		}

		span = span.subspan( cbWaveFormat, waveFormat.cbSize );
		return setExtraData( ctx, span );
	}
}

HRESULT applyConfig( AVCodecContext* ctx, const sDecoderInit& decInit ) noexcept
{
	if( nullptr == ctx )
		return E_POINTER;

	ReadOnlySpan span = getConfigBlob( decInit );
	if( span.size() > 1024 * 64 )
	{
		logError( u8"Unrealistically long audio codec config blob; the container is probably bad." );
		return E_BOUNDS;
	}

	ctx->sample_rate = decInit.sampleRate;
	switch( decInit.codec )
	{
	case eAudioCodec::AAC:
	case eAudioCodec::Opus:
		if( span.empty() )
		{
			printWarning( u8"%s decoder requires a config blob.", cstr( decInit.codec ) );
			return S_OK;
		}
		return setExtraData( ctx, span );

	case eAudioCodec::DTS:
	case eAudioCodec::AC3:
	case eAudioCodec::EAC3:
		if( !span.empty() )
			printWarning( u8"%s decoder won’t use decoder config blob; input ignored.", cstr( decInit.codec ) );
		return S_OK;

	case eAudioCodec::Wma1:
	case eAudioCodec::Wma2:
	case eAudioCodec::WmaVoice:
	case eAudioCodec::WmaLossless:
	case eAudioCodec::WmaPro:
		return waveFormatConfig( ctx, span, decInit.codec );

	default:
		return E_UNEXPECTED;
	}
}