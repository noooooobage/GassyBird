#ifndef _FONT_RESOURCE_HPP_
#define _FONT_RESOURCE_HPP_

#include <SFML/Graphics.hpp>

#include "Resource.hpp"

/**
 * Stores an SFML Sprite along with a vector of texture rectangles denoting the different "frames"
 * that the sprite can have. If the sprite is not animated, then the length of this vector is 1. If
 * the sprite is animated, then the length is > 1.
 */
class FontResource : public Resource {

public:

    FontResource(const sf::Font ft) : font(ft) {}

    const ResourceType& getType() const override { return TYPE; }

    static const ResourceType TYPE;
    
    sf::Font font;
};

#endif // _FONT_RESOURCE_HPP
