#pragma once
#include "iSampleDecoder.h"
#include "eAudioCodec.h"

namespace AudioCodecs
{
	// Initialisation parameters for sample decoder
	struct sDecoderInit
	{
		// Audio codec of the input samples
		eAudioCodec codec;
		// Ideal count of output channels
		// When unable to convert internally, the decoder may create a stream with different count of them.
		uint8_t outputChannels;
		// Ideal output format; when unable to convert internally, the decoder may create a stream with different output format.
		eSampleFormat outputFormat;
		// Expected sample frequency of the output PCM data.
		// When the decoder disagrees, may create output stream with different frequency.
		int sampleRate;
		// Magic blob with codec initialisation data from the container.
		const uint8_t* configBlob;
		// Count of bytes in the blob
		uint32_t configBlobBytes;

		bool hasConfigBlob() const noexcept
		{
			return configBlob != nullptr && configBlobBytes != 0;
		}
	};

	// Create a decoder. On success, the output object retains the ownership of the sample source with IUnknown.AddRef.
	extern "C" HRESULT __stdcall createSampleDecoder( const sDecoderInit& decInit, iSampleSource* source, iDecodedStream** result ) noexcept;
}