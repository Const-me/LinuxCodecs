#include "stdafx.h"
#include "CodecContext.h"
#include <Utils/loggerUtils.h>

void CodecContext::swap( CodecContext& that ) noexcept
{
	std::swap( ctx, that.ctx );
	std::swap( frame, that.frame );
	converter.swap( that.converter );
	source.swap( that.source );
	std::swap( logLevel, that.logLevel );
	std::swap( endOfStream, that.endOfStream );
}

void CodecContext::logMessage( AudioCodecs::eLogLevel lvl, const char8_t* message ) const noexcept
{
	if( nullptr != source )
		source->logMessage( lvl, message );
}

AudioCodecs::eLogLevel CodecContext::maxLogLevel() const noexcept
{
	return logLevel;
}

DecoderInfo CodecContext::getInfoStrings( const AudioCodecs::sDecoderInit& decInit ) const noexcept
{
	DecoderInfo sm;
	sm.codec = cstr( decInit.codec );
	sm.outFormat = cstr( converter->sampleFormat );
	sm.sourceFormat = av_get_sample_fmt_name( converter->sourceFormat );
	sm.outChans = converter->channels;
	sm.sourceChans = converter->sourceChannels;
	return sm;
}