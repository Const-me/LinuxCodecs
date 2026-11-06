cmake_minimum_required(VERSION 3.22)
project( FFmpegStatic LANGUAGES C )

# C language
set( CMAKE_C_STANDARD 17 )
set( CMAKE_C_STANDARD_REQUIRED ON )
set( CMAKE_C_EXTENSIONS OFF )
# Assembly language
enable_language( ASM_NASM )
set( CMAKE_ASM_NASM_OBJECT_FORMAT elf64 )

# Position-independent code
set( CMAKE_POSITION_INDEPENDENT_CODE ON )

# Whole-program optimisation (LTO) for Release, analogous to /GL + /LTCG
set( CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON )

# Include auto-generated lists of source files
include( ${CMAKE_CURRENT_SOURCE_DIR}/FFmpegStatic.cmake )
include( ${CMAKE_CURRENT_SOURCE_DIR}/FFmpegStatic.nasm.cmake )

add_library( FFmpegStatic STATIC ${FFMPEGSTATIC_SOURCES} ${FFMPEGSTATIC_NASM} )

target_include_directories( FFmpegStatic PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/config/linux/
	${CMAKE_CURRENT_SOURCE_DIR}/FFmpeg/
)

set_target_properties( FFmpegStatic PROPERTIES
	VISIBILITY_INLINES_HIDDEN ON )

target_compile_options( FFmpegStatic PRIVATE $<$<COMPILE_LANGUAGE:C>:-march=x86-64-v3> )

# Disable all compiler warnings :-(
target_compile_options( FFmpegStatic PRIVATE $<$<COMPILE_LANGUAGE:C>:-w> )
target_compile_definitions( FFmpegStatic PRIVATE $<$<COMPILE_LANGUAGE:C>:HAVE_AV_CONFIG_H> )

target_compile_options( FFmpegStatic PRIVATE
	$<$<COMPILE_LANGUAGE:ASM_NASM>:-I${CMAKE_CURRENT_SOURCE_DIR}/FFmpeg>
	$<$<COMPILE_LANGUAGE:ASM_NASM>:-I${CMAKE_CURRENT_SOURCE_DIR}/FFmpeg/libavutil/x86>
	$<$<COMPILE_LANGUAGE:ASM_NASM>:-P${CMAKE_CURRENT_SOURCE_DIR}/config/linux/config.asm>
	$<$<COMPILE_LANGUAGE:ASM_NASM>:-DPIC>
	$<$<COMPILE_LANGUAGE:ASM_NASM>:-Ox>
)