#ifndef _DEBUG_DRAWER_HPP_
#define _DEBUG_DRAWER_HPP_

#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>

/**
 * Used by box2d for debug drawing, i.e. draws the exact positions of hitboxes, centers of mass,
 * etc.
 */
class DebugDrawer : public b2Draw {

public:

    /**
     * Default constructor.
     */
    DebugDrawer();

    /**
     * Initializes with a render target.
     */
    void init(sf::RenderTarget& target);

    /**
     * Draws a closed polygon, with vertices provided in CCW order.
     */
	void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override;

    /**
     * Draws a solid closed polygon, with vertices provided in CCW order.
     */
	void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override;

    /**
     * Draws a circle.
     */
	void DrawCircle(const b2Vec2& center, float radius, const b2Color& color);

    /**
     * Draws a solid circle.
     */
	void DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis,
            const b2Color& color) override;

    /**
     * Draws a line segment.
     */
	void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) override;

    /**
     * Draws a transform. Choose your own length scale.
     */
	void DrawTransform(const b2Transform& xf) override;

    /**
     * Draws a point.
     */
	void DrawPoint(const b2Vec2& p, float size, const b2Color& color) override;

private:

    bool _initialized;

    sf::RenderTarget* _renderTarget;

    const float _OUTLINE_THICKNESS;
};

#endif
