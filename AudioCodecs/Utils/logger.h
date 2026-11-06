#pragma once
#include <API/iSampleDecoder.h>
#include <cstdarg>

bool willLogMessage( AudioCodecs::eLogLevel level );
void logMessage( AudioCodecs::eLogLevel level, const char8_t* message );
void logMessageFormat( AudioCodecs::eLogLevel level, const char8_t* pszFormat, std::va_list argList );

inline void logError( const char8_t* message )
{
	logMessage( AudioCodecs::eLogLevel::Error, message );
}

inline void logWarning( const char8_t* message )
{
	logMessage( AudioCodecs::eLogLevel::Warning, message );
}

inline void printError( const char8_t* pszFormat, ... )
{
	va_list args;
	va_start( args, pszFormat );
	logMessageFormat( AudioCodecs::eLogLevel::Error, pszFormat, args );
	va_end( args );
}

inline void printWarning( const char8_t* pszFormat, ... )
{
	va_list args;
	va_start( args, pszFormat );
	logMessageFormat( AudioCodecs::eLogLevel::Warning, pszFormat, args );
	va_end( args );
}

inline void printDebug( const char8_t* pszFormat, ... )
{
	va_list args;
	va_start( args, pszFormat );
	logMessageFormat( AudioCodecs::eLogLevel::Debug, pszFormat, args );
	va_end( args );
}