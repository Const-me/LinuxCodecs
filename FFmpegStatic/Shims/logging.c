#include <libavutil/log.h>

/*
// These two have been moved into the main DLL project, messages forwarded to iSampleSource callback interface
void av_log( void* avcl, int level, const char* fmt, ... )
{
}
int av_log_get_level( void )
{
	return 0;
}
*/

const char* av_default_item_name( void* ptr )
{
	return ( *(AVClass**)ptr )->class_name;
}

void avpriv_report_missing_feature( void* avc, const char* msg, ... )
{
}

void avpriv_request_sample( void* avc, const char* msg, ... )
{
}