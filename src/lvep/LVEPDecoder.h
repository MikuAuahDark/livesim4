#pragma once

#include "FFMpegStream.h"

// LOVE
#include <filesystem/File.h>
#include <sound/Decoder.h>

// FFMPEG
extern "C"
{
#include <libswresample/swresample.h>
}

class LVEPDecoder : public love::sound::Decoder
{
public:
	LVEPDecoder(love::filesystem::File *file);
	virtual ~LVEPDecoder();

	love::sound::Decoder *clone();
	int decode();
	int getSize() const;
	void *getBuffer() const;
	bool seek(float s);
	bool rewind();
	bool isSeekable();
	bool isFinished();
	int getChannels() const;
	int getBitDepth() const;
	int getSampleRate() const;
	double getDuration();

private:
	FFMpegStream stream;
	AVFrame *frame;
	SwrContext *recodeContext;
	love::StrongRef<love::filesystem::File> file;
	bool eos;

	static const int bufferSize = 8192;
	uint8_t buffer[bufferSize];
};
