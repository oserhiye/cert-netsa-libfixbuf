#!/bin/bash

if [ "$1" == "clean" ]; then
  rm -f exTool libfixbuf.so result
  rm ../src/infomodel.*
  exit
fi
cd ..
home=`pwd`
cd $home/exTool
# make library first
cd $home/src && ./make-infomodel --package libfixbuf cert ipfix netflowv9
cd $home && gcc -shared -o $home/exTool/libfixbuf.so -fPIC -Iinclude -Isrc -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include src/*.c -pthread -lglib-2.0

# now make export tool
cd $home/exTool
gcc  -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -I$home/include -O0 -g -Wl,-rpath='$ORIGIN' exTool.c -L`pwd` -l:libfixbuf.so -lglib-2.0
mv a.out exTool
chmod +x exTool
