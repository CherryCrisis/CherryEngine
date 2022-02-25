#include "pch.hpp"

#include "engine.hpp"
#include "al.h"
#include "alc.h"
#include "alut.h"

Engine::Engine() 
{
	SoundInit();
}

void Engine::SoundInit()
{
	if (!alutInit(nullptr, nullptr))
	{
		ALenum error = alutGetError();
		fprintf(stderr, "%s\n", alutGetErrorString(error));
		exit(EXIT_FAILURE);
	}
}

void Engine::PlayFile(const char* fileName)
{
	ALuint buffer;
	ALuint source;
	ALenum error;
	ALint status;

	// Create an AL buffer from the given sound file. 
	buffer = alutCreateBufferFromFile(fileName);
	if (buffer == AL_NONE)
	{
		error = alutGetError();
		fprintf(stderr, "Error loading file: '%s'\n",
			alutGetErrorString(error));
		alutExit();
		exit(EXIT_FAILURE);
	}

	/* Generate a single source, attach the buffer to it and start playing. */
	alGenSources(1, &source);
	alSourcei(source, AL_BUFFER, buffer);
	alSourcePlay(source);

	/* Normally nothing should go wrong above, but one never knows... */
	error = alGetError();
	if (error != ALUT_ERROR_NO_ERROR)
	{
		fprintf(stderr, "%s\n", alGetString(error));
		alutExit();
		exit(EXIT_FAILURE);
	}
	
	/* Check every 0.1 seconds if the sound is still playing. */
	do
	{
		alutSleep(0.1f);
		alGetSourcei(source, AL_SOURCE_STATE, &status);
	} while (status == AL_PLAYING);
}

void Engine::Tick() 
{
	for (Behaviour* behaviour : behaviours)
	{
		behaviour->Update();
	}
}

void Engine::Launch() 
{
	isPlaying = true;

	for (Behaviour* behaviour : behaviours) 
	{
		behaviour->Start();
	}
}