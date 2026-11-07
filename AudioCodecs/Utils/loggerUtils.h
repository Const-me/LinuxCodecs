#pragma once
#include <API/sPcmDesc.h>
#include <API/eAudioCodec.h>

// Human-readable name of the output sample format
const char* cstr( AudioCodecs::eSampleFormat sampleFormat ) noexcept;

// Human-readable name of the audio codec
const char* cstr( AudioCodecs::eAudioCodec codec ) noexcept;
// FFmpeg name of the audio codec
const char* decoderName( AudioCodecs::eAudioCodec codec ) noexcept;