#pragma once
#include <memory>
#include "PcmConverter.h"

// Copy single input channel into the only channel of the output
template<bool makeInt16 = false>
class FloatPassthrough final: public PcmConverter
{
	const uint8_t chanIndex;
public:
	FloatPassthrough( uint8_t chanIn, uint8_t sourceChanIdx = 0 ) noexcept;
	~FloatPassthrough() noexcept = default;

protected:
	uint32_t samplesFromBytes( uint32_t bytes ) const noexcept final;
	HRESULT convertImpl( const AVFrame& frame ) noexcept final;
};

// Interleaves arbitrary count of input channels, very inefficiently.
class FloatInterleave final: public PcmConverter
{
public:
	FloatInterleave( uint8_t count ) noexcept;
	~FloatInterleave() noexcept = default;

protected:
	HRESULT convertImpl( const AVFrame& frame ) noexcept final;
};

class ChannelLayout;

HRESULT createPassthrough( const ChannelLayout& layout, std::unique_ptr<PcmConverter>& result, bool makeInt16 = false );