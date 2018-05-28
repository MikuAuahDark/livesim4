#include "LFSIOContext.h"

LFSIOContext::LFSIOContext(love::filesystem::File *file)
	: bufferSize(4096)
	, file(file)
	, fileData(nullptr)
{
	if (file->isOpen())
		file->close();
	if (!file->open(love::filesystem::File::MODE_READ))
		throw love::Exception("Could not open input file");
	unsigned char *buffer = (unsigned char*) av_malloc(bufferSize);
	context = avio_alloc_context(buffer, bufferSize, 0, this, read, nullptr, seek);
	file->retain();
}

LFSIOContext::LFSIOContext(love::Data *fileData)
	: bufferSize(4096)
	, file(nullptr)
	, fileData(fileData)
{
	unsigned char *buffer = (unsigned char*) av_malloc(bufferSize);
	context = avio_alloc_context(buffer, bufferSize, 0, this, read, nullptr, seek);
	fileData->retain();
}

LFSIOContext::~LFSIOContext()
{
	if (file) file->release();
	if (fileData) fileData->release();
	av_free(context);
}

LFSIOContext::operator AVIOContext *()
{
	return context;
}

int LFSIOContext::readFile(uint8_t *buf, int bufSize)
{
	return this->file->read(buf, bufSize);
}

int LFSIOContext::readFileData(uint8_t *buf, int bufSize)
{
	size_t size = fileData->getSize();
	size_t maxRead = bufSize;
	if ((bufSize + fileDataPos) > size)
		maxRead = size - fileDataPos;

	if (maxRead == 0) return 0;

	char *ptr = (char *) fileData->getData() + fileDataPos;
	memcpy(buf, ptr, maxRead);
	fileDataPos += maxRead;
	return maxRead;
}

int64_t LFSIOContext::seekFile(int64_t offset, int whence)
{
	if (whence == SEEK_CUR)
		offset += this->file->tell();
	else if (whence == SEEK_END)
		offset += this->file->getSize();
	else if (whence == AVSEEK_SIZE)
		return this->file->getSize();

	this->file->seek(offset);
	return this->file->tell();
}

int64_t LFSIOContext::seekFileData(int64_t offset, int whence)
{
	size_t size = fileData->getSize();

	if (whence == SEEK_CUR)
		offset += fileDataPos;
	else if (whence == SEEK_END)
		offset += size;
	else if (whence == AVSEEK_SIZE)
		return size;

	return fileDataPos = offset > size ? size : (offset < 0 ? 0 : offset);
}

int LFSIOContext::read(void *opaque, uint8_t *buf, int bufSize)
{
	LFSIOContext *self = (LFSIOContext*) opaque;
	if (self->fileData)
		return self->readFileData(buf, bufSize);
	else
		return self->readFile(buf, bufSize);
}

int64_t LFSIOContext::seek(void *opaque, int64_t offset, int whence)
{
	LFSIOContext *self = (LFSIOContext*) opaque;
	if (self->fileData)
		return self->seekFileData(offset, whence);
	else
		return self->seekFile(offset, whence);
}
