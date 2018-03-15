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

#ifndef LIVESIM_SCENE_H
#define LIVESIM_SCENE_H

// stdlib
#include <cstdint>
#include <cstdlib>
#include <string>

// love
#include "common/Object.h"
#include "lovewrap/SafeGuard.h"
#include "modules/keyboard/Keyboard.h"
#include "modules/mouse/Mouse.h"

namespace livesim
{
namespace base
{

/* Base class of LOVE scene function */
class Scene: public love::Object
{
public:
	/* Initialize scene */
	Scene(void *args);
	/* Free scene resource */
	virtual ~Scene();
	/* This is basically your drawing function. */
	virtual void draw();
	/**
	 * This is basically your update function.
	 *
	 * @param deltaT Elapsed time of previous frame.
	 */
	virtual void update(double deltaT);
	/**
	 * On window focus
	 *
	 * @param f Window is on focus?
	 */
	virtual void focus(bool f);
	/**
	 * On window visible
	 *
	 * @param v Window is visible?
	 */
	virtual void visible(bool v);
	/**
	 * On window resize
	 *
	 * @param w New window width.
	 * @param h New window height.
	 */
	virtual void resize(uint32_t w, uint32_t h);
	/**
	 * On key pressed
	 *
	 * @param key Character of the pressed key.
	 * @param scancode The scancode representing the pressed key.
	 * @param repeat Whether this keypress event is a repeat. The delay between key repeats depends on the user's system settings.
	 */
	virtual void keyPressed(love::keyboard::Keyboard::Key key, love::keyboard::Keyboard::Scancode scancode, bool repeat);
	/**
	 * On key released
	 *
	 * @param key Character of the pressed key.
	 * @param scancode The scancode representing the pressed key.
	 */
	virtual void keyReleased(love::keyboard::Keyboard::Key key, love::keyboard::Keyboard::Scancode scancode);
	/**
	 * On text input
	 *
	 * @param text The UTF-8 encoded unicode text.
	 */
	virtual void textInput(std::string text);
	/**
	 * On mouse key pressed
	 *
	 * @param x Mouse x position, in pixels.
	 * @param y Mouse y position, in pixels.
	 * @param button The button index that was pressed. 1 is the primary mouse button, 2 is the secondary mouse button
	 *               and 3 is the middle button. Further buttons are mouse dependent.
	 * @param istouch True if the mouse button press originated from a touchscreen touch-press.
	 */
	virtual void mousePressed(int32_t x, int32_t y, int32_t button, bool istouch);
	/**
	 * On mouse key released
	 *
	 * @param x Mouse x position, in pixels.
	 * @param y Mouse y position, in pixels.
	 * @param button The button index that was pressed. 1 is the primary mouse button, 2 is the secondary mouse button
	 *               and 3 is the middle button. Further buttons are mouse dependent.
	 * @param istouch True if the mouse button press originated from a touchscreen touch-press.
	 */
	virtual void mouseReleased(int32_t x, int32_t y, int32_t button, bool istouch);
	/**
	 * On mouse moved
	 *
	 * @param x The mouse position on the x-axis.
	 * @param y The mouse position on the y-axis.
	 * @param dx The amount moved along the x-axis since the last time love.mousemoved was called.
	 * @param dy The amount moved along the y-axis since the last time love.mousemoved was called.
	 * @param istouch True if the mouse button press originated from a touchscreen touch-press.
	 */
	virtual void mouseMoved(int32_t x, int32_t y, int32_t dx, int32_t dy, bool istouch);
	/**
	 * On mouse focus
	 *
	 * @param f Whether the window has mouse focus or not.
	 */
	virtual void mouseFocus(bool focus);
};

/* Initialize scene system */
void initializeScene();
/* Unitialize scene system */
void freeScene();

/**
 * Get current scene pointer
 *
 * @return Current scene pointer. New one always created if none is loaded before.
 */
Scene *getCurrentScene();
/**
 * Queue new scene loading.
 *
 * @param scene The scene object to queue.
 */
void loadScene(Scene *scene);
/* Swap scene. Should not be called directly! */
void swapScene();

} // base
} // livesim

#endif
