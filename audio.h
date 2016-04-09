#ifndef AUDIO_H
#define AUDIO_H

#include "ttMath.h"

#define NUM_BUFFERS 2

int initAudio();
void setListenerTransform(Vec3 pos, Vec3 fwd, Vec3 up);
void audioExit();

#endif // AUDIO_H
