#ifndef _OBSTACLE_HPP_
#define _OBSTACLE_HPP_

#include <cassert>
#include <list>

#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>

#include "PhysicalActor.hpp"
#include "Resources/SpriteResource.hpp"

/**
 * An obstacle is something that does not move on its own in the world. You lose the game if you
 * collide with an obstacle.
 * 
 * Obstacles must be created with an ObstacleFactory method. These methods make sure that obstacles
 * with multiple components are created correctly.
 */
class Obstacle : public PhysicalActor {

public:

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    // only the ObstacleFactory is able to create Obstacles
    friend class ObstacleFactory;

    std::string getName() { return _NAME; };

private:

    /**
     * Creates an Obstacle from the given texture. All added components are scaled by scale.
     */
    Obstacle(const sf::Texture& texture, const sf::Vector2f& scale, const std::string& name);

    /**
     * Creates an Obstacle from the given texture. All added components are scaled by scaleFactor.
     */
    Obstacle(const sf::Texture& texture, const float& scaleFactor, const std::string& name);

    /**
     * Adds a component to the obstacle. A component has a visual component, denoted by a texture
     * rectangle in the texture; and a hitbox given by a polygon.
     * 
     * @param textureRect texture rectangle in the obstacle's texture 
     * @param fixtureDef a fixture definition which is associated with every given polygon
     * @param polygons list of polygon hitboxes given in normalized coordinates
     * @param translation visual vertices and hitboxes are translated by this amount, given in
     *     pixels in the texture
     */
    void addComponent(const sf::IntRect& textureRect, const b2FixtureDef& fixtureDef,
            std::list<b2PolygonShape> polygons, const sf::Vector2f& translation);

    const sf::Texture& _TEXTURE;
    const sf::Vector2f _SCALE;
    const std::string _NAME;
    sf::VertexArray _vertices;
};

#endif // _OBSTACLE_HPP_

