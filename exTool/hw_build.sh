#!/bin/bash

tool=$1
if [ "$tool" == "clean" ]; then
  rm -f exTool libfixbuf.so result
  rm ../src/infomodel.*
  exit
fi
if [ "s$tool" == "s" ]; then
  echo "use like ./hw_build.sh <path to crosscompilation toolchain>"
  echo "toolchain expected to have"
  echo " - bin with arm-linux-gcc"
  echo " - arm-sm-linux-gnueabi/sysroot with glib-2.0 present"
  exit
fi
cd ..
home=`pwd`
cd $home/exTool
# make library first
cd $home/src && ./make-infomodel --package libfixbuf cert ipfix netflowv9
PATH=${tool}/bin/:${tool}/arm-sm-linux-gnueabi/sysroot/usr/include:${PATH}
cd $home && arm-linux-gcc --sysroot=${tool}/arm-sm-linux-gnueabi/sysroot -shared -o $home/exTool/libfixbuf.so -fPIC -Iinclude -Isrc -I${tool}/arm-sm-linux-gnueabi/sysroot/usr/include/glib-2.0 -I${tool}/arm-sm-linux-gnueabi/sysroot/usr/lib/glib-2.0/include src/*.c -pthread -lglib-2.0

# now make export tool
cd $home/exTool
arm-linux-gcc --sysroot=${tool}/arm-sm-linux-gnueabi/sysroot -I${tool}/arm-sm-linux-gnueabi/sysroot/usr/include/glib-2.0 -I${tool}/arm-sm-linux-gnueabi/sysroot/usr/lib/glib-2.0/include -I$home/include -O0 -g -Wl,-rpath='$ORIGIN' exTool.c -L`pwd` -l:libfixbuf.so -lglib-2.0
mv a.out exTool
chmod +x exTool


