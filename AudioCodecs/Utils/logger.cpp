#include "stdafx.h"
#include <cstdio>
#include "logger.h"
#include "iLoggerSetup.h"

namespace
{
	thread_local iLoggerSetup* ts_setup = nullptr;
	using AudioCodecs::eLogLevel;
	constexpr size_t ccMaxMessage = 0x200;

	inline bool willLog( iLoggerSetup* obj, eLogLevel level )
	{
		const AudioCodecs::eLogLevel lvl = ts_setup->maxLogLevel();
		return (uint8_t)level <= (uint8_t)lvl;
	}
}

bool willLogMessage( eLogLevel level )
{
	iLoggerSetup* const obj = ts_setup;
	return nullptr != obj && willLog( obj, level );
}

void logMessage( eLogLevel level, const char8_t* message )
{
	iLoggerSetup* const obj = ts_setup;
	if( nullptr == obj || !willLog( obj, level ) )
		return;

	obj->logMessage( level, message );
}

void logMessageFormat( eLogLevel level, const char8_t* pszFormat, std::va_list argList )
{
	iLoggerSetup* const obj = ts_setup;
	if( nullptr == obj || !willLog( obj, level ) )
		return;

	std::array<char, ccMaxMessage> buffer;
	const ptrdiff_t written = std::vsnprintf( buffer.data(), buffer.size(), (const char*)pszFormat, argList );
	if( written <= 0 )
		return;	// Formatting failed or empty output

	buffer[ ccMaxMessage - 1 ] = '\0';
	obj->logMessage( level, (const char8_t*)buffer.data() );
}

LoggerSetupRaii::LoggerSetupRaii( iLoggerSetup* setup ) noexcept
{
	assert( ts_setup == nullptr );
	ts_setup = setup;
}

LoggerSetupRaii::~LoggerSetupRaii() noexcept
{
	assert( ts_setup != nullptr );
	ts_setup = nullptr;
}

extern "C"
{
#include <libavutil/log.h>

	void av_log( void* avcl, int level, const char* fmt, ... )
	{
		iLoggerSetup* const obj = ts_setup;
		if( nullptr == obj || level <= AV_LOG_QUIET )
			return;

		eLogLevel lvl = eLogLevel::Debug;
		if( level <= AV_LOG_ERROR )
			lvl = eLogLevel::Error;
		else if( level <= AV_LOG_WARNING )
			lvl = eLogLevel::Warning;
		else if( level <= AV_LOG_INFO )
			lvl = eLogLevel::Info;
		else if( level <= AV_LOG_VERBOSE )
			lvl = eLogLevel::Debug;
		else
			return;	// DEBUG and TRACE are never delivered

		if( lvl > obj->maxLogLevel() )
			return;

		std::array<char, ccMaxMessage> buffer;
		std::va_list args;
		va_start( args, fmt );
		const ptrdiff_t written = std::vsnprintf( buffer.data(), buffer.size(), fmt, args );
		va_end( args );
		if( written <= 0 )
			return;	// Formatting failed or empty output

		// Make sure the string is null-terminated; vsnprintf supposed to do internally but I don't trust it.
		buffer[ buffer.size() - 1 ] = 0;

		// Trim trailing whitespaces
		for( ptrdiff_t end = std::min( written - 1, (ptrdiff_t)ccMaxMessage - 1 ); end >= 0; end-- )
		{
			const char c = buffer[ end ];
			if( c == '\r' || c == '\n' || c == ' ' || c == '\t' )
				buffer[ end ] = '\0';
			else
				break;
		}

		if( '\0' == buffer[ 0 ] )
			return;
		obj->logMessage( lvl, (const char8_t*)buffer.data() );
	}

	int av_log_get_level()
	{
		iLoggerSetup* const obj = ts_setup;
		if( nullptr == obj )
			return AV_LOG_QUIET;

		switch( obj->maxLogLevel() )
		{
		case eLogLevel::Error:
			return AV_LOG_ERROR;
		case eLogLevel::Warning:
			return AV_LOG_WARNING;
		case eLogLevel::Info:
			return AV_LOG_INFO;
		case eLogLevel::Debug:
			return AV_LOG_VERBOSE;
		}
		return AV_LOG_QUIET;
	}
}