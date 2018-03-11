// STL
#include <iostream>
#include <fstream>

// Lua
#include <lua.hpp>

// LOVE
#include <common/runtime.h>

// LVEP
#include "LVEPVideoStream.h"
#include "LVEPDecoder.h"

int w_newVideoStream(lua_State *L)
{
	auto file = love::luax_checktype<love::filesystem::File>(L, 1, love::FILESYSTEM_FILE_ID);
	love::luax_catchexcept(L, [&]() {
		love::video::VideoStream *stream = new LVEPVideoStream(file);
		luax_pushtype(L, love::VIDEO_VIDEO_STREAM_ID, stream);
		stream->release();
	});
	return 1;
}

int w_newDecoder(lua_State *L)
{
	auto file = love::luax_checktype<love::filesystem::File>(L, 1, love::FILESYSTEM_FILE_ID);
	love::luax_catchexcept(L, [&]() {
		love::sound::Decoder *decoder = new LVEPDecoder(file);
		luax_pushtype(L, love::SOUND_DECODER_ID, decoder);
		decoder->release();
	});
	return 1;
}

extern "C" int luaopen_lvep(lua_State *L)
{
	lua_newtable(L);
	lua_pushcfunction(L, w_newVideoStream);
	lua_setfield(L, -2, "newVideoStream");
	lua_pushcfunction(L, w_newDecoder);
	lua_setfield(L, -2, "newDecoder");
	return 1;
}
