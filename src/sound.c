#include "stdafx.h"
#include "sound.h"

//code source: http://www.dunsanyinteractive.com/blogs/oliver/?p=72
//it was written in C++ so I had to rewrite the code in order
boolean loadWav(char* file, ALuint* buffer, ALsizei* size, ALsizei* freq, ALenum* format, uint8_t** dataPointer) {
	FILE* soundFile = NULL;
	WAVE_FORMAT wave_format;
	RIFF_HEADER riff_header;
	WAVE_DATA wave_data;
	uint8_t* data;

	boolean error = false;

	soundFile = fopen(file, "rb");

	if (soundFile != NULL) {
		fread(&riff_header, sizeof(RIFF_HEADER), 1, soundFile);

		if ((riff_header.chunkID[0] != 'R' ||
			riff_header.chunkID[1] != 'I' ||
			riff_header.chunkID[2] != 'F' ||
			riff_header.chunkID[3] != 'F') ||
			(riff_header.format[0] != 'W' ||
				riff_header.format[1] != 'A' ||
				riff_header.format[2] != 'V' ||
				riff_header.format[3] != 'E')) {

			error = true;
			printf("Invalid .wav file header!\n");
			goto error_handler;
		}

		fread(&wave_format, sizeof(WAVE_FORMAT), 1, soundFile);

		if (wave_format.subChunkID[0] != 'f' ||
			wave_format.subChunkID[1] != 'm' ||
			wave_format.subChunkID[2] != 't' ||
			wave_format.subChunkID[3] != ' ') 
		{
			error = true;
			printf("Invalid .wav format!\n");
			goto error_handler;
		}

		if (wave_format.subChunkSize > 16) {
			fseek(soundFile, sizeof(int16_t), SEEK_CUR);
		}

		fread(&wave_data, sizeof(WAVE_DATA), 1, soundFile);
		
		if (wave_data.subChunkID[0] != 'd' ||
			wave_data.subChunkID[1] != 'a' ||
			wave_data.subChunkID[2] != 't' ||
			wave_data.subChunkID[3] != 'a') 
		{
			error = true;
			printf("Invalid data header!\n");
			goto error_handler;
		}

		data = malloc(wave_data.subChunk2Size);

		if (!fread(data, wave_data.subChunk2Size, 1, soundFile)) {
			error = true;
			printf("Error loading WAVE data into struct!\n");
			free(data);
			goto error_handler;
		}

		*size = wave_data.subChunk2Size;
		*freq = wave_format.sampleRate;
		*dataPointer = data;
		
		if (wave_format.numChannels == 1) {
			if (wave_format.bitsPerSample == 8)
				*format = AL_FORMAT_MONO8;
			else if (wave_format.bitsPerSample == 16)
				*format = AL_FORMAT_MONO16;
		}
		else if (wave_format.numChannels == 2) {
			if (wave_format.bitsPerSample == 8)
				*format = AL_FORMAT_STEREO8;
			else if (wave_format.bitsPerSample == 16)
				*format = AL_FORMAT_STEREO16;
		}
		
		alGenBuffers(1, buffer);
		
		alBufferData(*buffer, *format, (void*)data, *size, *freq);
		fclose(soundFile);
		printf("[DEBUG] - Loaded .wav file: %s %.2lf MB depth: %d channels: %d \n", file, wave_data.subChunk2Size / 1024.0 / 1024.0, wave_format.bitsPerSample, wave_format.numChannels);
		return true;
	}
	if (soundFile == NULL) {
		return false;
	}
	error_handler:
	if (error) {
		fclose(soundFile);
		return false;
	}
	return false;
}

void sound_construct(Sound** sound, char* file) {
	if (sound == NULL) { return; }

	*sound = malloc(sizeof(Sound));

	if (*sound == NULL) {
		//shit happens
		exit(0xDEADBEEF);
	}

	boolean loaded = loadWav(file, &((*sound)->buffer), &((*sound)->size), &((*sound)->freq), &((*sound)->format), &((*sound)->data));
	if (!loaded) {
		printf("Couldn't load wave file: %s\n", file);
		free(*sound);
		*sound = NULL;
	}
}

void sound_destruct(Sound* sound) {
	if (sound == NULL) { return; }
	alDeleteBuffers(1, &(sound->buffer));
	free(sound->data);
	free(sound);
}

void sound_source_play(Sound_source* source) {
	if(source == NULL) { return; }
	alSourcePlay(source->source);
}

void sound_source_stop(Sound_source* source) {
	alSourceStop(source->source);
}

void sound_source_construct(Sound_source** source, Sound* sound) {
	if(source == NULL) { return; }
	*source = malloc(sizeof(Sound_source));
	if (*source == NULL) {
		//shit happens
		exit(0xDEADBEEF);
	}

	alGenSources(1, &((*source)->source));
	sound_source_setloop(*source, false);
	sound_source_setvolume(*source, 1);
	sound_source_setpos(*source, 0, 0, 0);
	if (sound != NULL) {
		alSourcei((*source)->source, AL_BUFFER, sound->buffer);
	}
}

void sound_source_destruct(Sound_source* source) {
	if (source == NULL) { return; }

	alDeleteSources(1, &(source->source));
	free(source);
}

void sound_source_setsound(Sound_source* source, Sound* sound) {
	if(source == NULL || sound == NULL) { return; }
	source->sound = sound;
	alSourcei(source->source, AL_BUFFER, sound->buffer);
}
void sound_source_setloop(Sound_source* source, boolean loop) {
	if(source == NULL) { return; }
	source->looping = loop;
	alSourcei(source->source, AL_LOOPING, (loop ? AL_TRUE : AL_FALSE));
}
void sound_source_setpos(Sound_source* source, ALfloat x, ALfloat y, ALfloat z) {
	if(source == NULL) { return; }
	source->x = x;
	source->y = y;
	source->z = z;
	alSource3f(source->source, AL_POSITION, x, y, z);
}

void sound_source_setvolume(Sound_source* source, ALfloat gain) {
	if(source == NULL) { return; }
	source->gain = gain;
	alSourcef(source->source, AL_GAIN, gain);
}
