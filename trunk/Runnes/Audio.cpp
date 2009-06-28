#include "Audio.h"

Audio::Audio(const int _buffer,const char* dir): buffer(_buffer)
{
	load=false;
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
	alGenBuffers(buffer, &uiBuffer );
	if (!ALFWLoadWaveToBuffer((char*)ALFWaddMediaPath(dir), uiBuffer))
	{
		ALFWprintf("Failed to load %s\n", ALFWaddMediaPath(dir));
		return;
	}
	load=true;

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