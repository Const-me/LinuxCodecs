#include "stdafx.h"
#include "ffIncludes.h"

HRESULT failedStatus( int avCode ) noexcept
{
	if( avCode >= 0 )
	{
		assert( false );
		return E_UNEXPECTED;
	}

	const int status = -avCode;
	constexpr int errnoTsvLast = 133;
	if( status <= errnoTsvLast )
		return hresultFromErrno( status );

	switch( avCode )
	{
	case AVERROR_INVALIDDATA:
		return MF_E_INVALIDMEDIATYPE;
	case AVERROR_EOF:
		return MF_E_END_OF_STREAM;
	case AVERROR_PATCHWELCOME:
		return E_NOTIMPL;
	case AVERROR_BUFFER_TOO_SMALL:
		return NTE_BUFFER_TOO_SMALL;
	}
	// TODO: ideally make a custom facility for these FFmpeg fails
	return E_FAIL;
}