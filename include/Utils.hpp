#ifndef _UTILS_HPP_
#define _UTILS_HPP_

#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>

#include "Globals.hpp"

// Note: Simple math functions and conversion functions get the inline. Other functions do not.

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
 * Converts an sf::Vector2f to a b2Vec2.
 */
inline b2Vec2 sfTob2Vector(const sf::Vector2f& vector) {
    return b2Vec2(vector.x, vector.y);
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

/**
 * Translate the given polygon by the specified amount.
 */
void translatePolygon(b2PolygonShape& polygon, const b2Vec2& translation);

/**
 * Scales the given polygon by the given scale.
 */
void scalePolygon(b2PolygonShape& polygon, const b2Vec2& scale);

/**
 * Translates and scales the given polygon such that it fits around the given sprite. Takes the 
 * sprite's origin and scale properties into account. For this to work correctly, the polygon's
 * vertices must be in normalized coordinates, i.e. (0, 0) is the center of the sprite, (0.5, 0.5)
 * is the top-right corner, and (-0.5, -0.5) is the bottom-left corner.
 */
void fitPolygonToSprite(b2PolygonShape& polygon, const sf::Sprite& sprite);

/**
 * Centers the given text on the given point.
 */
void centerTextOnPoint(sf::Text& text, const sf::Vector2f& point);

/**
 * Centers the given text inside the given rectangle.
 */
void centerTextOnRect(sf::Text& text, const sf::FloatRect& rect);

#endif // _UTILS_HPP_
