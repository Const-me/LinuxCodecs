#!/bin/sh
mkdir -p config/windows
cd config/windows
../../FFmpeg/configure \
  --toolchain=msvc --arch=x86_64 \
  --disable-everything \
  --enable-decoder=aac,ac3,dca,eac3,wmav1,wmav2,wmavoice,wmalossless,wmapro \
  --disable-autodetect --disable-programs --disable-doc \
  --disable-avformat --disable-avfilter --disable-avdevice --disable-swscale \
  --disable-network --disable-debug --disable-stripping \
  --disable-pthreads --disable-w32threads --disable-os2threads \
  --disable-runtime-cpudetect --cpu=haswell