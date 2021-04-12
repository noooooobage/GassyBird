#include <cassert>

#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>

#include "Utils.hpp"
#include "Globals.hpp"

int randomInt(int low, int high) {
    std::uniform_int_distribution<int> dist(low, high);
    return dist(rng);
}

float randomFloat(float low, float high) {
    std::uniform_real_distribution<float> dist(low, high);
    return dist(rng);
}

void translatePolygon(b2PolygonShape& polygon, const b2Vec2& translation) {
    for (int i = 0; i < polygon.m_count; ++i) {
        polygon.m_vertices[i] += translation;
    }
}

bool randomBool() {
    return randomInt(0, 1) == 0;
}

void scalePolygon(b2PolygonShape& polygon, const b2Vec2& scale) {

    // scale vertices
    for (int i = 0; i < polygon.m_count; ++i) {
        polygon.m_vertices[i].x *= scale.x;
        polygon.m_vertices[i].y *= scale.y;
    }

    // If the scale flipped the polygon about the x xor y axis (not both), then need to reset the
    // hull of the polygon so that its vertices remain in counter-clockwise order.
    bool isFlipped = scale.x * scale.y < 0.0f;
    if (isFlipped)
        polygon.Set(polygon.m_vertices, polygon.m_count);

    // sanity check -- make absolutely sure that the polygon's vertices are in CCW order
    assert(polygon.Validate());
}

void fitPolygonToSprite(b2PolygonShape& polygon, const sf::Sprite& sprite) {

    // translate the polygon in normalized coordinates to the sprite's origin
    translatePolygon(polygon, b2Vec2(
        -(sprite.getOrigin().x / sprite.getTextureRect().width - 0.5f),
        sprite.getOrigin().y / sprite.getTextureRect().height - 0.5f
    ));

    // scale the polygon so it fits around the sprite
    scalePolygon(polygon, b2Vec2(
        sprite.getTextureRect().width * sprite.getScale().x * METERS_PER_PIXEL,
        sprite.getTextureRect().height * sprite.getScale().y * METERS_PER_PIXEL
    ));
}

void centerTextOnPoint(sf::Text& text, const sf::Vector2f& point) {

    sf::FloatRect bounds = text.getLocalBounds();

    text.setOrigin(bounds.left + bounds.width / 2.0f, bounds.top + bounds.height / 2.0f);
    text.setPosition(point);
}

void centerTextOnRect(sf::Text& text, const sf::FloatRect& rect) {
    // get the center point of the rectange, then just center on that point
    sf::Vector2f point(rect.left + rect.width / 2.0f, rect.top + rect.height / 2.0f);
    centerTextOnPoint(text, point);
}
