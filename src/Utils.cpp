#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>

#include "Utils.hpp"
#include "Globals.hpp"

void scalePolygonToSprite(b2PolygonShape& polygon, const sf::Sprite& sprite) {

    // determine the scale factor
    b2Vec2 scaleFactor(
        sprite.getTextureRect().width * sprite.getScale().x * METERS_PER_PIXEL,
        sprite.getTextureRect().height * sprite.getScale().y * METERS_PER_PIXEL
    );

    // scale all the vertices in the polygon
    for (int i = 0; i < polygon.m_count; ++i) {
        polygon.m_vertices[i].x *= scaleFactor.x;
        polygon.m_vertices[i].y *= scaleFactor.y;
    }
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
