#!/bin/bash
OUTDIR="./build"

STARTTIME=$(date +%s)
COMPILEPARAM="-ggdb -O0"
GAMELIBS="-lopenal -lfreetype -lalut -lGL -lGLEW"
EXELIBS="-lGL -lGLEW -lX11 -ldl -lm -lpthread"

#clang lib/parson.c -c -fpic $COMPILEPARAM

echo "Compiling source code..."
clang $COMPILEPARAM -c -fpic -std=gnu99 \
-I/usr/include/freetype2 \
 camera.c \
 ttmath.c \
 mesh.c \
 transform.c \
 material.c \
 terrain.c \
 texture.c \
 audio.c \
 debug.c \
 memory.c \
 input.c \
 core.c \
 modelParser.c \
 opengl.c \
 voxel_terrain.c

CURTIME=$(date +%s)
echo "Linking shared library... (compiling took $(($CURTIME - $STARTTIME))s)"
STARTTIME=$(date +%s)

mv *.o $OUTDIR
cwd=$(pwd)
cd $OUTDIR
clang $COMPILEPARAM -shared -std=gnu99 -o libgame.so camera.o ttmath.o mesh.o transform.o material.o terrain.o texture.o audio.o debug.o memory.o input.o core.o modelParser.o opengl.o voxel_terrain.o \
$GAMELIBS

cd $cwd
#cp libgame.so \usr\lib\

CURTIME=$(date +%s)
echo "Creating executable... (linking shared library took $(($CURTIME - $STARTTIME))s)"
STARTTIME=$(date +%s)

clang $COMPILEPARAM platform_linux.c input.c memory.c ttmath.c -std=gnu99 -o game.out $EXELIBS

mv -v *.out $OUTDIR

CURTIME=$(date +%s)
echo "Done (creating executable took $(($CURTIME - $STARTTIME))s)"
