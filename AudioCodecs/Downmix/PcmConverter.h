#pragma once
#include <memory>
#include <vector>
#include <API/sPcmDesc.h>
#include <Decoder/ffIncludes.h>

struct PcmConverter
{
	PcmConverter( uint8_t chanOut, AudioCodecs::eSampleFormat fmtOut,
		uint8_t chanIn, AVSampleFormat fmtIn ) noexcept;
	virtual ~PcmConverter() noexcept {}

	// Create the object; the output format is a desire not a contract. Input format however is fixed.
	static HRESULT create( uint8_t chanOut, AudioCodecs::eSampleFormat fmtOut,
		const AVFrame& frame, std::unique_ptr<PcmConverter>& result ) noexcept;

	// Count of output channels
	const uint8_t channels;
	// Data format of the output audio
	const AudioCodecs::eSampleFormat sampleFormat;

	// If there're unconsumed samples, copy them out of this class and advance.
	// Otherwise do nothing and return S_FALSE
	HRESULT copyOutput( void* rdi, const int requested, int& delivered ) const noexcept;

	// Verify the format's the same, resize output buffer to ( cbSample * nb_samples ), call the virtual method
	HRESULT convertSample( const AVFrame& frame ) noexcept;

	size_t bytesFromSamples( size_t samples ) const noexcept
	{
		return samples * cbSample;
	}

	const uint8_t sourceChannels;
	const AVSampleFormat sourceFormat;

protected:
	virtual HRESULT convertImpl( const AVFrame& frame ) noexcept = 0;

	// The indirect branches are very predictable here, the outcome never changes
	// Most output formats have powers of 2 sizes; prolly faster than integer divisions
	virtual uint32_t samplesFromBytes( uint32_t bytes ) const noexcept
	{
		return bytes / cbSample;
	}
	uint8_t* bufferPointer() noexcept
	{
		return buffer.data();
	}


	// Multiplier to compute output bytes from output samples; equals to channels * byteSize( sampleFormat )
	const uint16_t cbSample;

	std::vector<uint8_t> buffer;
	mutable size_t readOffset = 0;
};