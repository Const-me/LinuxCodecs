#include "stdafx.h"
#include "ChannelLayout.h"
#include <Utils/logger.h>
#include <cmath>

ChannelLayout::ChannelLayout() noexcept
{
	std::fill( ids.begin(), ids.end(), AV_CHAN_NONE );
	LF = RF = LS = RS = C = -1;
	bitmap = chans = 0;
}

HRESULT ChannelLayout::initialise( const AVChannelLayout& avLayout ) noexcept
{
	const int chansInt = avLayout.nb_channels;
	if( chansInt < 1 || chansInt > 8 )
	{
		printError( u8"Unexpected count of channels %i", chansInt );
		return E_INVALIDARG;
	}

	chans = (uint8_t)chansInt;
	unspecStereo = ( 2 == chans ) && ( avLayout.order == AV_CHANNEL_ORDER_UNSPEC );

	resolveLayout( avLayout );
	findPlanes();
	bitmap = computeBitmap();
	return detectLayout();
}

void ChannelLayout::resolveLayout( const AVChannelLayout& layout ) noexcept
{
	for( uint8_t i = 0; i < chans; i++ )
		ids[ i ] = av_channel_layout_channel_from_index( &layout, i );
}

// Plane index of the first channel id equal to a or b, or -1
int8_t ChannelLayout::findPlane( AVChannel a, AVChannel b ) const noexcept
{
	for( uint8_t i = 0; i < chans; i++ )
		if( ids[ i ] == a || ( b != AV_CHAN_NONE && ids[ i ] == b ) )
			return i;
	return -1;
}

void ChannelLayout::findPlanes() noexcept
{
	LF = findPlane( AV_CHAN_FRONT_LEFT );
	RF = findPlane( AV_CHAN_FRONT_RIGHT );
	LS = findPlane( AV_CHAN_SIDE_LEFT, AV_CHAN_BACK_LEFT );
	RS = findPlane( AV_CHAN_SIDE_RIGHT, AV_CHAN_BACK_RIGHT );
	C = findPlane( AV_CHAN_FRONT_CENTER );
}

uint8_t ChannelLayout::computeBitmap() const noexcept
{
	uint8_t r = 0;
	r |= ( LF >= 0 ) ? 1 : 0;
	r |= ( RF >= 0 ) ? 2 : 0;
	r |= ( LS >= 0 ) ? 4 : 0;
	r |= ( RS >= 0 ) ? 8 : 0;
	r |= ( C >= 0 ) ? 0x10 : 0;
	return r;
}

HRESULT ChannelLayout::detectLayout() noexcept
{
	if( 0 == chans )
		return OLE_E_BLANK;
	if( 1 == chans )
	{
		layout = eChannelLayout::Mono;
		return S_OK;
	}
	if( unspecStereo )
	{
		layout = eChannelLayout::Stereo;
		return S_OK;
	}

	if( 3 != ( bitmap & 3 ) )
	{
		logError( u8"Unsupported channel layout for the mono downmix, no front left/right pair" );
		return E_INVALIDARG;
	}

	switch( bitmap )
	{
	case 0x3:
		layout = eChannelLayout::Stereo;
		return S_OK;
	case 0x0F:
		layout = eChannelLayout::Quad;
		return S_OK;
	case 0x13:
		layout = eChannelLayout::StereoCentre;
		return S_OK;
	case 0x1F:
		layout = eChannelLayout::QuadCentre;
		return S_OK;
	}

	// Something's incomplete, best effort below
	if( 0x13 == ( bitmap & 0x13 ) )
	{
		layout = eChannelLayout::StereoCentre;
		return S_OK;
	}
	layout = eChannelLayout::Stereo;
	return S_OK;
}

namespace
{
	const std::array<uint8_t, 6> chanCount
	{
		0, 1, 2, 3, 4, 5
	};

	struct WeightsTemp
	{
		double front, surround, centre;

		void storeScaled( sDownmixWeights& rdi, bool stereo ) const noexcept
		{
			const double mul = stereo ? 1 : 2;
			double sum = centre;
			sum = std::fma( surround, mul, sum );
			sum = std::fma( front, mul, sum );

			rdi.front = (float)( front / sum );
			rdi.surround = (float)( surround / sum );
			rdi.centre = (float)( centre / sum );
		}
	};
}

uint8_t ChannelLayout::usableChannels() const noexcept
{
	return chanCount[ (uint8_t)layout ];
}

HRESULT ChannelLayout::downmixWeights( sDownmixWeights& rdi, bool stereo ) const noexcept
{
	rdi = {};
	// https://www.itu.int/dms_pubrec/itu-r/rec/bs/R-REC-BS.775-4-202212-I!!PDF-E.pdf
	// Annex 4 "Downward mixing of multichannel audio signals" on page 8
	WeightsTemp temp;
	if( stereo )
	{
		temp.front = 1;
		temp.centre = 0.7071;
		temp.surround = 0.7071;
	}
	else
	{
		temp.front = 0.7071;
		temp.centre = 1;
		temp.surround = 0.5;
	}

	switch( layout )
	{
	case eChannelLayout::None:
		return OLE_E_BLANK;
	case eChannelLayout::Mono:
		rdi.front = 1;
		return S_OK;
	case eChannelLayout::Stereo:
		rdi.front = stereo ? 1.0f : 0.5f;
		return S_OK;
	case eChannelLayout::StereoCentre:
		temp.surround = 0;
		break;
	case eChannelLayout::Quad:
		temp.centre = 0;
		break;
	case eChannelLayout::QuadCentre:
		break;
	default:
		return E_UNEXPECTED;
	}

	temp.storeScaled( rdi, stereo );
	return S_OK;
}

HRESULT ChannelLayout::makeChannelMap( uint8_t* rdi, size_t length ) const noexcept
{
	if( layout == eChannelLayout::None )
		return OLE_E_BLANK;
	if( length != usableChannels() )
		return E_INVALIDARG;
	if( nullptr == rdi )
		return E_POINTER;

	switch( layout )
	{
	case eChannelLayout::Mono:
		rdi[ 0 ] = 0;
		return S_OK;

	case eChannelLayout::Stereo:
		if( unspecStereo )
		{
			rdi[ 0 ] = 0;
			rdi[ 1 ] = 1;
		}
		else
		{
			rdi[ 0 ] = (uint8_t)LF;
			rdi[ 1 ] = (uint8_t)RF;
		}
		return S_OK;

	case eChannelLayout::StereoCentre:
		rdi[ 0 ] = (uint8_t)LF;
		rdi[ 1 ] = (uint8_t)RF;
		rdi[ 2 ] = (uint8_t)C;
		return S_OK;

	case eChannelLayout::Quad:
		rdi[ 0 ] = (uint8_t)LF;
		rdi[ 1 ] = (uint8_t)RF;
		rdi[ 2 ] = (uint8_t)LS;
		rdi[ 3 ] = (uint8_t)RS;
		return S_OK;

	case eChannelLayout::QuadCentre:
		rdi[ 0 ] = (uint8_t)LF;
		rdi[ 1 ] = (uint8_t)RF;
		rdi[ 2 ] = (uint8_t)LS;
		rdi[ 3 ] = (uint8_t)RS;
		rdi[ 4 ] = (uint8_t)C;
		return S_OK;

	default:
		return E_UNEXPECTED;
	}
}