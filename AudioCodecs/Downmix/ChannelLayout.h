#pragma once
#include <array>
#include <Decoder/ffIncludes.h>
#include "sDownmixWeights.h"

enum struct eChannelLayout
{
	None = 0,
	Mono,
	// The selector should select LF, RF
	Stereo,
	// The selector should select LF, RF, C
	StereoCentre,
	// The selector should select LF, RF, LS, RS
	Quad,
	// The selector should select LF, RF, LS, RS, C
	QuadCentre,
};

class ChannelLayout
{
	int8_t LF, RF, LS, RS, C;
	uint8_t bitmap = 0;
	uint8_t chans = 0;
	bool unspecStereo = false;
	eChannelLayout layout = eChannelLayout::None;
	std::array<AVChannel, 8> ids;

	void resolveLayout( const AVChannelLayout& layout ) noexcept;
	int8_t findPlane( AVChannel a, AVChannel b = AV_CHAN_NONE ) const noexcept;
	void findPlanes() noexcept;
	uint8_t computeBitmap() const noexcept;
	HRESULT detectLayout() noexcept;

public:
	ChannelLayout() noexcept;

	HRESULT initialise( const AVChannelLayout& layout ) noexcept;

	uint8_t sourceChannels() const noexcept
	{
		return chans;
	}

	eChannelLayout getLayout() const noexcept
	{
		return layout;
	}

	uint8_t usableChannels() const noexcept;
	HRESULT downmixWeights( sDownmixWeights& rdi, bool stereo ) const noexcept;
	HRESULT makeChannelMap( uint8_t* rdi, size_t length ) const noexcept;
};