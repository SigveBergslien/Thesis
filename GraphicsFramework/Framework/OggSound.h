#pragma once
#include "sound.h"
#include <ogg/ogg.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisenc.h>
#include <vorbis/vorbisfile.h>

#define BUFFERLENGTH 32768	
class OggSound : public Sound
{
public:
	OggSound(void);
	virtual ~OggSound(void);

	void			LoadFromOgg(string filename);
	virtual double	StreamData(ALuint	buffer, double timeLeft);

protected:
	FILE*			fileHandle;
	OggVorbis_File	streamHandle;
};

