#pragma once

extern "C"
{
#include <libavcodec/avcodec.h>
}

// Translate failed negative status code into HRESULT
HRESULT failedStatus( int avCode ) noexcept;

inline constexpr HRESULT hresultFromErrno( int code )
{
	assert( code < 0x10000 );
	constexpr int facility = 'E';
	// Severity = 1, Reserved = 0, Customer = 1, NTSTATUS = 0
	constexpr int failMask = ( facility << 16 ) | (int)( 0xA0000000u );
	return failMask | (uint16_t)code;
}

// Translate failed negative status code into HRESULT return value from the functions
#define AV_CHECK( status ) do { const int ffmpg_status = ( status ); if( ffmpg_status < 0 ) return failedStatus( ffmpg_status ); } while( false )