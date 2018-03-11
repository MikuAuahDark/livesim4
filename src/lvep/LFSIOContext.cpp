#include "LFSIOContext.h"

LFSIOContext::LFSIOContext(love::filesystem::File *file)
	: bufferSize(4096)
	, file(file)
{
	if (file->isOpen())
		file->close();
	if (!file->open(love::filesystem::File::MODE_READ))
		throw love::Exception("Could not open input file");
	unsigned char *buffer = (unsigned char*) av_malloc(bufferSize);
	context = avio_alloc_context(buffer, bufferSize, 0, this, read, nullptr, seek);
}

LFSIOContext::~LFSIOContext()
{
	av_free(context);
}

LFSIOContext::operator AVIOContext *()
{
	return context;
}

int LFSIOContext::read(void *opaque, uint8_t *buf, int bufSize)
{
	LFSIOContext *self = (LFSIOContext*) opaque;
	return self->file->read(buf, bufSize);
}

int64_t LFSIOContext::seek(void *opaque, int64_t offset, int whence)
{
	LFSIOContext *self = (LFSIOContext*) opaque;

	if (whence == SEEK_CUR)
		offset += self->file->tell();
	else if (whence == SEEK_END)
		offset += self->file->getSize();
	else if (whence == AVSEEK_SIZE)
		return self->file->getSize();

	self->file->seek(offset);
	return self->file->tell();
}
