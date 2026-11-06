cmake_minimum_required(VERSION 3.24)
project( AudioCodecs LANGUAGES CXX )

# Language
set( CMAKE_CXX_STANDARD 20 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS OFF )

# Position-independent code
set( CMAKE_POSITION_INDEPENDENT_CODE ON )

# Whole-program optimisation (LTO) for Release, analogous to /GL + /LTCG
set( CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON )

# Include the auto-generated list of source files
include( ${CMAKE_CURRENT_SOURCE_DIR}/AudioCodecs.cmake )

add_library( AudioCodecs SHARED ${AUDIOCODECS_SOURCES} )

target_include_directories( AudioCodecs PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/../FFmpegStatic/config/linux/
	${CMAKE_CURRENT_SOURCE_DIR}/../FFmpegStatic/FFmpeg/
)

set_target_properties( AudioCodecs PROPERTIES
	LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/x64/linux/bin"
	VISIBILITY_INLINES_HIDDEN ON
	# Relink when the version script changes
	LINK_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/AudioCodecs.exports"
	# AudioCodecs.amd64.so output name to match the convention
	OUTPUT_NAME "AudioCodecs.amd64"
	PREFIX "" )

# x86-64-v3 (AVX2, BMI2, FMA, etc.), no fallbacks, for both C and C++
target_compile_options( AudioCodecs PRIVATE -march=x86-64-v3 )

# Suppress the warning: ignoring attributes on template argument ‘__m256’ [-Wignored-attributes] std::array<__m256, len>
target_compile_options( AudioCodecs PRIVATE $<$<COMPILE_LANG_AND_ID:CXX,GNU>:-Wno-ignored-attributes> )

# Precompiled header for C++ sources only
target_precompile_headers( AudioCodecs PRIVATE
    "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/stdafx.h>"
)

# Static C++ runtime, version script, and no re-export of symbols from static libs
target_link_options( AudioCodecs PRIVATE
	-static-libstdc++
	-static-libgcc
	LINKER:--version-script=${CMAKE_CURRENT_SOURCE_DIR}/AudioCodecs.exports
	LINKER:--no-undefined
	LINKER:--no-undefined-version
)

# Link the dependent static libraries; see CMakeLists.txt in the parent folder for the integration.
target_link_libraries( AudioCodecs PRIVATE FFmpegStatic )
target_link_libraries( AudioCodecs PRIVATE $<LINK_LIBRARY:WHOLE_ARCHIVE,ResamplerStatic> )