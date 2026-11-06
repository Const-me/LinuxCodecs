#include <libavutil/imgutils.h>

void av_image_copy( uint8_t* const dst_data[ 4 ], const int dst_linesizes[ 4 ],
	const uint8_t* const src_data[ 4 ], const int src_linesizes[ 4 ],
	enum AVPixelFormat pix_fmt, int width, int height )
{
	__debugbreak();
}

int av_image_check_size( unsigned int w, unsigned int h, int log_offset, void* log_ctx )
{
	__debugbreak();
	return -1;
}

int av_image_check_size2( unsigned int w, unsigned int h, int64_t max_pixels, enum AVPixelFormat pix_fmt, int log_offset, void* log_ctx )
{
	__debugbreak();
	return -1;
}

int av_image_check_sar( unsigned int w, unsigned int h, AVRational sar )
{
	__debugbreak();
	return -1;
}

int av_image_fill_pointers( uint8_t* data[ 4 ], enum AVPixelFormat pix_fmt, int height,
	uint8_t* ptr, const int linesizes[ 4 ] )
{
	__debugbreak();
	return -1;
}

int av_image_fill_plane_sizes( size_t size[ 4 ], enum AVPixelFormat pix_fmt,
	int height, const ptrdiff_t linesizes[ 4 ] )
{
	__debugbreak();
	return -1;
}

int av_image_fill_linesizes( int linesizes[ 4 ], enum AVPixelFormat pix_fmt, int width )
{
	__debugbreak();
	return -1;
}