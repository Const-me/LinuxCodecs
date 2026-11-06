#pragma once
#include <API/sPcmDesc.h>
#include <API/eAudioCodec.h>

const char* cstr( AudioCodecs::eSampleFormat sampleFormat ) noexcept;
const char* cstr( AudioCodecs::eAudioCodec codec ) noexcept;