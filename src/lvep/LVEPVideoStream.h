#pragma once

// LOVE
#include <common/Object.h>
#include <video/VideoStream.h>
#include <filesystem/File.h>

// FFMPEG
extern "C"
{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavutil/pixdesc.h>
}

#include "FFMpegStream.h"

class LVEPVideoStream : public love::video::VideoStream
{
public:
	LVEPVideoStream(love::filesystem::File *file);
	virtual ~LVEPVideoStream();

	virtual int getWidth() const;
	virtual int getHeight() const;
	virtual const std::string &getFilename() const;
	virtual void fillBackBuffer();
	virtual const void *getFrontBuffer() const;
	virtual size_t getSize() const;
	virtual bool swapBuffers();

private:
	FFMpegStream *stream;
	love::StrongRef<love::filesystem::File> file;

	AVFrame *frame;
	bool dirty;
	bool eos;
	double previousTime;
	double previousFrame;

	love::video::VideoStream::Frame *frontBuffer;
	love::video::VideoStream::Frame *backBuffer;

	love::video::VideoStream::Frame *allocateBuffer();
	void tinySeek(double target);
};
