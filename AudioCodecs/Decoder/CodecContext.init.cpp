#include "stdafx.h"
#include "CodecContext.h"
#include "applyConfig.h"

namespace
{
	constexpr uint16_t maxInitialSamples = 0x80;

	using namespace AudioCodecs;

	const char* decoderName( eAudioCodec ac ) noexcept
	{
		switch( ac )
		{
		case eAudioCodec::AAC: return "aac";
		case eAudioCodec::DTS: return "dca";
		case eAudioCodec::AC3: return "ac3";
		case eAudioCodec::EAC3: return "eac3";

		case eAudioCodec::Wma1: return "wmav1";
		case eAudioCodec::Wma2: return "wmav2";
		case eAudioCodec::WmaVoice: return "wmavoice";
		case eAudioCodec::WmaLossless: return "wmalossless";
		case eAudioCodec::WmaPro: return "wmapro";
		}
		return nullptr;
	}
}

HRESULT CodecContext::createContext( const sDecoderInit& decInit ) noexcept
{
	const char* const name = decoderName( decInit.codec );
	if( nullptr == name )
	{
		logFail( u8"Unknown or unsupported codec ID" );
		return E_INVALIDARG;
	}

	const AVCodec* const codec = avcodec_find_decoder_by_name( name );
	if( nullptr == codec )
	{
		logFail( u8"Unable to find a registered decoder with the specified name." );
		return E_FAIL;
	}

	ctx = avcodec_alloc_context3( codec );
	if( nullptr == ctx )
		return E_OUTOFMEMORY;

	CHECK( applyConfig( ctx, decInit ) );

	AV_CHECK( avcodec_open2( ctx, codec, nullptr ) );

	frame = av_frame_alloc();
	return S_OK;
}

HRESULT CodecContext::detectFormat() noexcept
{
	if( nullptr == source || nullptr == ctx )
		return OLE_E_BLANK;

	uint16_t sampleCounter;
	for( sampleCounter = 0; sampleCounter < maxInitialSamples; sampleCounter++ )
	{
		HRESULT hr = sendNextPacket();
		CHECK( hr );
		if( S_OK != hr )
		{
			endOfStream = true;
			return MF_E_END_OF_STREAM;
		}

		hr = receiveFrame();
		if( hr == E_AGAIN )
			continue;
		CHECK( hr );
		if( 0 == frame->nb_samples )
			continue;
		break;
	}

	if( sampleCounter >= maxInitialSamples )
	{
		logFail( u8"The decoder consumed non-trivial count of encoded samples, but failed to deliver any audio." );
		return E_INVALIDARG;
	}
	const int freq = frame->sample_rate;
	if( freq < 8000 || freq > 192000 )
	{
		logFail( u8"The decoder delivered unrealistic output sample rate" );
		return E_INVALIDARG;
	}
	const int chan = frame->ch_layout.nb_channels;
	if( chan < 1 || chan > 16 )
	{
		logFail( u8"The decoder delivered unrealistic count of channels" );
		return E_INVALIDARG;
	}
	const AVSampleFormat sampleFormat = (AVSampleFormat)frame->format;
	if( sampleFormat == AVSampleFormat::AV_SAMPLE_FMT_NONE )
	{
		logFail( u8"The decoder failed to deliver a sample format" );
		return E_INVALIDARG;
	}
	return S_OK;
}

HRESULT CodecContext::makeOutputDesc( AudioCodecs::sPcmDesc& rdi ) const noexcept
{
	if( !converter || nullptr == frame )
		return OLE_E_BLANK;

	rdi.channels = converter->channels;
	rdi.sampleFormat = converter->sampleFormat;
	rdi.frequency = frame->sample_rate;
	return S_OK;
}