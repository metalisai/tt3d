#include "audio.h"

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>
#include <sys/stat.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>

// TODO: remove globals
ALuint buffers[NUM_BUFFERS];
ALuint source[1];

int initAudio()
{
    alutInit(0, NULL);
    alGetError();
    ALenum error;
    buffers[0] = alutCreateBufferFromFile("test.wav");
    if ( (error=alutGetError()) != ALUT_ERROR_NO_ERROR )
    {
        printf("Unable to create buffer from a file! error:%s\n",alutGetErrorString(error));
        return 0;
    }
    alGenSources(1, source);
    if ((error = alGetError()) != AL_NO_ERROR)
    {
      printf("alGenSources : %d", error);
      return 0;
    }
    alSourcei(source[0], AL_LOOPING, AL_TRUE);
    alSourcei(source[0], AL_BUFFER, buffers[0]);
    if ((error = alGetError()) != AL_NO_ERROR)
    {
      printf("alSourcei : %d", error);
      return 0;
    }
    Vec3 pos(0.f,0.f,0.f);
    Vec3 vel(0.f,0.f,0.f);
    float dir[6];
    dir[0] = 1.f;
    dir[1] = 0.f;
    dir[2] = 0.f;
    dir[3] = 0.f;
    dir[4] = 1.f;
    dir[5] = 0.f;
    alSourcefv (source[0], AL_POSITION, &pos.x);
    alSourcefv (source[0], AL_VELOCITY, &vel.x);
    alSourcefv (source[0], AL_DIRECTION, dir);
    alListenerfv(AL_POSITION,&pos.x);
    alListenerfv(AL_VELOCITY,&vel.x);
    alListenerfv(AL_ORIENTATION, dir);
    alSourcePlay(source[0]);
    return 0;
}

void setListenerTransform(Vec3 pos, Vec3 fwd, Vec3 up)
{
    float orient[6];
    orient[0] =fwd.x;
    orient[1] =fwd.y;
    orient[2] =fwd.z;
    orient[3] =up.x;
    orient[4] =up.y;
    orient[5] =up.z;
    alListener3f(AL_POSITION,pos.x,pos.y,pos.z);
    alListenerfv(AL_ORIENTATION, orient);
}

void audioExit()
{
    alutExit();
}
