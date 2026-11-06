#include "stdafx.h"
#include "loggerUtils.h"
using namespace AudioCodecs;

const char* cstr( eSampleFormat sampleFormat ) noexcept
{
	switch( sampleFormat )
	{
	case eSampleFormat::Int16: return "int16";
	case eSampleFormat::Fp32: return "fp32";
	}
	return "<error>";
}

const char* cstr( eAudioCodec codec ) noexcept
{
	switch( codec )
	{
	case eAudioCodec::AAC: return "AAC";
	case eAudioCodec::DTS: return "DTS";
	case eAudioCodec::AC3: return "AC3";
	case eAudioCodec::EAC3: return "EAC3";

	case eAudioCodec::Wma1: return "WMA v1";
	case eAudioCodec::Wma2: return "WMA v2";
	case eAudioCodec::WmaVoice: return "WMA Voice";
	case eAudioCodec::WmaLossless: return "WMA Lossless";
	case eAudioCodec::WmaPro: return "WMA Pro";

	}
	return "<error>";
}