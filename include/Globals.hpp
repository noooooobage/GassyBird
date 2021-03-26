#ifndef _GLOBALS_HPP_
#define _GLOBALS_HPP_

/**
 * Contains global constants and objects.
 */

#include <SFML/Graphics.hpp>

#include "ResourceCache.hpp"
#include "EventMessenger.hpp"

// if the program was compiled in DEBUG mode or not
#ifdef NDEBUG
const bool DEBUG = false;
#else // NDEBUG
const bool DEBUG = true;
#endif // NDEBUG

// The native resolution of the game screen. The aspect ratio of this resolution should not change
// when the window is resized.
const sf::Vector2i NATIVE_RESOLUTION(1200, 600);
const float NATIVE_ASPECT_RATIO = NATIVE_RESOLUTION.x / NATIVE_RESOLUTION.y;

// For converting between physical units (meters) and graphical units (pixels). This is needed
// because the physics engine does not work in pixel units; instead it works in meters.
const float PIXELS_PER_METER = 50;
const float METERS_PER_PIXEL = 1.0f / PIXELS_PER_METER;

const float PI = 3.14159265359f;

// global objects
extern ResourceCache resourceCache;
extern EventMessenger eventMessenger;

#endif // _GLOBALS_HPP_
