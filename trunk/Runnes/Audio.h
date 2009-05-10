#pragma once

#include "headers.h"
#include "Framework/Framework.h"
	

class Audio
{
public:
	Audio(void);
	void Play(const char* dir);
	//void Play();
	void Pause();
	void Stop();
	void Encolar();
public:
	~Audio(void);
	ALuint      uiBuffer;
	ALuint      uiSource;  
	ALint       iState;
};
