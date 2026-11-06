#include <libavutil/eval.h>

// The OG thing seems designed for optimal RCE: parses untrusted strings into function pointers, then calls the pointers
int av_expr_parse_and_eval( double* d, const char* s,
	const char* const* const_names, const double* const_values,
	const char* const* func1_names, double ( * const* funcs1 )( void*, double ),
	const char* const* func2_names, double ( * const* funcs2 )( void*, double, double ),
	void* opaque, int log_offset, void* log_ctx )
{
	return -1;
}