#pragma once

#include <string>
#include <map>
#include <iostream>
#include <fstream>

#include "../OpenAL1.1SDK/include/al.h"

using namespace std;

typedef unsigned long DWORD;

struct FMTCHUNK {
	short format;
	short channels;
	DWORD srate;
	DWORD bps;
	short balign;
	short samp;
};

class Sound {
public:
	char*			GetData()		{return data;}
	int				GetBitRate()	{return bitRate;}
	float			GetFrequency()	{return freqRate;}
	int				GetChannels()	{return channels;}
	int				GetSize()		{return size;}
	ALuint			GetBuffer()		{return buffer;}
	bool			IsStreaming()	{return streaming;}		//Part 2
	virtual double	StreamData(ALuint	buffer, double timeLeft){ return 0.0f;}							//Part 2

	ALenum			GetOALFormat();
	double			GetLength();

	static void		AddSound(string n);
	static Sound*	GetSound(string name);

	static void		DeleteSounds();

protected:
	Sound();
	virtual ~Sound(void);

	void			LoadFromWAV(string filename);
	void			LoadWAVChunkInfo(ifstream &file, string &name, unsigned int &size);

	char*			data;
	ALuint			buffer;

	bool			streaming;			//Part 2
	
	float			freqRate;
	double			length;
	unsigned int	bitRate;
	unsigned int	size;
	unsigned int	channels;

	static map<string, Sound*> sounds;
};