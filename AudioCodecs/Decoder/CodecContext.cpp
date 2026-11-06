#include "stdafx.h"
#include "CodecContext.h"
#include <Utils/logger.h>

CodecContext::~CodecContext() noexcept
{
	av_frame_free( &frame );
	avcodec_free_context( &ctx );
}

HRESULT CodecContext::initialise( const AudioCodecs::sDecoderInit& decInit, AudioCodecs::iSampleSource* source ) noexcept
{
	if( nullptr == source )
		return E_POINTER;
	if( nullptr != ctx )
		return E_ALREADY_INITIALIZED;
	this->source = source;
	logLevel = (AudioCodecs::eLogLevel)(uint8_t)source->maxLogLevel();
	LoggerSetupRaii loggerSetup{ this };

	CHECK( createContext( decInit ) );
	CHECK( detectFormat() );
	CHECK( PcmConverter::create( decInit.outputChannels, decInit.outputFormat, *frame, converter ) );
	// Convert the initially decoded sample into the output layout
	CHECK( converter->convertSample( *frame ) );
	return S_OK;
}

namespace
{
	constexpr uint8_t maxEmptySamples = 0x10;

	static AVPacket createPacket()
	{
		AVPacket packet;
		// Copy-pasted following from `static void get_packet_defaults( AVPacket *pkt )` in packet.c
		memset( &packet, 0, sizeof( AVPacket ) );
		packet.pts = AV_NOPTS_VALUE;
		packet.dts = AV_NOPTS_VALUE;
		packet.pos = -1;
		packet.time_base = av_make_q( 0, 1 );
		return packet;
	}
}

HRESULT CodecContext::sendNextPacket() noexcept
{
	AudioCodecs::sSampleBuffer buffer;
	uint8_t emptySamplesCounter = 0;
	do
	{
		const HRESULT hr = source->getNextSample( buffer );
		if( S_OK != hr )
		{
			// Either fail or EOS; propagate to the caller
			return hr;
		}
		emptySamplesCounter++;
	} while( buffer.length == 0 && emptySamplesCounter < maxEmptySamples );

	if( buffer.length == 0 )
	{
		logFail( u8"The container supplied a long sequence of zero length samples; the media is probably corrupt." );
		return E_UNEXPECTED;
	}

	if( buffer.rsi == nullptr )
		return E_POINTER;
	if( buffer.length < 0 )
		return E_INVALIDARG;

	AVPacket packet = createPacket();
	packet.data = const_cast<uint8_t*>( buffer.rsi );
	packet.size = buffer.length;

	const int avCode = avcodec_send_packet( ctx, &packet );
	if( avCode >= 0 )
		return S_OK;

	logFail( u8"Unable to submit input media sample to the audio decoder." );
	return failedStatus( avCode );
}

HRESULT CodecContext::sendFlushPacket() noexcept
{
	const int avCode = avcodec_send_packet( ctx, nullptr );
	if( avCode >= 0 )
		return S_OK;
	logWarning( u8"Unable to submit flush media sample to the audio decoder." );
	return failedStatus( avCode );
}

HRESULT CodecContext::receiveFrame() noexcept
{
	if( nullptr == ctx || nullptr == frame )
		return OLE_E_BLANK;
	AV_CHECK( avcodec_receive_frame( ctx, frame ) );
	return S_OK;
}

HRESULT CodecContext::refillBuffer() noexcept
{
	constexpr uint8_t maxSamples = 0x10;

	uint8_t sampleCounter;
	for( sampleCounter = 0; sampleCounter < maxSamples; sampleCounter++ )
	{
		HRESULT hr = sendNextPacket();
		CHECK( hr );
		if( S_OK != hr )
			return S_FALSE;	//< iSampleSource object reported end of stream

		hr = receiveFrame();
		if( hr == E_AGAIN )
			continue;
		CHECK( hr );
		if( 0 == frame->nb_samples )
			continue;
		break;
	}

	if( sampleCounter >= maxSamples )
	{
		logFail( u8"The decoder consumed non-trivial count of encoded samples, but failed to deliver any audio." );
		return E_INVALIDARG;
	}

	return converter->convertSample( *frame );
}

HRESULT CodecContext::readSamples( uint8_t* rdi, int requested, int& delivered ) noexcept
{
	if( nullptr == source || nullptr == ctx || !converter )
		return OLE_E_BLANK;

	int upd;
	HRESULT hr;
	delivered = 0;
	while( true )
	{
		// Supply buffered audio, if any
		hr = converter->copyOutput( rdi, requested, upd );
		CHECK( hr );
		if( S_OK == hr )
		{
			delivered += upd;
			requested -= upd;
			if( requested <= 0 )
				return S_OK;	// Filled the requested count of samples, report success
			rdi += converter->bytesFromSamples( (uint32_t)upd );
		}

		// Try to pull next sample from the decoder without supplying moar encoded data
		hr = receiveFrame();
		if( endOfStream && hr == MF_E_END_OF_STREAM )
			break;
		if( hr != E_AGAIN )
		{
			CHECK( hr );
			CHECK( converter->convertSample( *frame ) );
			continue;
		}

		if( endOfStream )
			break;

		hr = refillBuffer();
		CHECK( hr );
		if( S_OK == hr )
			continue;

		endOfStream = true;
		// submit nullptr sample to the decoder
		hr = sendFlushPacket();
		if( FAILED( hr ) )
		{
			logWarning( u8"Unable to flush the audio decoder." );
			break;
		}

		hr = receiveFrame();
		if( FAILED( hr ) )
		{
			// That's prolly not an error, not all codecs needs flushing
			break;
		}
		hr = converter->convertSample( *frame );
		CHECK( hr );
	}

	return ( 0 == delivered ) ? S_FALSE : S_OK;
}