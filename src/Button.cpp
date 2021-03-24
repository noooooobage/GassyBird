#include "Button.hpp"
#include <iostream>
#include "Globals.hpp"

Button::Button() {

}

Button::Button(int minX, int maxX, int minY, int maxY) {
    minXClickWindow = minX;
    maxXClickWindow = maxX;
    minYClickWindow = minY;
    maxYClickWindow = maxY;
}

Button::Button(const Button& b) {
    *this = b;
}

void Button::onClick() {

}

void Button::draw(sf::RenderTarget& target) const {
    int x = maxXClickWindow - minXClickWindow;
    int y = maxYClickWindow - minYClickWindow;
    sf::RectangleShape rect(sf::Vector2f(x, y));
    rect.setFillColor(sf::Color::White);
    rect.setOrigin(x / 2, y / 2);
    rect.setPosition(sf::Vector2f(NATIVE_RESOLUTION.x/2, NATIVE_RESOLUTION.y/2));
    target.draw(rect);
    sf::Text t;
}

bool Button::isInsideBounds(int x, int y) {
    if(x >= minXClickWindow && x <= maxXClickWindow && y >= minYClickWindow && y <= maxYClickWindow) {
        return true;
    } else {
        return false;
    }
}