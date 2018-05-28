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

// STL
#include <string>
#include <vector>

// love.window
#include "modules/window/Window.h"

struct GameWindowSettings
{
#pragma region WindowSettings
	bool fullscreen;
	love::window::Window::FullscreenType fstype;
	int vsync;
	int msaa;
	bool stencil;
	int depth;
	bool resizable;
	int minwidth;
	int minheight;
	bool borderless;
	bool centered;
	int display;
	bool highdpi;
	double refreshrate;
	bool useposition;
	int x;
	int y;
#pragma endregion WindowSettings

	// Window size
	int width, height;
	// The window title
	std::string title;
	// Filepath to an image to use as the window's icon
	std::string icon;
};

/// Game configuration structure.
struct GameConfiguration
{
	// The name of the save directory
	std::string identity;
	// Search files in source directory before save directory
	bool appendidentity;
	// Enable the accelerometer on iOS and Android by exposing it as a Joystick
	bool accelerometerjoystick;
	// True to save files (and read from the save directory) in external storage on Android
	bool externalstorage;
	// Enable gamma-correct rendering, when supported by the system
	bool gammacorrect;
	// Keep background music playing when opening LOVE (iOS and Android only)
	bool mixwithsystem;

	// It is possible to defer window creation until love::window::setMode is first called
	// in your code. To do so, set this field to NULL. If this is done, LÖVE may crash if
	// any function from love.graphics is called before the first love::window::setMode in
	// your code.
	GameWindowSettings *window;
	// List of needed modules.
	std::vector<std::string> modules;
};
