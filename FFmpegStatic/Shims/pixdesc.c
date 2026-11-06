#include <stddef.h>
#include <libavutil/pixdesc.h>

const AVPixFmtDescriptor* av_pix_fmt_desc_get( enum AVPixelFormat pix_fmt )
{
	__debugbreak();
	return NULL;
}

int av_pix_fmt_count_planes( enum AVPixelFormat pix_fmt )
{
	__debugbreak();
	return -1;
}

const char* av_color_range_name( enum AVColorRange range )
{
	__debugbreak();
	return NULL;
}

const char* av_color_primaries_name( enum AVColorPrimaries primaries )
{
	__debugbreak();
	return NULL;
}

const char* av_color_transfer_name( enum AVColorTransferCharacteristic transfer )
{
	__debugbreak();
	return NULL;
}

const char* av_color_space_name( enum AVColorSpace space )
{
	__debugbreak();
	return NULL;
}

const char* av_chroma_location_name( enum AVChromaLocation location )
{
	__debugbreak();
	return NULL;
}

int av_get_bits_per_pixel( const AVPixFmtDescriptor* pixdesc )
{
	__debugbreak();
	return 0;
}

enum AVPixelFormat av_get_pix_fmt( const char* name )
{
	__debugbreak();
	return AV_PIX_FMT_NONE;
}

const char* av_get_pix_fmt_name( enum AVPixelFormat pix_fmt )
{
	__debugbreak();
	return "";
}