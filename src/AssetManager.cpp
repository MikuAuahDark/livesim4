/**
 * Copyright (c) 2039 Dark Energy Processor
 * 
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 * 
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

// love
#include "common/Module.h"

// AssetManager
#include "AssetManager.h"

namespace livesim
{
namespace asset
{

love::graphics::opengl::Image *loadImage(std::string filename)
{
	auto lfs = love::Module::getInstance<love::filesystem::physfs::Filesystem>(love::Module::M_FILESYSTEM);
	
	// Create new File object
	auto file = lfs->newFile(filename.c_str());
	auto out = loadImage(file);
	
	file->release();
	return out;
}
love::graphics::opengl::Image *loadImage(love::filesystem::File *filename)
{
	auto li = love::Module::getInstance<love::image::Image>(love::Module::M_IMAGE);
	auto lg = love::Module::getInstance<love::graphics::opengl::Graphics>(love::Module::M_GRAPHICS);

	// Open file
	if (!filename->isOpen())
	{
		if (!filename->open(love::filesystem::File::Mode::MODE_READ))
			// File can't be opened
			return nullptr;
	}
	else
		filename->seek(0);

	// Load ImageData
	love::filesystem::FileData *imagedata = filename->read();
	love::graphics::opengl::Image *image = nullptr;
	love::graphics::opengl::Image::Flags flg;
	flg.linear = false;
	flg.mipmaps = false;
	try
	{
		// Try CompressedImageData first
		love::image::CompressedImageData *cid_real = li->newCompressedData(imagedata);
		image = lg->newImage(std::vector<decltype(cid_real)>({cid_real}), flg);
		cid_real->release();
	}
	catch (love::Exception &)
	{
		try
		{
			// Now try ImageData
			love::image::ImageData *id_real = li->newImageData(imagedata);
			image = lg->newImage(std::vector<decltype(id_real)>({id_real}), flg);
			id_real->release();
		}
		catch (love::Exception &) {}
	}
	
	imagedata->release();
	return image;
}

love::graphics::opengl::Font *loadFont(std::string filename, int size)
{
	auto lfs = love::Module::getInstance<love::filesystem::Filesystem>(love::Module::M_FILESYSTEM);

	// Create new File object
	love::filesystem::File *f = lfs->newFile(filename.c_str());
	auto out = loadFont(f, size);

	f->release();
	return out;
}

love::graphics::opengl::Font *loadFont(love::filesystem::File *filename, int size)
{
	// If filename is nullptr, then use Vera sans variant
	if (filename == nullptr)
		return loadFont(size);
	
	auto lf = love::Module::getInstance<love::font::Font>(love::Module::M_FONT);

	// Open file
	if (!filename->isOpen())
	{
		if (!filename->open(love::filesystem::File::Mode::MODE_READ))
			// File can't be opened
			return nullptr;
	}
	else
		filename->seek(0);

	// Create FileData
	love::filesystem::FileData *fd = filename->read();
	love::font::Rasterizer *rast = nullptr;

	// Create TrueTypeRasterizer
	try
	{
		rast = lf->newTrueTypeRasterizer(fd, size, love::font::TrueTypeRasterizer::HINTING_NORMAL);
		fd->release();
	}
	catch (love::Exception &) {
		fd->release();
		return nullptr;
	}
	
	// Create new Font object
	auto lg = love::Module::getInstance<love::graphics::opengl::Graphics>(love::Module::M_GRAPHICS);
	auto out = lg->newFont(rast);
	rast->release();

	return out;
}

love::graphics::opengl::Font *loadFont(int size)
{
	auto lf = love::Module::getInstance<love::font::Font>(love::Module::M_FONT);
	auto lg = love::Module::getInstance<love::graphics::opengl::Graphics>(love::Module::M_GRAPHICS);

	// Load Vera sans rasterizer
	love::font::Rasterizer *rast = lf->newTrueTypeRasterizer(size, love::font::TrueTypeRasterizer::HINTING_NORMAL);
	auto out = lg->newFont(rast);
	rast->release();

	return out;
}

} // asset
} // livesim
