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

// std
#include <cstdlib>
#include <exception>

extern "C" {
// Lua
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "luajit.h"

// libav
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
}

// PhysFS
#include "libraries/physfs/physfs.h"

// love
#include "common/Module.h"
#include "modules/love/love.h"

// love.graphics
#include "modules/graphics/Graphics.h"
#include "modules/graphics/opengl/Graphics.h"

// lvep
extern "C" int luaopen_lvep(lua_State *L);

// Asset manager
#include "AssetManager.h"

// scene
#include "Scene.h"

// Lovewrap
#include "lovewrap/LOVEWrap.h"

#ifdef LOVE_WINDOWS
extern "C" {
LOVE_EXPORT DWORD NvOptimusEnablement = 1;
LOVE_EXPORT DWORD AmdPowerXpressRequestHighPerformance = 1;
}
#endif

class SimpleSceneTest: public livesim::base::Scene
{
public:
	SimpleSceneTest(void *args): Scene(args)
	{
		livesim2_image = lovewrap::graphics::newImage("livesim2_icon.png");
	}
	~SimpleSceneTest()
	{
		fprintf(stdout, "love.quit");
		livesim2_image->release();
	}
	void draw()
	{
		auto g = love::Module::getInstance<love::graphics::Graphics>(love::Module::M_GRAPHICS);
		g->clear(love::Optional<love::Colorf>(love::Colorf(255.0, 255.0, 255.0, 255.0)), love::OptionalInt(), love::OptionalDouble());
		//livesim2_image->draw(0, 0, 0, 0.25, 0.25, 0, 0, 0, 0);
		lovewrap::graphics::draw(livesim2_image, 0, 0, 0.25, 0.25);
	}
	void update(double deltaT)
	{
		fprintf(stdout, "deltaT: %g\n", deltaT);
	}
private:
	love::graphics::Drawable *livesim2_image;
};

// Simple inlined function :P
inline void setCFunction(lua_State *L, const char *name, lua_CFunction func)
{
	auto f = [](lua_State *L)->int
	{
		lua_CFunction func = (lua_CFunction) (lua_topointer(L, lua_upvalueindex(1)));

		try
		{
			return func(L);
		}
		catch(love::Exception &e)
		{
			return luaL_error(L, "LOVE Error: %s", e.what());
		}
		catch(std::exception &e)
		{
			return luaL_error(L, "Exception: %s", e.what());
		}
		catch(...)
		{
			return luaL_error(L, "Unknown exception!");
		}
	};

	lua_pushstring(L, name);
	lua_pushlightuserdata(L, func);
	lua_pushcclosure(L, f, 1);
	lua_rawset(L, -3);
}

// This function hijack most LOVE callbacks and love.update and love.draw
int hijackHandlers(lua_State *L)
{
	using namespace livesim::base;

	// Load our scene
	loadScene(new SimpleSceneTest(nullptr));
	swapScene(); // forced

	// Get love table
	lua_getglobal(L, "love");

	// love.update
	setCFunction(L, "update", [](lua_State *L)->int
	{
		getCurrentScene()->update(luaL_checknumber(L, 1));
		return 0;
	});

	// love.draw
	setCFunction(L, "draw", [](lua_State *L)->int
	{
		getCurrentScene()->draw();
		swapScene();
		return 0;
	});

	// love.quit
	setCFunction(L, "quit", [](lua_State *L)->int
	{
		return 0;
	});

	// Get love.handlers now.
	lua_getfield(L, -1, "handlers");

	// love.focus
	setCFunction(L, "focus", [](lua_State *L)->int
	{
		getCurrentScene()->focus(lua_toboolean(L, 1));
		return 0;
	});

	// love.visible
	setCFunction(L, "visible", [](lua_State *L)->int
	{
		getCurrentScene()->visible(lua_toboolean(L, 1));
		return 0;
	});

	// love.resize
	setCFunction(L, "resize", [](lua_State *L)->int
	{
		getCurrentScene()->resize(luaL_checkinteger(L, 1), luaL_checkinteger(L, 2));
		return 0;
	});

	// love.keypressed
	setCFunction(L, "keypressed", [](lua_State *L)->int
	{
		love::keyboard::Keyboard::Key key;
		love::keyboard::Keyboard::Scancode sc;
		if (!love::keyboard::Keyboard::getConstant(luaL_checkstring(L, 1), key))
			return luaL_error(L, "invalid key");
		if (!love::keyboard::Keyboard::getConstant(luaL_checkstring(L, 2), sc))
			return luaL_error(L, "invalid scancode");

		getCurrentScene()->keyPressed(key, sc, lua_toboolean(L, 3));
		return 0;
	});

	// love.keyreleased
	setCFunction(L, "keyreleased", [](lua_State *L)->int
	{
		love::keyboard::Keyboard::Key key;
		love::keyboard::Keyboard::Scancode sc;
		if (!love::keyboard::Keyboard::getConstant(luaL_checkstring(L, 1), key))
			return luaL_error(L, "invalid key");
		if (!love::keyboard::Keyboard::getConstant(luaL_checkstring(L, 2), sc))
			return luaL_error(L, "invalid scancode");

		getCurrentScene()->keyReleased(key, sc);
		return 0;
	});

	// Remove love.handlers and love table
	lua_pop(L, 2);

	// Just send dummy table, lol
	lua_newtable(L);
	return 1;
}

int runLivesim4(int argc, char *argv[])
{
	// Open Lua state
	lua_State *L = luaL_newstate();
	luaL_openlibs(L);

	// We don't need JIT compiler because this game is mostly written in C++
	luaJIT_setmode(L, 0, LUAJIT_MODE_ENGINE | LUAJIT_MODE_OFF);
	LUAJIT_VERSION_SYM();

	// Preload LOVE and other libraries
	lua_getglobal(L, "package");
	lua_getfield(L, -1, "preload");
	lua_pushcfunction(L, &luaopen_love);
	lua_setfield(L, -2, "love");
	lua_pushcfunction(L, &luaopen_lvep);
	lua_setfield(L, -2, "lvep");
	lua_pushcfunction(L, &hijackHandlers);
	lua_setfield(L, -2, "livesim4");

	// Add command line arguments to global arg (like stand-alone Lua).
	{
		lua_newtable(L);
		if (argc > 0)
		{
			lua_pushstring(L, argv[0]);
			lua_rawseti(L, -2, -2);
		}
		
		lua_pushstring(L, "embedded boot.lua");
		lua_rawseti(L, -2, -1);

		lua_setglobal(L, "arg");
	}

	// require "love"
	lua_getglobal(L, "require");
	lua_pushstring(L, "love");
	lua_call(L, 1, 0);
	// Boot


	// Initialize scene
	livesim::base::initializeScene();

	// get debug.traceback
	lua_getglobal(L, "debug");
	lua_getfield(L, -1, "traceback");
	
	// require "love.boot" (preloaded when love was required)
	lua_getglobal(L, "require");
	lua_pushstring(L, "love.boot");
	lua_call(L, 1, 1);

	// Call the returned boot function.
	int status = lua_pcall(L, 0, 1, lua_gettop(L) - 1);
	int retval = 0;

	// If status is nonzero, that means there's error.
	if (status)
	{
		retval = 1;
		fprintf(stderr, "Lua Error: %s", lua_tostring(L, -1));
	}
	else
		// If the boot function return number, then
		// it should be the status code
		if (lua_isnumber(L, -1))
			retval = (int)lua_tonumber(L, -1);
		
	livesim::base::freeScene();
	lua_close(L);

	// Back control to main
	return retval;
}

int main(int argc, char *argv[])
{
	// Init PhysFS
	PHYSFS_init(argv[0]);
	
	// Open libav
	av_register_all();
	avcodec_register_all();

	// Since iOS is not allowed to exit explicitly
	// use this ifdef hack.
	try
	{
	#ifdef LOVE_IOS
		for(;;)
	#else
		return
	#endif
		runLivesim4(argc, argv);
		return 0;
	}
	catch (std::exception &e)
	{
		// I know we shouldn't exit under iOS, but this is exception!
		// (if you know the joke :P)
		fprintf(stderr, "Exception: %s\n", e.what());
		return 1;
	}
}
