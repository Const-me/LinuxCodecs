#pragma once
#include <API/createDecoder.h>
#include "ffIncludes.h"

HRESULT applyConfig( AVCodecContext* ctx, const AudioCodecs::sDecoderInit& decInit ) noexcept;