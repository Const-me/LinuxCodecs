#include "stdafx.h"
#include <API/createDecoder.h>
#include "DecoderObj.h"
using AudioCodecs::eAudioCodec;

extern "C" HRESULT __stdcall AudioCodecs::createSampleDecoder(
	const sDecoderInit& decInit, iSampleSource* source, iDecodedStream** result ) noexcept
{
	if( nullptr == source || nullptr == result )
		return E_POINTER;

	CodecContext cc;
	CHECK( cc.initialise( decInit, source ) );

	ComLight::CComPtr<ComLight::Object<DecoderObj>> obj;
	CHECK( ComLight::Object<DecoderObj>::create( obj ) );
	CHECK( obj->initialise( cc, decInit ) );
	obj.detach( result );
	return S_OK;
}