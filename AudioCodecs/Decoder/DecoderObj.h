#pragma once
#include <API/iSampleDecoder.h>
#include "../../../CGML/ComLightLib/comLightServer.h"
#include "CodecContext.h"

class DecoderObj: public ComLight::ObjectRoot<AudioCodecs::iDecodedStream>
{
	CodecContext context;
	AudioCodecs::sPcmDesc m_desc;
	HRESULT COMLIGHTCALL getOutputDesc( AudioCodecs::sPcmDesc& desc ) const noexcept final;
	HRESULT COMLIGHTCALL readSamples( uint8_t* rdi, int requested, int& delivered ) noexcept final;

public:
	DecoderObj() = default;
	~DecoderObj() = default;
	HRESULT initialise( CodecContext& ctx, const AudioCodecs::sDecoderInit& decInit ) noexcept;
};