#ifndef _UTILS_HPP_
#define _UTILS_HPP_

#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>

#include "Globals.hpp"

/**
 * Clamps a value to the range [low, high].
 */
template <typename T>
inline T clamp(const T& value, const T& low, const T& high) {

    if (value < low)
        return low;
    else if (value > high)
        return high;

    return value;
}

/**
 * Returns a transformation which converts the physical coordinate system to the graphical
 * coordinate system for a specific body.
 */
inline sf::Transform physicalToGraphicalTransform(const b2Body& body) {

    sf::Transform transform;

    transform.translate(
        body.GetPosition().x * PIXELS_PER_METER,
        NATIVE_RESOLUTION.y - body.GetPosition().y * PIXELS_PER_METER
    ).rotate(-180.0f / PI * body.GetAngle());

    return transform;
}

/**
 * Converts a point in the physical coordinate system to the graphical coordinate system, then
 * return the graphical point.
 */
inline sf::Vector2f physicalToGraphicalCoord(const b2Vec2& point) {
    return sf::Vector2f(
        point.x * PIXELS_PER_METER,
        NATIVE_RESOLUTION.y - point.y * PIXELS_PER_METER
    );
}

/**
 * Converts a b2Color to an sf::Color.
 */
inline sf::Color b2ToSfColor(const b2Color& color) {
    return sf::Color(
        clamp((int)(color.r * 255), 0, 255),
        clamp((int)(color.g * 255), 0, 255),
        clamp((int)(color.b * 255), 0, 255),
        clamp((int)(color.a * 255), 0, 255)
    );
}

#endif // _UTILS_HPP_
