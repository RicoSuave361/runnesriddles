#include "Audio.h"

Audio::Audio(const int buffer,const char* dir)
{
	ALFWInit();
	// Initialize Framework
	if (!ALFWInitOpenAL())
	{
		if (!ALFWInitOpenAL())
		{
			ALFWprintf("Failed to initialize OpenAL\n");
			ALFWShutdown();
			return;
		}
	}
	// Generate an AL Buffer
	this->buffer=buffer;
	alGenBuffers(this->buffer, &uiBuffer );
	if (!ALFWLoadWaveToBuffer((char*)ALFWaddMediaPath(dir), uiBuffer))
	{
		this->buffer=-1;
		ALFWprintf("Failed to load %s\n", ALFWaddMediaPath(dir));
		return;
	}

}
Audio::~Audio(void)
{
	alSourceStop(uiSource);
    alDeleteSources(buffer, &uiSource);
	alDeleteBuffers(buffer, &uiBuffer);
	ALFWShutdownOpenAL();
	ALFWShutdown();
}
void Audio::Play(){	
	if(buffer!=-1){
		// Generate a Source to playback the Buffer
		alGenSources( buffer, &uiSource );

		// Attach Source to Buffer
		alSourcei( uiSource, AL_BUFFER, uiBuffer );

		// Play Source
		alSourcePlay( uiSource );
		ALFWprintf("Play: %d\n", buffer);
	}
}