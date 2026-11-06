#pragma once

namespace AudioCodecs
{
	enum struct eSampleFormat: uint8_t
	{
		// int16 PCM audio
		Int16 = 1,
		// fp32 PCM audio
		Fp32 = 2,
	};

	// Metadata about decoder output
	struct sPcmDesc
	{
		// Sample rate in Hz
		int frequency = 0;
		// Count of channels in the output stream; samples are interleaved.
		uint8_t channels = 0;
		// Format of these samples
		eSampleFormat sampleFormat = (eSampleFormat)0;
	};
}