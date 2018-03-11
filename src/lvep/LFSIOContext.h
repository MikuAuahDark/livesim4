#pragma once

// LOVE
#include <filesystem/File.h>

// FFMPEG
extern "C"
{
#include <libavformat/avio.h>
}

class LFSIOContext
{
public:
	LFSIOContext(love::filesystem::File *file);
	~LFSIOContext();

	operator AVIOContext*();

private:
	AVIOContext *context;
	love::filesystem::File *file;

	unsigned char *buffer;
	int bufferSize;

	static int read(void *opaque, uint8_t *buf, int bufSize);
	static int64_t seek(void *opaque, int64_t offset, int whence);
};

