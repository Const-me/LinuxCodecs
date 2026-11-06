#include <libavutil/hwcontext.h>

const char* av_hwdevice_get_type_name( enum AVHWDeviceType type )
{
	__debugbreak();
	return NULL;
}

AVBufferRef* av_hwframe_ctx_alloc( AVBufferRef* device_ref_in )
{
	__debugbreak();
	return NULL;
}

int av_hwframe_ctx_init( AVBufferRef* ref )
{
	__debugbreak();
	return -1;
}

int av_hwframe_transfer_data( AVFrame* dst, const AVFrame* src, int flags )
{
	__debugbreak();
	return -1;
}

int av_hwframe_get_buffer( AVBufferRef* hwframe_ref, AVFrame* frame, int flags )
{
	__debugbreak();
	return -1;
}