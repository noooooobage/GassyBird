#ifndef _UTILS_HPP_
#define _UTILS_HPP_

#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>

/**
 * Converts physical coordinates (meters) to graphical coordinates (pixels).
 */
sf::Vector2f physicalToGraphicalCoords(const b2Vec2& physical);

#endif // _UTILS_HPP_
