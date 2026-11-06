#pragma once
#include <API/createDecoder.h>
#include "ffIncludes.h"
#include <Downmix/PcmConverter.h>
#include "../../ComLightLib/comLightClient.h"
#include <Utils/iLoggerSetup.h>

struct DecoderInfo
{
	const char* codec;
	const char* outFormat;
	const char* sourceFormat;
	uint8_t outChans;
	uint8_t sourceChans;
};

class CodecContext: public iLoggerSetup
{
	AVCodecContext* ctx = nullptr;
	AVFrame* frame = nullptr;
	std::unique_ptr<PcmConverter> converter;
	ComLight::CComPtr<AudioCodecs::iSampleSource> source;
	AudioCodecs::eLogLevel logLevel = AudioCodecs::eLogLevel::Warning;
	bool endOfStream = false;

	HRESULT createContext( const AudioCodecs::sDecoderInit& decInit ) noexcept;

	HRESULT detectFormat() noexcept;

	HRESULT receiveFrame() noexcept;

	HRESULT sendNextPacket() noexcept;
	HRESULT sendFlushPacket() noexcept;

	static constexpr HRESULT E_AGAIN = hresultFromErrno( EAGAIN );

	void logMessage( AudioCodecs::eLogLevel lvl, const char8_t* message ) const noexcept final;
	AudioCodecs::eLogLevel maxLogLevel() const noexcept final;

public:
	CodecContext()  noexcept = default;
	CodecContext( const CodecContext& ) = delete;
	void swap( CodecContext& that ) noexcept;

	~CodecContext() noexcept;

	HRESULT initialise( const AudioCodecs::sDecoderInit& decInit, AudioCodecs::iSampleSource* source ) noexcept;

	inline void logFail( const char8_t* message ) const noexcept
	{
		if( nullptr != source )
			source->logMessage( AudioCodecs::eLogLevel::Error, message );
	}

	HRESULT makeOutputDesc( AudioCodecs::sPcmDesc& rdi ) const noexcept;

	// Feed next compressed sample to the decoder, decode to PCM, and convert to the output format
	HRESULT refillBuffer() noexcept;

	HRESULT readSamples( uint8_t* rdi, int requested, int& delivered ) noexcept;

	DecoderInfo getInfoStrings( const AudioCodecs::sDecoderInit& decInit ) const noexcept;
};