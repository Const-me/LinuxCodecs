#include <libavcodec/avcodec.h>

int ff_thread_init( AVCodecContext* s )
{
	return AVERROR( ENOSYS );
}

void ff_thread_free( AVCodecContext* s )
{
}

void ff_thread_flush( AVCodecContext* s )
{
}