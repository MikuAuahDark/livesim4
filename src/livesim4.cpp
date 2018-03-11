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

// stdlib
#include <cstdlib>

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

// love
#include "modules/love/love.h"

int runLivesim4(int argc, char *argv[])
{
	// Open Lua state
	LUAJIT_VERSION_SYM();
	lua_State *L = luaL_newstate();
	luaJIT_setmode(L, 0, LUAJIT_MODE_ENGINE | LUAJIT_MODE_OFF);
	luaL_openlibs(L);

	// Preload LOVE and other libraries
	lua_getglobal(L, "package");
	lua_getfield(L, -1, "preload");
	lua_pushcfunction(L, &luaopen_love);
	lua_setfield(L, -2, "love");

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

		for (int i = 1; i < argc; i++)
		{
			lua_pushstring(L, argv[i]);
			lua_rawseti(L, -2, i);
		}

		lua_setglobal(L, "arg");
	}

	// require "love"
	lua_getglobal(L, "require");
	lua_pushstring(L, "love");
	lua_call(L, 1, 0);
	
	// require "love.boot" (preloaded when love was required.)
	lua_getglobal(L, "require");
	lua_pushstring(L, "love.boot");
	lua_call(L, 1, 1);

	// Call the returned boot function.
	lua_call(L, 0, 1);

	// If the boot function return number, then
	// it should be the status code
	int retval = 0;
	if (lua_isnumber(L, -1))
		retval = (int)lua_tonumber(L, -1);

	lua_close(L);

	// Back control to main
	return retval;
}

int main(int argc, char *argv[])
{
	// Open libav
	av_register_all();
	avcodec_register_all();

	// Since iOS is not allowed to exit explicitly
	// use this ifdef hack.
#ifdef LOVE_IOS
	for(;;)
#else
	return
#endif
	runLivesim4(argc, argv);
	return 0;
}
