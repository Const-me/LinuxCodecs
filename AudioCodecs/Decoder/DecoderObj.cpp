#include "stdafx.h"
#include "DecoderObj.h"
#include <Utils/logger.h>
#include <cmath>

HRESULT COMLIGHTCALL DecoderObj::getOutputDesc( AudioCodecs::sPcmDesc& desc ) const noexcept
{
	if( m_desc.frequency != 0 && m_desc.channels != 0 )
	{
		desc = m_desc;
		return S_OK;
	}
	return OLE_E_BLANK;
}

static inline const char* pluralSuffix( int i )
{
	return ( i != 1 ) ? "s" : "";
}

HRESULT DecoderObj::initialise( CodecContext& ctx, const AudioCodecs::sDecoderInit& decInit ) noexcept
{
	CHECK( ctx.makeOutputDesc( m_desc ) );
	ctx.swap( context );

	LoggerSetupRaii logSetup{ &context };
	if( !willLogMessage( AudioCodecs::eLogLevel::Debug ) )
		return S_OK;

	const DecoderInfo info = context.getInfoStrings( decInit );
	double kHz = std::round( m_desc.frequency * 1E-2 ) / 10.0;
	printDebug( u8"Created %s decoder: %g kHz sample rate, %i %s output channel%s, %i %s decoded channel%s",
		info.codec, kHz,
		info.outChans, info.outFormat, pluralSuffix( info.outChans ),
		info.sourceChans, info.sourceFormat, pluralSuffix( info.sourceChans ) );
	return S_OK;
}

HRESULT COMLIGHTCALL DecoderObj::readSamples( uint8_t* rdi, int requested, int& delivered ) noexcept
{
	LoggerSetupRaii loggerSetup{ &context };
	return context.readSamples( rdi, requested, delivered );
}