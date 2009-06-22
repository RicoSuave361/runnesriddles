#include "Audio.h"

Audio::Audio(void)
{
	// Initialize Framework
	ALFWInit();
	if (!ALFWInitOpenAL())
	{
		ALFWprintf("Failed to initialize OpenAL\n");
		ALFWShutdown();
		//exit(0);
	}
	// Generate an AL Buffer
	alGenBuffers( 1, &uiBuffer );

}
Audio::~Audio(void)
{
	alSourceStop(uiSource);
    alDeleteSources(1, &uiSource);
	alDeleteBuffers(1, &uiBuffer);
	ALFWShutdownOpenAL();
	ALFWShutdown();
}
void Audio::Play(const char* dir){	
	if (!ALFWLoadWaveToBuffer((char*)ALFWaddMediaPath(dir), uiBuffer))
	{
		ALFWprintf("Failed to load %s\n", ALFWaddMediaPath(dir));
		return;
	}

	// Generate a Source to playback the Buffer
    alGenSources( 1, &uiSource );

	// Attach Source to Buffer
	alSourcei( uiSource, AL_BUFFER, uiBuffer );

	// Play Source
    alSourcePlay( uiSource );
	ALFWprintf("Play: %s\n", ALFWaddMediaPath(dir));
}