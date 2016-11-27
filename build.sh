#!/bin/bash
OUTDIR="./build"

STARTTIME=$(date +%s)
COMPILEPARAM=-ggdb

#clang lib/parson.c -c -fpic $COMPILEPARAM

echo "Compiling source code..."

clang $COMPILEPARAM  -c -fpic -std=c99 \
texture.c

clang $COMPILEPARAM -c -fpic -std=c++11 \
-I/usr/include/freetype2 \
 texture.cpp \
 camera.cpp \
 ttmath.cpp \
 mesh.cpp \
 transform.cpp \
 material.cpp \
 terrain.cpp \
 audio.cpp \
 debug.cpp \
 memory.cpp \
 input.cpp \
 core.cpp \
 modelParser.cpp

CURTIME=$(date +%s)
echo "Linking shared library... (compiling took $(($CURTIME - $STARTTIME))s)"
STARTTIME=$(date +%s)

mv *.o $OUTDIR
cwd=$(pwd)
cd $OUTDIR
clang $COMPILEPARAM -shared -o libgame.so camera.o ttmath.o mesh.o transform.o material.o terrain.o texture.o audio.o debug.o memory.o input.o core.o modelParser.o parson.o \
-lopenal \
-lfreetype \
-lalut \
-lGL \
-lGLEW \
-lstdc++

cd $cwd
#cp libgame.so \usr\lib\

CURTIME=$(date +%s)
echo "Creating executable... (linking shared library took $(($CURTIME - $STARTTIME))s)"
STARTTIME=$(date +%s)

clang $COMPILEPARAM -fpic \
platform_linux.cpp input.cpp memory.cpp ttmath.cpp -std=c++11 -o game.out \
-lGL \
-lGLEW \
-lX11 \
-ldl \
-lm

mv -v *.out $OUTDIR

CURTIME=$(date +%s)
echo "Done (creating executable took $(($CURTIME - $STARTTIME))s)"
