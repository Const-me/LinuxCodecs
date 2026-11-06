#include <libavcodec/avcodec.h>

#define avcodec_default_get_buffer2 avcodec_default_get_buffer2_OFF
#include <libavcodec/get_buffer.c>
#undef avcodec_default_get_buffer2

int avcodec_default_get_buffer2( AVCodecContext* avctx, AVFrame* frame, int flags )
{
	int ret;
	if( ( ret = update_frame_pool( avctx, frame ) ) < 0 )
		return ret;

	switch( avctx->codec_type ) {
	case AVMEDIA_TYPE_AUDIO:
		return audio_get_buffer( avctx, frame );
	default:
		return -1;
	}
}