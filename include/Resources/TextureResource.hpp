#ifndef _TEXTURE_RESOURCE_HPP_
#define _TEXTURE_RESOURCE_HPP_

#include <memory>

#include <SFML/Graphics.hpp>

#include "Resource.hpp"

/**
 * Stores an SFML Texture.
 */
class TextureResource : public Resource {

public:

    TextureResource (const sf::Texture& texture) : texture(texture) {}

    const ResourceType& getType() const override { return TYPE; }

    static const ResourceType TYPE;
    
    const sf::Texture texture;
};

#endif // _TEXTURE_RESOURCE_HPP_
