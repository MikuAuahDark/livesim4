// This is equivalent to love.conf
// Edit if necessary. Default is supplied.
#include "Boot.h"

GameWindowSettings ws = {
	// fullscreen
	false,
	// fstype
	love::window::Window::FULLSCREEN_DESKTOP,
	// vsync
	1,
	// msaa
	0,
	// stencil
	true,
	// depth
	0,
	// resizable
	true,
	// minwidth
	1,
	// minheight
	1,
	// borderless
	false,
	// centered
	true,
	// display
	0,
	// highdpi
	false,
	// refreshrate: keep this 0!
	0.0,
	// useposition, x, y
	false, 0, 0,
	// width,height
	960, 640,
	// title
	"livesim4",
	// icon
	""
};

// It must be named "conf"
GameConfiguration conf = {
	// identity
	"livesim4",
	// appendidentity
	true,
	// accelerometerjoystick
	false,
	// externalstorage
	true,
	// gammacorrect
	false,
	// mixwithsystem
	true,
	// window. Set to `nullptr` to defer window creation.
	&ws,
	// modules
	// Modules which are always unconditionally enabled:
	// love.data
	// love.filesystem
	// love.thread
	std::vector<std::string>({
		"audio", "data", "event", "font", "graphics", "image",
		"joystick", "keyboard", "math", "mouse", "physics", "sound",
		"timer", "touch", "video", "window"
	})
};
