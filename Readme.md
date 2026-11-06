# AudioCodecs

## Introduction

This repository contains 4 projects.

* `MakeCMake` is a simple .NET 10 console application which converts `.vcxproj` files into a source file list for CMake.
* `FFmpegStatic` compiles a subset of [FFmpeg](https://ffmpeg.org/) into a static library.
* `ResamplerStatic` compiles [SoX Resampler library](https://sourceforge.net/projects/soxr/) into a static library.
* `AudioCodecs` links both static libraries into a single DLL.
For FFmpeg, the DLL implements a sane object-oriented API based on IUnknown interfaces.
The resampler’s public API was already good, so the DLL just exports functions implemented in the static library.

In runtime, the DLL requires AMD64 processor which supports AVX2 instruction set and runs a 64-bit OS.

I have only used Windows version during development for debugger and profiler, i.e. it’s tested very little.

I have not modified any of the FFmpeg or libsoxr source codes.
Instead, I have added both as submodules, and wrote cmake scripts to compile the pieces I need, bypassing their OG configure and build scripts.
Some FFmpeg functions are replaced by my own implementations at link time.
See the `*.c` source files in `FFmpegStatic/Patches` and `FFmpegStatic/Shims` for the details.

This project is not affiliated with or endorsed by FFmpeg or the SoX projects.

## Motivation

I needed to decode audio files in a desktop Linux software.
For some of the codecs like Mp3 and Vorbis, there’re multiple good quality implementations easy to integrate.
However, I also wanted to support AAC, DTS, AC3, and WMA.
For them, seems [FFmpeg](https://ffmpeg.org/) is the only good option available.

I didn’t want to parse any containers in C++; this DLL only includes a subset of libavcodec library.

Additionally, the DLL includes SoX Resampler which uses same LGPL 2.1 license as FFMpeg.

# Build Instructions

Clone this repository including submodules.

The rest depends on the OS.

## Linux

### Requirements

- [cmake](https://cmake.org/) 3.24 or newer.
Note old Linuxes (where you should be building this thing due to glibc compatibility shenanigans) don’t have that version in their package repositories.
I recommend installing cmake [from kitware.com](https://apt.kitware.com/).

- A [GCC compiler](https://en.wikipedia.org/wiki/GNU_Compiler_Collection) which supports C++/20 language version.

- [NASM](https://www.nasm.us/); on Ubuntu, the command to install is `sudo apt install nasm`

### Build Instructions

In the command-line shell, navigate to FFmpegStatic subfolder and run `config-linux.sh` script there.

In the command-line shell, navigate to the root of the cloned repository, and run `build.sh` script there.

If compiled successfully, the output file should be `AudioCodecs/x64/linux/bin/AudioCodecs.amd64.so`

## Windows

### Requirements

- Visual Studio 2026 or newer

- [MSYS2](https://www.msys2.org/docs/installer/)

- [NASM](https://www.nasm.us/), and its visual studio [integration](https://github.com/ShiftMediaProject/VSNASM).

### Build Instructions

Navigate to FFmpegStatic subfolder. Edit `config-windows.cmd` shell script. Find the following line:

```
set MSYS=C:\Soft\Utils\MSYS2  
```

Replace the value with your path to MSYS2

Run the `config-windows.cmd` script.

Edit `NasmConfig.props` in the same folder; inside the `<NasmPath>` element, specify path to NASM installation folder.

Double click `AudioCodecs.slnx` and build AudioCodecs project.

If compiled successfully, the output file should be `x64/$(Configuration)/AudioCodecs.amd64.dll`