#pragma once

#include "headers.h"
#include "Framework/Framework.h"
	

class Audio
{
public:
	Audio(const int buffer,const char* dir);
	void Play();
public:
	~Audio(void);
	ALuint      uiBuffer;
	ALuint      uiSource;  
	ALint       iState;
	int buffer;
	char *arch;
};
