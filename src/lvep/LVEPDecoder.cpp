#include "LVEPDecoder.h"

extern "C"
{
#include <libavutil/opt.h>
}

LVEPDecoder::LVEPDecoder(love::filesystem::File *file)
	: stream(file, FFMpegStream::TYPE_AUDIO)
	, frame(nullptr)
	, file(file)
	, eos(false)
{
	frame = av_frame_alloc();
	if (!stream.readFrame(frame))
	{
		av_frame_free(&frame);
		throw love::Exception("No first frame");
	}

	recodeContext = swr_alloc();
	int channelLayout = frame->channel_layout;
	if (!channelLayout)
		channelLayout = av_get_default_channel_layout(frame->channels);

	av_opt_set_int(recodeContext, "in_channel_layout", channelLayout, 0);
	av_opt_set_int(recodeContext, "in_sample_rate", frame->sample_rate, 0);
	av_opt_set_sample_fmt(recodeContext, "in_sample_fmt", (AVSampleFormat) frame->format, 0);

	av_opt_set_int(recodeContext, "out_channel_layout", channelLayout, 0);
	av_opt_set_int(recodeContext, "out_sample_rate", frame->sample_rate, 0);
	av_opt_set_sample_fmt(recodeContext, "out_sample_fmt", AV_SAMPLE_FMT_S16, 0);
	swr_init(recodeContext);
}

LVEPDecoder::~LVEPDecoder()
{
	swr_free(&recodeContext);
	av_frame_free(&frame);
}

love::sound::Decoder *LVEPDecoder::clone()
{
	// FIXME?
	throw love::Exception("LVEP does not support cloning Decoders");
}

int LVEPDecoder::getSize() const
{
	return bufferSize;
}

void *LVEPDecoder::getBuffer() const
{
	return (void*) buffer;
}

int LVEPDecoder::decode()
{
	if (!stream.readFrame(frame))
	{
		eos = true;
		return 0;
	}
	uint8_t *buffers[1] = {buffer};
	int decoded = swr_convert(recodeContext,
				buffers, bufferSize,
				(const uint8_t**) &frame->data[0], frame->nb_samples*frame->channels);
	if (decoded < 0)
		return 0;
	return decoded*frame->channels;
}

bool LVEPDecoder::seek(float s)
{
	eos = false;
	return stream.seek(s);
}

bool LVEPDecoder::rewind()
{
	return seek(0);
}

bool LVEPDecoder::isSeekable()
{
	return true;
}

bool LVEPDecoder::isFinished()
{
	return eos;
}

int LVEPDecoder::getChannels() const
{
	return frame->channels;
}

int LVEPDecoder::getBitDepth() const
{
	return 16;
}

int LVEPDecoder::getSampleRate() const
{
	return frame->sample_rate;
}

double LVEPDecoder::getDuration()
{
	return stream.getDuration();
}
