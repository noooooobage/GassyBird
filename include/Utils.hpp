#ifndef _UTILS_HPP_
#define _UTILS_HPP_

#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>

#include "Globals.hpp"

/**
 * Returns a transformation which converts the physical coordinate system to the graphical
 * coordinate system.
 */
inline sf::Transform physicalToGraphicalTransform(const b2Body& body) {

    sf::Transform transform;

    transform.translate(
        body.GetPosition().x * PIXELS_PER_METER,
        NATIVE_RESOLUTION.y - body.GetPosition().y * PIXELS_PER_METER
    ).rotate(-180.0f / PI * body.GetAngle());

    return transform;
}

#endif // _UTILS_HPP_
