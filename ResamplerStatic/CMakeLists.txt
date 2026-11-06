cmake_minimum_required(VERSION 3.22)
project( ResamplerStatic LANGUAGES C )

# C language
set( CMAKE_C_STANDARD 17 )
set( CMAKE_C_STANDARD_REQUIRED ON )
set( CMAKE_C_EXTENSIONS OFF )

# Position-independent code
set( CMAKE_POSITION_INDEPENDENT_CODE ON )

# Whole-program optimisation (LTO) for Release, analogous to /GL + /LTCG
set( CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON )

# Include auto-generated list of source files
include( ${CMAKE_CURRENT_SOURCE_DIR}/ResamplerStatic.cmake )

add_library( ResamplerStatic STATIC ${RESAMPLERSTATIC_SOURCES} )

target_include_directories( ResamplerStatic PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
)

set_target_properties( ResamplerStatic PROPERTIES
	VISIBILITY_INLINES_HIDDEN ON )

target_compile_definitions( ResamplerStatic PRIVATE SOXR_LIB )

target_compile_options( ResamplerStatic PRIVATE -march=x86-64-v3 )