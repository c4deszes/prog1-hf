#pragma once
#include "library.h"
#include "AL/al.h"

//used to validate the header format of the .wav file
typedef struct RIFF_HEADER {
	uint8_t chunkID[4];
	long chunkSize;
	uint8_t format[4];
} RIFF_HEADER;

//used to store the audio format of the .wav file
typedef struct WAVE_FORMAT {
	uint8_t subChunkID[4];
	long subChunkSize;
	int16_t audioFormat;
	int16_t numChannels;
	long sampleRate;
	long byteRate;
	int16_t blockAlign;
	int16_t bitsPerSample;
} WAVE_FORMAT;

//wave data
typedef struct WAVE_DATA {
	uint8_t subChunkID[4];
	long subChunk2Size;
} WAVE_DATA;

//storing the file name, audio buffer location, etc.
typedef struct Sound {
	char* file;
	ALuint buffer;			//AL buffer id
	ALsizei size;			//Buffer length in bytes
	ALsizei freq;			
	ALenum format;
	uint8_t *data;			//Data location
} Sound;

//playing back sound
typedef struct Sound_source {
	ALuint source;			//AL source id
	Sound* sound;			//
	ALfloat x, y, z;		//Source location
	boolean looping;		
	ALfloat gain;			//Volume
} Sound_source;

//Sound source
//contructing new sound source
void sound_source_construct(Sound_source** source, Sound* sound);

void sound_source_destruct(Sound_source* source);
//setting sound buffer location
void sound_source_setsound(Sound_source* source, Sound* sound);
//enable/disable loop
void sound_source_setloop(Sound_source* source, boolean loop);
//setting source location
void sound_source_setpos(Sound_source* source, ALfloat x, ALfloat y, ALfloat z);
//starting sound playback
void sound_source_play(Sound_source* source);
//stopping sound playback
void sound_source_stop(Sound_source* source);
//setting volume
void sound_source_setvolume(Sound_source* source, ALfloat gain);

//Sound
//construct new sound object
void sound_construct(Sound** sound, char* file);

void sound_destruct(Sound* sound);
//loading wave file, this should not be used out of sound_construct's scope, since it
//will allocate the wave data even if the buffer pointer is not stored
boolean loadWav(char* file, ALuint* buffer, ALsizei* size, ALsizei* freq, ALenum* format, uint8_t** data);