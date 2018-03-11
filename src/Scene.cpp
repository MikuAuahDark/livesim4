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
#include <algorithm>
#include <type_traits>
#include <vector>

// Scene.h
#include "Scene.h"

#define DUMMY_FUNC(f, ...) void livesim::base::Scene::##f(__VA_ARGS__) {}

livesim::base::Scene::Scene(void *) {}
livesim::base::Scene::~Scene() {}

// Lazy mode
DUMMY_FUNC(draw, void)
DUMMY_FUNC(update, double)
DUMMY_FUNC(focus, bool)
DUMMY_FUNC(visible, bool)
DUMMY_FUNC(resize, uint32_t, uint32_t)
DUMMY_FUNC(keyPressed, love::keyboard::Keyboard::Key, love::keyboard::Keyboard::Scancode, bool)
DUMMY_FUNC(keyReleased, love::keyboard::Keyboard::Key, love::keyboard::Keyboard::Scancode)
DUMMY_FUNC(textInput, std::string)
DUMMY_FUNC(mousePressed, int32_t, int32_t, int32_t, bool)
DUMMY_FUNC(mouseReleased, int32_t, int32_t, int32_t, bool)
DUMMY_FUNC(mouseMoved, int32_t, int32_t, int32_t, int32_t, bool)
DUMMY_FUNC(mouseFocus, bool)

namespace livesim
{
namespace base
{

// Current scene.
Scene *currentScene;
// Queued scene.
Scene *queuedScene;

void initializeScene()
{
	currentScene = nullptr;
	queuedScene = nullptr;
}

void freeScene()
{
	// Free current scene
	getCurrentScene()->release();

	if (queuedScene)
		queuedScene->release();
}

Scene *getCurrentScene()
{
	if (currentScene == nullptr)
		currentScene = new Scene(nullptr);

	return currentScene;
}

void loadScene(Scene *scene)
{
	// If there's previously loaded scene, unload that one
	if (queuedScene)
		queuedScene->release();

	queuedScene = scene;
}

void swapScene()
{
	// Usually called just before love.draw returns.
	if (queuedScene)
	{
		getCurrentScene()->release();
		currentScene = queuedScene;
		queuedScene = nullptr;
	}
}

} // base
} // livesim
