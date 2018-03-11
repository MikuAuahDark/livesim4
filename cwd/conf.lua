local love = require("love")
function love.conf(t)
	t.identity              = "livesim4"
	t.version               = "0.10.2"
	t.console               = false
	t.accelerometerjoystick = false
	t.externalstorage       = true
	t.gammacorrect          = false
	
	t.window.title          = "FiLive!"
	t.window.icon           = nil
	t.window.width          = 960
	t.window.height         = 640
	t.window.borderless     = false
	t.window.resizable      = true
	t.window.minwidth       = 320
	t.window.minheight      = 240
	t.window.fullscreen     = love._os == "iOS"
	t.window.fullscreentype = "desktop"
	t.window.vsync          = true
	t.window.msaa           = 0
	t.window.display        = 1
	t.window.highdpi        = false
	t.window.x              = nil
	t.window.y              = nil
	
	t.modules.joystick      = false
	t.modules.physics       = false
end
