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
#include "modules/graphics/Graphics.h"
#include "modules/graphics/opengl/Graphics.h"

// lovewrap
#include "LOVEWrap.h"
#include "SafeGuard.h"

namespace lovewrap
{
namespace graphics
{
using namespace love::graphics;

void push(Graphics::StackType type)
{
	return getInstance()->push(type);
}

void rotate(float f)
{
	return getInstance()->rotate(f);
}

void scale(float x, float y)
{
	return getInstance()->scale(x, y);
}

void scale(float s)
{
	return getInstance()->scale(s, s);
}

void translate(float x, float y)
{
	return getInstance()->translate(x, y);
}

void shear(float kx, float ky)
{
	return getInstance()->shear(kx, ky);
}

void origin()
{
	return getInstance()->origin();
}

void pop()
{
	return getInstance()->pop();
}

Canvas *newCanvas()
{
	auto inst = getInstance();
	return newCanvas(inst->getWidth(), inst->getHeight());
}

Canvas *newCanvas(int32_t w, int32_t h, Canvas::Settings *setting)
{
	Canvas::Settings defaultSetting;
	auto inst = getInstance();

	if (setting == nullptr)
	{
		setting = &defaultSetting;
		defaultSetting.width = w;
		defaultSetting.height = h;
		defaultSetting.dpiScale = inst->getCurrentDPIScale();
		defaultSetting.format = love::PixelFormat::PIXELFORMAT_NORMAL;
		defaultSetting.type = TextureType::TEXTURE_2D;
		defaultSetting.msaa = 0;
		defaultSetting.mipmaps = Canvas::MipmapMode::MIPMAPS_NONE;
		defaultSetting.readable.hasValue = false;
	}
	return inst->newCanvas(*setting);
}

Font *newFont(love::font::Rasterizer *rast)
{
	return getInstance()->newFont(rast);
}

Font *newFont(int32_t size)
{
	auto finst = lovewrap::font::getInstance();
	auto vera = finst->newTrueTypeRasterizer(size, love::font::TrueTypeRasterizer::HINTING_NORMAL);
	auto ret = getInstance()->newFont(vera);

	vera->release();
	return ret;
}

Image *newImage(const std::string& filename, const Image::Settings *settings)
{
	auto lfs = lovewrap::filesystem::getInstance();
	auto f = lfs->newFile(filename.c_str());
	auto out = newImage(f, settings);
	f->release();
	return out;
}

Image *newImage(love::filesystem::File *file, const Image::Settings *settings)
{
	// Open file
	if (!file->isOpen())
	{
		if (!file->open(love::filesystem::File::Mode::MODE_READ))
			// File can't be opened
			return nullptr;
	}
	else
		file->seek(0);
	
	// Load ImageData
	love::filesystem::FileData *filedata = file->read();
	if (filedata == nullptr)
		return nullptr;

	auto out = newImage(filedata, settings);
	filedata->release();
	return out;
}

Image *newImage(love::filesystem::FileData *filedata, const Image::Settings *settings)
{
	auto li = lovewrap::image::getInstance();

	// Try CompressedImageData
	try
	{
		love::image::CompressedImageData *img = li->newCompressedData(filedata);
		auto out = newImage(img, settings);
		img->release();
		return out;
	}
	// Try ImageData
	catch (love::Exception &)
	{
		love::image::ImageData *img = li->newImageData(filedata);
		auto out = newImage(img, settings);
		img->release();
		return out;
	}
}

Image *newImage(love::image::ImageData *imagedata, const Image::Settings *settings)
{
	auto lg = getInstance();
	
	Image::Settings def;
	if (settings == nullptr)
	{
		settings = &def;
		def.dpiScale = lovewrap::graphics::getDPIScale();
		def.linear = false;
		def.mipmaps = false;
	}

	Image::Slices slices(TEXTURE_2D);
	slices.set(0, 0, imagedata);
	auto out = lg->newImage(slices, *settings);
	return out;
}

Image *newImage(love::image::CompressedImageData *cimgd, const Image::Settings *settings)
{
	auto lg = getInstance();
	
	Image::Settings def;
	if (settings == nullptr)
	{
		settings = &def;
		def.dpiScale = lovewrap::graphics::getDPIScale();
		def.linear = false;
		def.mipmaps = false;
	}

	Image::Slices slices(TEXTURE_2D);
	slices.add(cimgd, 0, 0, false, settings->mipmaps);
	auto out = lg->newImage(slices, *settings);
	return out;
}

void draw(Drawable *drawable, float x, float y, float r, float sx, float sy, float ox, float oy, float kx, float ky)
{
	getInstance()->draw(drawable, love::Matrix4(x, y, r, sx, sy, ox, oy, kx, ky));
}

void draw(Drawable *drawable, love::math::Transform *transform)
{
	getInstance()->draw(drawable, transform->getMatrix());
}

void draw(Texture *texture, Quad *quad, float x, float y, float r, float sx, float sy, float ox, float oy, float kx, float ky)
{
	getInstance()->draw(texture, quad, love::Matrix4(x, y, r, sx, sy, ox, oy, kx, ky));
}

void draw(Texture *texture, Quad *quad, love::math::Transform *transform)
{
	getInstance()->draw(texture, quad, transform->getMatrix());
}

} // graphics
} // lovewrap
