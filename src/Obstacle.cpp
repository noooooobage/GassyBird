#include <cassert>
#include <array>
#include <list>
#include <iostream>

#include <SFML/Graphics.hpp>

#include "Obstacle.hpp"
#include "Utils.hpp"

Obstacle::Obstacle(const sf::Texture& texture, const sf::Vector2f& scale, const std::string& name) :
    _TEXTURE(texture),
    _SCALE(scale),
    _NAME(name)
{
    _vertices.setPrimitiveType(sf::Quads);
}

Obstacle::Obstacle(const sf::Texture& texture, const float& scaleFactor, const std::string& name) :
    _TEXTURE(texture),
    _SCALE(sf::Vector2f(scaleFactor, scaleFactor)),
    _NAME(name)
{
    _vertices.setPrimitiveType(sf::Quads);
}

void Obstacle::addComponent(const sf::IntRect& textureRect, const b2FixtureDef& fixtureDef,
        std::list<b2PolygonShape> polygons, const sf::Vector2f& translation) {

    // create vertices encompassing texture rectangle
    std::array<sf::Vertex, 4> vertices = {
        sf::Vertex( // top-left
            sf::Vector2f(0.0f, 0.0f),
            sf::Color::White,
            sf::Vector2f(textureRect.left, textureRect.top)
        ), sf::Vertex( // top-right
            sf::Vector2f(textureRect.width, 0.0f),
            sf::Color::White,
            sf::Vector2f(textureRect.left + textureRect.width, textureRect.top)
        ), sf::Vertex( // bottom-right
            sf::Vector2f(textureRect.width, textureRect.height),
            sf::Color::White,
            sf::Vector2f(textureRect.left + textureRect.width, textureRect.top + textureRect.height)
        ), sf::Vertex( // bottom-left
            sf::Vector2f(0.0f, textureRect.height),
            sf::Color::White,
            sf::Vector2f(textureRect.left, textureRect.top + textureRect.height)
        )
    };

    // translate and scale vertices, then add them to internal vertex array
    for (sf::Vertex& vertex : vertices) {

        vertex.position += translation;

        vertex.position.x *= _SCALE.x;
        vertex.position.y *= _SCALE.y;

        _vertices.append(vertex);
    }

    // determine normalized physical translation
    b2Vec2 normalizedTranslation(
        0.5f + translation.x / textureRect.width,
        -(0.5f + translation.y / textureRect.height)
    );

    // determine scale such that the polygon is scaled correctly in meters
    sf::Vector2f scale = sf::Vector2f(
        textureRect.width * _SCALE.x,
        textureRect.height * _SCALE.y
    ) * METERS_PER_PIXEL;

    // apply translation and scale to all polygons, then add them as shapes and add fixture def
    for (b2PolygonShape& polygon : polygons) {
        translatePolygon(polygon, normalizedTranslation);
        scalePolygon(polygon, sfTob2Vector(scale));
        addShape(polygon);
        addFixtureDef(fixtureDef);
    }
}

void Obstacle::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    states.texture = &_TEXTURE;
    target.draw(_vertices, states);
}
