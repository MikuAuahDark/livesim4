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
#include <algorithm>
#include <map>
#include <string>

#include "Boot.h"

// Lua
#include "lua.hpp"

// SDL
#include <SDL_hints.h>

// LOVEWrap
#include "lovewrap/LOVEWrap.h"

// List of LOVE modules
extern "C" {
int luaopen_love_data(lua_State *L);
int luaopen_love_timer(lua_State *L);
int luaopen_love_event(lua_State *L);
int luaopen_love_keyboard(lua_State *L);
int luaopen_love_joystick(lua_State *L);
int luaopen_love_mouse(lua_State *L);
int luaopen_love_touch(lua_State *L);
int luaopen_love_sound(lua_State *L);
int luaopen_love_system(lua_State *L);
int luaopen_love_audio(lua_State *L);
int luaopen_love_image(lua_State *L);
int luaopen_love_video(lua_State *L);
int luaopen_love_font(lua_State *L);
int luaopen_love_window(lua_State *L);
int luaopen_love_graphics(lua_State *L);
int luaopen_love_math(lua_State *L);
int luaopen_love_physics(lua_State *L);
}
#define MAKE_LOVE_MODULE_PAIR(name) std::make_pair<std::string, lua_CFunction>(std::string(#name), &luaopen_love_##name)
static std::map<std::string, lua_CFunction> loveModules = {
	MAKE_LOVE_MODULE_PAIR(data),
	MAKE_LOVE_MODULE_PAIR(timer),
	MAKE_LOVE_MODULE_PAIR(event),
	MAKE_LOVE_MODULE_PAIR(keyboard),
	MAKE_LOVE_MODULE_PAIR(joystick),
	MAKE_LOVE_MODULE_PAIR(mouse),
	MAKE_LOVE_MODULE_PAIR(touch),
	MAKE_LOVE_MODULE_PAIR(sound),
	MAKE_LOVE_MODULE_PAIR(system),
	MAKE_LOVE_MODULE_PAIR(audio),
	MAKE_LOVE_MODULE_PAIR(image),
	MAKE_LOVE_MODULE_PAIR(video),
	MAKE_LOVE_MODULE_PAIR(font),
	MAKE_LOVE_MODULE_PAIR(window),
	MAKE_LOVE_MODULE_PAIR(graphics),
	MAKE_LOVE_MODULE_PAIR(math),
	MAKE_LOVE_MODULE_PAIR(physics)
};
#undef MAKE_LOVE_MODULE_PAIR

// This is absolutely needed!
extern "C" int luaopen_love_filesystem(lua_State *L);

// Contains the game configuration, declared in conf.cpp
extern GameConfiguration conf;

void bootLivesim4(lua_State *L)
{
	// Require love.filesystem
	lua_pushcfunction(L, luaopen_love_filesystem);
	lua_pushstring(L, "love.filesystem");
	lua_call(L, 1, 0);

	// Set source
	std::string exePath = lovewrap::filesystem::getExecutablePath();
	bool canHasGame = lovewrap::filesystem::setSource(exePath);
	
	// Set fused flag
	lovewrap::filesystem::setFused(canHasGame);
	// Set deprecation warn
	love::setDeprecationOutputEnabled(!canHasGame);

	// Set accelerometer joystick
	SDL_SetHint(SDL_HINT_ACCELEROMETER_AS_JOYSTICK, conf.accelerometerjoystick ? "1" : "0");
	// Set gamma correct
	love::graphics::setGammaCorrect(conf.gammacorrect);

	// Load desired modules
	for (std::string i : conf.modules)
	{
		// Is valid module name?
		if (loveModules.count(i))
		{
			lua_pushcfunction(L, loveModules[i]);
			lua_pushfstring(L, "love.%s", i.c_str());
			lua_call(L, 1, 0);
		}
	}

	// Setup window
	if (conf.window != nullptr && std::find(conf.modules.begin(), conf.modules.end(), std::string("window")) != conf.modules.end())
	{
		// The casting is assumed to be valid, unless LOVE add some alignment in the
		// love::window::WindowSettings struct.
		bool set = lovewrap::window::setMode(conf.window->width, conf.window->height, (love::window::WindowSettings *)&conf.window);
		if (!set)
			throw love::Exception("Could not set window mode");

		lovewrap::window::setTitle(conf.window->title);
		if (!conf.window->icon.empty())
		{
			// TODO: set window icon
		}
	}

	if (lovewrap::isModuleLoaded("timer"))
		// first timestep, because window creation can take some time
		lovewrap::timer::step();
	// love.filesystem thing, set identity and save
	{
		love::filesystem::Filesystem *x = love::Module::getInstance<love::filesystem::Filesystem>(love::Module::M_FILESYSTEM);
		x->setAndroidSaveExternal(conf.externalstorage);
		bool identitySet = x->setIdentity(conf.identity.c_str(), conf.appendidentity);
		if (!identitySet)
			throw love::Exception("Failed to set identity to: %s", conf.identity.c_str());
	}

	// We all set, no need to check & load main.lua and conf.lua
	// because our game code is written in C++ entirely :P
}
