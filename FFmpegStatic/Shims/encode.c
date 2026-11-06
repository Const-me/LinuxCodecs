#include <libavcodec/avcodec.h>
#include <libavcodec/avcodec_internal.h>

int avcodec_default_get_encode_buffer( AVCodecContext* s, AVPacket* pkt, int flags )
{
	return AVERROR( EINVAL );
}

int ff_encode_preinit( struct AVCodecContext* avctx )
{
	__debugbreak();
	return -1;
}

void ff_encode_flush_buffers( AVCodecContext* avctx )
{
	__debugbreak();
}

struct AVCodecInternal* ff_encode_internal_alloc( void )
{
	__debugbreak();
	return NULL;
}

int ff_encode_receive_frame( struct AVCodecContext* avctx, struct AVFrame* frame )
{
	__debugbreak();
	return -1;
}