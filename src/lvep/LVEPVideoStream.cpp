#include "LVEPVideoStream.h"

#include <timer/Timer.h>

LVEPVideoStream::LVEPVideoStream(love::filesystem::File *file)
	: stream(new FFMpegStream(file, FFMpegStream::TYPE_VIDEO))
	, file(file)
	, dirty(false)
	, eos(false)
	, previousTime(0)
	, previousFrame(0)
{
	frame = av_frame_alloc();
	if (!stream->readFrame(frame))
	{
		av_frame_free(&frame);
		delete stream;
		throw love::Exception("No first frame");
	}

	frontBuffer = allocateBuffer();
	backBuffer = allocateBuffer();

	// frameSync is a StrongRef, so it retains itself, so after set it has a reference
	// count of 2, rather than 1
	frameSync.set(new love::video::VideoStream::DeltaSync());
	frameSync->release();

	previousTime = love::timer::Timer::getTime();
}

LVEPVideoStream::~LVEPVideoStream()
{
	av_frame_free(&frame);
	delete frontBuffer;
	delete backBuffer;
	delete stream;
}

love::video::VideoStream::Frame *LVEPVideoStream::allocateBuffer()
{
	love::video::VideoStream::Frame *buffer;
	buffer = new love::video::VideoStream::Frame();
	buffer->yw = getWidth();
	buffer->yh = getHeight();

	// TODO: Format support (non yuv420p)
	// swrast to recode?
	buffer->cw = getWidth()/2;
	buffer->ch = getHeight()/2;

	buffer->yplane = new unsigned char[buffer->yw*buffer->yh];
	buffer->cbplane = new unsigned char[buffer->cw*buffer->ch];
	buffer->crplane = new unsigned char[buffer->cw*buffer->ch];

	return buffer;
}

int LVEPVideoStream::getWidth() const
{
	return frame->width;
}

int LVEPVideoStream::getHeight() const
{
	return frame->height;
}

const std::string &LVEPVideoStream::getFilename() const
{
	return file->getFilename();
}

void LVEPVideoStream::fillBackBuffer()
{
	double curTime = love::timer::Timer::getTime();
	double dt = curTime-previousTime;
	previousTime = curTime;

	frameSync->update(dt);
	double time = frameSync->getPosition();

	// If we've drawn a frame past the current timestamp, we must have rewound
	if (time < previousFrame)
	{
		stream->seek(time);
		stream->readFrame(frame);

		// Now we're at the keyframe before our target, look for the actual frame
		tinySeek(time);
		eos = false;
	}

	if (eos)
		return;

	double pts = stream->translateTimestamp(frame->pkt_pts);
	if (time < pts)
		return;

	if (time > pts+15) // We're far behind, do a large seek
	{
		stream->seek(time);
		stream->readFrame(frame);
	}
	if (time > pts+0.2) // We're a bit behind, do a tiny seek
		tinySeek(time);

	dirty = true;
	previousFrame = pts;

	memcpy(backBuffer->yplane, frame->data[0], backBuffer->yw*backBuffer->yh);
	memcpy(backBuffer->cbplane, frame->data[1], backBuffer->cw*backBuffer->ch);
	memcpy(backBuffer->crplane, frame->data[2], backBuffer->cw*backBuffer->ch);

	if (!stream->readFrame(frame))
		eos = true;
}

const void *LVEPVideoStream::getFrontBuffer() const
{
	return (void*) frontBuffer;
}

size_t LVEPVideoStream::getSize() const
{
	return sizeof(love::video::VideoStream::Frame);
}

bool LVEPVideoStream::swapBuffers()
{
	if (!dirty)
		return false;

	dirty = false;
	love::video::VideoStream::Frame *temp = frontBuffer;
	frontBuffer = backBuffer;
	backBuffer = temp;
	return true;
}

void LVEPVideoStream::tinySeek(double target)
{
	while (target > stream->translateTimestamp(frame->pkt_pts + frame->pkt_duration))
		stream->readFrame(frame);
}
