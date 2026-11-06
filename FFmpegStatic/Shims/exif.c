#include <libavcodec/exif.h>

int32_t av_exif_get_tag_id( const char* name )
{
	__debugbreak();
	return -1;
}

int av_exif_get_entry( void* logctx, AVExifMetadata* ifd, uint16_t id, int flags, AVExifEntry** value )
{
	__debugbreak();
	return -1;
}

int av_exif_remove_entry( void* logctx, AVExifMetadata* ifd, uint16_t id, int flags )
{
	__debugbreak();
	return -1;
}

int av_exif_parse_buffer( void* logctx, const uint8_t* data, size_t size,
	AVExifMetadata* ifd, enum AVExifHeaderMode header_mode )
{
	__debugbreak();
	return -1;
}

int av_exif_write( void* logctx, const AVExifMetadata* ifd, AVBufferRef** buffer, enum AVExifHeaderMode header_mode )
{
	__debugbreak();
	return -1;
}

void av_exif_free( AVExifMetadata* ifd ) {}

int av_exif_ifd_to_dict( void* logctx, const AVExifMetadata* ifd, AVDictionary** metadata )
{
	__debugbreak();
	return -1;
}

AVExifMetadata* av_exif_clone_ifd( const AVExifMetadata* ifd )
{
	__debugbreak();
	return NULL;
}

int av_exif_orientation_to_matrix( int32_t* matrix, int orientation )
{
	__debugbreak();
	return -1;
}