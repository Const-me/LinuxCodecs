#include <libavutil/common.h>
#include <libavutil/parseutils.h>

int av_parse_video_size( int* width_ptr, int* height_ptr, const char* str )
{
	return AVERROR( EINVAL );
}

int av_parse_video_rate( AVRational* rate, const char* arg )
{
	return AVERROR( EINVAL );
}

int av_parse_color( uint8_t* rgba_color, const char* color_string, int slen, void* log_ctx )
{
	return AVERROR( EINVAL );
}

int av_parse_time( int64_t* timeval, const char* timestr, int duration )
{
	return AVERROR( EINVAL );
}