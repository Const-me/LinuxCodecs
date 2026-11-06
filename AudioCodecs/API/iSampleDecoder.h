#pragma once
#include "eLogLevel.h"
#include "sPcmDesc.h"

namespace AudioCodecs
{
	// A span of encoded bytes of the audio
	struct sSampleBuffer
	{
		// This library assumes encoded data is in unmanaged memory i.e. never moves,
		// decoder is the sole user of the sample source, and the data stays there until destroyed by another getNextSample call
		const uint8_t* rsi = nullptr;
		// Length of the data in bytes. iSampleSource should guarantee there're 64 extra readable bytes after the end, and these bytes are all zeros.
		int length = 0;
	};

	// C# implemented callback interface to pull compressed audio from containers
	// It assumes framing is solved by the container
	struct DECLSPEC_NOVTABLE iSampleSource: public ComLight::IUnknown
	{
		DEFINE_INTERFACE_ID( "433e59c9-7b9e-4702-9ef8-031573637c30" );

		// Fetch next chunk of compressed audio.
		// Returns S_OK on success, S_FALSE when the container doesn't have any more data, or a failed status.
		// Fails propagate out of the readSamples method.
		// End of streams too but not necessarily on the same call; some decoders flush final blocks.
		virtual HRESULT COMLIGHTCALL getNextSample( sSampleBuffer& buffer ) noexcept = 0;

		// Log a message
		virtual HRESULT COMLIGHTCALL logMessage( eLogLevel lvl, const char8_t* message ) noexcept = 0;

		// Maximum message level to log
		virtual int COMLIGHTCALL maxLogLevel() noexcept = 0;
	};

	// C++ implemented audio decoder object in the initialised state.
	// It retains iSampleSource object for its lifetime.
	struct DECLSPEC_NOVTABLE iDecodedStream: public ComLight::IUnknown
	{
		DEFINE_INTERFACE_ID( "53efbce8-63f7-49d3-b8bd-f03a54ab7d7e" );

		// Pull decoded PCM audio.
		// The integers expressed in samples per channel: requesting 10 samples for fp32 stereo needs a buffer of at least 80 bytes
		// The implementations should not assume any particular granularity; they must deliver the exact count, unless the stream has ended or things failed.
		virtual HRESULT COMLIGHTCALL readSamples( uint8_t* rdi, int requested, int& delivered ) noexcept = 0;

		// Fetch stream metadata
		virtual HRESULT COMLIGHTCALL getOutputDesc( sPcmDesc& desc ) const noexcept = 0;
	};
}