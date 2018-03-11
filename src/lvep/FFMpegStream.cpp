#include "FFMpegStream.h"

FFMpegStream::FFMpegStream(love::filesystem::File *file, StreamType type)
	: ioContext(file)
	, inputFormat(nullptr)
	, formatContext(nullptr)
	, codecContext(nullptr)
	, targetStream(-1)
	, type(type)
	, file(file)
{
	packet.buf = nullptr;
	const std::string &filename = file->getFilename();

	try
	{
		if (av_probe_input_buffer(ioContext, &inputFormat, filename.c_str(), nullptr, 0, 0) < 0)
			throw love::Exception("Could not probe format");

		formatContext = avformat_alloc_context();
		formatContext->pb = ioContext;
		formatContext->flags |= AVFMT_FLAG_CUSTOM_IO;

		if (avformat_open_input(&formatContext, filename.c_str(), inputFormat, nullptr) < 0)
			throw love::Exception("Could not open input stream");

		if (avformat_find_stream_info(formatContext, nullptr) < 0)
			throw love::Exception("Could not find stream information");

		AVMediaType targetType = type == TYPE_VIDEO ? AVMEDIA_TYPE_VIDEO : AVMEDIA_TYPE_AUDIO;
		for (unsigned int i = 0; i < formatContext->nb_streams; i++)
			if (formatContext->streams[i]->codec->codec_type == targetType)
			{
				targetStream = i;
				break;
			}

		if (targetStream == -1)
			throw love::Exception("File does not contain target stream type");

		for (unsigned int i = 0; i < formatContext->nb_streams; i++)
			if (i != targetStream)
				formatContext->streams[i]->discard = AVDISCARD_ALL;

		codecContext = formatContext->streams[targetStream]->codec;
		AVCodec *codec = avcodec_find_decoder(codecContext->codec_id);

		if (avcodec_open2(codecContext, codec, nullptr) < 0)
			throw love::Exception("Could not open target stream");
	}
	catch (love::Exception &e)
	{
		if (packet.buf)
			av_packet_unref(&packet);
		if (codecContext)
			avcodec_close(codecContext);
		if (formatContext)
			avformat_close_input(&formatContext);

		throw e;
	}
}

FFMpegStream::~FFMpegStream()
{
	if (packet.buf)
		av_packet_unref(&packet);
	avcodec_close(codecContext);
	avformat_close_input(&formatContext);
}

bool FFMpegStream::readPacket()
{
	do
		if (av_read_frame(formatContext, &packet) < 0)
			return false;
	while (packet.stream_index != targetStream);

	return true;
}

bool FFMpegStream::readFrame(AVFrame *frame)
{
	typedef int (*decoder_t)(AVCodecContext*,AVFrame*,int*,const AVPacket*);
	decoder_t decoder = type == TYPE_VIDEO ? &avcodec_decode_video2 : &avcodec_decode_audio4;

	int got_frame = 0;
	while (!got_frame)
	{
		if (!readPacket())
			return false;
		if (decoder(codecContext, frame, &got_frame, &packet) < 0)
			return false;
	}

	return true;
}

double FFMpegStream::translateTimestamp(int64_t ts) const
{
	AVRational &base = formatContext->streams[targetStream]->time_base;
	return (ts*base.num)/double(base.den);
}

double FFMpegStream::getDuration() const
{
	return translateTimestamp(formatContext->streams[targetStream]->duration);
}

bool FFMpegStream::seek(double target)
{
	AVRational &base = formatContext->streams[targetStream]->time_base;
	int64_t ts = target*base.den/double(base.num);
	avcodec_flush_buffers(codecContext);
	return av_seek_frame(formatContext, targetStream, ts, AVSEEK_FLAG_BACKWARD) >= 0;
}
