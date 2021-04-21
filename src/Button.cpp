#include <iostream>
#include <string>

#include <SFML/Graphics.hpp>

#include "Button.hpp"
#include "Globals.hpp"
#include "Utils.hpp"
#include "Resources/FontResource.hpp"

Button::Button() :

    _opacity(1.0f),

    _BASE_TEXT_COLOR(sf::Color::White),
    _HOVERED_TEXT_COLOR(sf::Color::Black),
    _CLICKED_TEXT_COLOR(sf::Color::Black),
    _BASE_FILL_COLOR(sf::Color(0, 0, 0, 25)),
    _HOVERED_FILL_COLOR(sf::Color(255, 255, 255, 125)),
    _CLICKED_FILL_COLOR(sf::Color::White),
    _BASE_OUTLINE_THICKNESS(2.0f),
    _HOVERED_OUTLINE_THICKNESS(5.0f)
{
    setString("PLACEHOLDER TEXT");
    setSize(sf::Vector2f(200.0f, 100.0f));
    setPosition(sf::Vector2f(0.0f, 0.0f));

    // start out at the base state
    toBase();
}

void Button::setFont(const FontResource& fontResource) {
    _text.setFont(fontResource.font);
    centerTextOnRect(_text, _shape.getGlobalBounds());
}

void Button::setString(const std::string& string) {
    _text.setString(string);
    centerTextOnRect(_text, _shape.getGlobalBounds());
}

void Button::setCharacterHeight(const int& height) {
    _text.setCharacterSize(height);
    centerTextOnRect(_text, _shape.getGlobalBounds());
}

void Button::setSize(const sf::Vector2f& size) {
    _shape.setSize(size);
    centerTextOnRect(_text, _shape.getGlobalBounds());
}

void Button::setPosition(const sf::Vector2f& position) {
    _shape.setPosition(position);
    centerTextOnRect(_text, _shape.getGlobalBounds());
}

void Button::setOpacity(const float& opacity) {

    _opacity = opacity;

    _shape.setFillColor(sf::Color(
        _opaqueShapeColor.r,
        _opaqueShapeColor.g,
        _opaqueShapeColor.b,
        clamp((int)(_opacity * _opaqueShapeColor.a), 0, 255)
    ));

    _shape.setOutlineColor(sf::Color(255, 255, 255, clamp((int)(_opacity * 255), 0, 255)));

    _text.setFillColor(sf::Color(
        _opaqueTextColor.r,
        _opaqueTextColor.g,
        _opaqueTextColor.b,
        clamp((int)(_opacity * _opaqueTextColor.a), 0, 255)
    ));
}

bool Button::contains(const sf::Vector2f& point) {
    return _shape.getGlobalBounds().contains(point);
}

void Button::toBase() {

    _opaqueShapeColor = _BASE_FILL_COLOR;
    _opaqueTextColor = _BASE_TEXT_COLOR;
    
    _shape.setOutlineThickness(_BASE_OUTLINE_THICKNESS);

    setOpacity(_opacity);
}

void Button::toHovered() {

    _opaqueShapeColor = _HOVERED_FILL_COLOR;
    _opaqueTextColor = _HOVERED_TEXT_COLOR;
    
    _shape.setOutlineThickness(_HOVERED_OUTLINE_THICKNESS);

    setOpacity(_opacity);
}

void Button::toClicked() {

    _opaqueShapeColor = _CLICKED_FILL_COLOR;
    _opaqueTextColor = _CLICKED_TEXT_COLOR;
    
    _shape.setOutlineThickness(_HOVERED_OUTLINE_THICKNESS);

    setOpacity(_opacity);
}

void Button::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(_shape);
    target.draw(_text);
}
