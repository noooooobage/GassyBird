#ifndef _SPRITE_RESOURCE_HPP_
#define _SPRITE_RESOURCE_HPP_

#include <memory>
#include <vector>

#include <SFML/Graphics.hpp>

#include "Resource.hpp"

/**
 * Stores an SFML Sprite along with a vector of texture rectangles denoting the different "frames"
 * that the sprite can have. If the sprite is not animated, then the length of this vector is 1. If
 * the sprite is animated, then the length is > 1. Also stores the sacle factor of the sprite.
 */
class SpriteResource : public Resource {

public:

    SpriteResource(
        const sf::Sprite& sprite, 
        const std::vector<sf::IntRect>& textureRects,
        const float& scaleFactor
    ) :
        sprite(sprite),
        textureRects(textureRects),
        scaleFactor(scaleFactor)
    {}

    const ResourceType& getType() const override { return TYPE; }

    static const ResourceType TYPE;
    
    const sf::Sprite sprite;
    const std::vector<sf::IntRect> textureRects;
    const float scaleFactor; // how many pixels wide the sprite should appear on screen
};

#endif // _SPRITE_RESOURCE_HPP
