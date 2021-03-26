#include <cassert>
#include <iostream>

#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>

#include "DebugDrawer.hpp"
#include "Utils.hpp"

DebugDrawer::DebugDrawer() :
    _initialized(false),
    _renderTarget(nullptr),
    _OUTLINE_THICKNESS(2.0f)
{}

void DebugDrawer::init(sf::RenderTarget& target) {

    _renderTarget = &target;

    _initialized = true;
}

void DebugDrawer::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) {

    assert(_initialized);
    assert(_renderTarget);

    sf::ConvexShape polygon(vertexCount);
    for (int i = 0; i < vertexCount; ++i)
        polygon.setPoint(i, physicalToGraphicalCoord(vertices[i]));
    
    polygon.setFillColor(sf::Color::Transparent);
    polygon.setOutlineColor(b2ToSfColor(color));
    polygon.setOutlineThickness(_OUTLINE_THICKNESS);

    _renderTarget->draw(polygon);
}

void DebugDrawer::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount,
        const b2Color& color) {

    assert(_initialized);
    assert(_renderTarget);
    
    DrawPolygon(vertices, vertexCount, color);
}

void DebugDrawer::DrawCircle(const b2Vec2& center, float radius, const b2Color& color) {

    assert(_initialized);
    assert(_renderTarget);

    float radiusPixels = radius * PIXELS_PER_METER;

    sf::CircleShape circle(radiusPixels);
    circle.setOrigin(radiusPixels, radiusPixels);
    circle.setPosition(physicalToGraphicalCoord(center));

    circle.setFillColor(sf::Color::Transparent);
    circle.setOutlineColor(b2ToSfColor(color));
    circle.setOutlineThickness(_OUTLINE_THICKNESS);

    _renderTarget->draw(circle);
}

void DebugDrawer::DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis,
        const b2Color& color) {

    assert(_initialized);
    assert(_renderTarget);

    DrawCircle(center, radius, color);
    DrawSegment(center, center + radius * axis, color);
}

void DebugDrawer::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) {

    assert(_initialized);
    assert(_renderTarget);

    sf::ConvexShape line(4);
    line.setPoint(0, physicalToGraphicalCoord(p1));
    line.setPoint(1, line.getPoint(0));
    line.setPoint(2, physicalToGraphicalCoord(p2));
    line.setPoint(3, line.getPoint(2));

    line.setFillColor(sf::Color::Transparent);
    line.setOutlineColor(b2ToSfColor(color));
    line.setOutlineThickness(_OUTLINE_THICKNESS);

    _renderTarget->draw(line);
}

void DebugDrawer::DrawTransform(const b2Transform& xf) {

    assert(_initialized);
    assert(_renderTarget);

    DrawPoint(xf.p, _OUTLINE_THICKNESS * 2.0f, b2Color(1.0f, 0.0f, 0.0f, 1.0f));

    float lineLength = 1.0f; // meters
    b2Vec2 p2 = xf.p + lineLength * b2Vec2(xf.q.c, xf.q.s);
    DrawSegment(xf.p, p2, b2Color(1.0f, 0.0f, 0.0f, 1.0f));
}

void DebugDrawer::DrawPoint(const b2Vec2& p, float size, const b2Color& color) {

    assert(_initialized);
    assert(_renderTarget);

    sf::CircleShape point(size);
    point.setPosition(physicalToGraphicalCoord(p));

    point.setFillColor(b2ToSfColor(color));

    _renderTarget->draw(point);
}
