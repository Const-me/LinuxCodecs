#pragma once

namespace AudioCodecs
{
	// Audio codec implemented in this DLL
	enum struct eAudioCodec: uint8_t
	{
		AAC,
		DTS,
		AC3,
		EAC3,
		Wma1,
		Wma2,
		WmaVoice,
		WmaLossless,
		WmaPro,
		Opus,
	};
}