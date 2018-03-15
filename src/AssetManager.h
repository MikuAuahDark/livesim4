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

#ifndef LIVESIM_ASSETMANAGER_H
#define LIVESIM_ASSETMANAGER_H

// std
#include <map>
#include <string>

// love
#include "modules/filesystem/Filesystem.h"
#include "modules/filesystem/physfs/Filesystem.h"
#include "modules/font/Font.h"
#include "modules/graphics/Graphics.h"
#include "modules/graphics/opengl/Graphics.h"
#include "modules/image/Image.h"

namespace livesim
{
namespace asset
{

/**
 * Load image (string filename). User must release the Image!
 *
 * @param filename Image filename
 * @return Drawable image object (or nullptr on failure)
 */
love::graphics::opengl::Image *loadImage(std::string filename);
/**
 * Load image (File object). User must release the Image!
 *
 * @param filename Image filename
 * @return Drawable image object (or nullptr on failure)
 */
love::graphics::opengl::Image *loadImage(love::filesystem::File *filename);
/**
 * Load font (string filename). User must release the Font!
 *
 * @param filename Font filename
 * @param size Font size
 * @return Font object (or nullptr on failure)
 */
love::graphics::opengl::Font *loadFont(std::string filename, int size = 12);
/**
 * Load font (File object). User must release the Font!
 *
 * @param filename Font filename (or nullptr for Vera sans)
 * @param size Font size
 * @return Font object (or nullptr on failure)
 */
love::graphics::opengl::Font *loadFont(love::filesystem::File *filename, int size = 12);
/**
 * Load font (Vera sans). User must release the Font!
 *
 * @param size Font size
 * @return Font object (or nullptr on failure)
 */
love::graphics::opengl::Font *loadFont(int size = 12);

} // asset
} // livesim

#endif
