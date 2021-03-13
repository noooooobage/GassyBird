#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>

#include "Utils.hpp"
#include "Globals.hpp"

sf::Vector2f physicalToGraphicalCoords(const b2Vec2& physical) {
    return sf::Vector2f(
        physical.x * PIXELS_PER_METER,
        NATIVE_RESOLUTION.y - physical.y * PIXELS_PER_METER
    );
}
