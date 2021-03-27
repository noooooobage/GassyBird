#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>

#include "Utils.hpp"
#include "Globals.hpp"

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
