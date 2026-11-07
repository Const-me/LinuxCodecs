#!/bin/sh
mkdir -p config/linux
cd config/linux
../../FFmpeg/configure --disable-everything \
  --enable-decoder=aac,ac3,dca,eac3,wmav1,wmav2,wmavoice,wmalossless,wmapro,opus \
  --disable-autodetect --disable-programs --disable-doc \
  --disable-avformat --disable-avfilter --disable-avdevice --disable-swscale \
  --disable-network --disable-debug --disable-stripping \
  --disable-pthreads --disable-w32threads --disable-os2threads \
  --disable-runtime-cpudetect --cpu=haswell \
  --extra-cflags=-march=x86-64-v3