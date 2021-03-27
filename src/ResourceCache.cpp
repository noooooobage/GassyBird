#include <cassert>
#include <memory>
#include <vector>
#include <unordered_map>
#include <string>
#include <type_traits>
#include <iostream>

#include <SFML/Graphics.hpp>

#include "Globals.hpp"
#include "ResourceCache.hpp"
#include "Resource.hpp"
#include "Resources/TextureResource.hpp"
#include "Resources/SpriteResource.hpp"

ResourceCache::ResourceCache() :
    _initialized(false)
{}

ResourceCache::~ResourceCache() {
    // free memory owned by resources
    for (auto i : _resources)
        i.second.reset();
}

void ResourceCache::init() {

    _initialized = true;

    // load test texture
    loadTextureResource(
        "TEST_TEXTURE",
        "../data/test_texture.png"
    );

    loadTextureResource(
        "BEACH_BACKGROUND_TEXTURE",
        "../data/beach-background-large.gif"
    );

    // load test bird sprite
    loadSpriteResource(
        "TEST_BIRD_SPRITE",
        *getResource<TextureResource>("TEST_TEXTURE"),
        {
            {0,  0, 7, 7},
            {7,  0, 7, 7},
            {14, 0, 7, 7},
            {21, 0, 7, 7},
            {0,  7, 7, 7},
            {7,  7, 7, 7},
            {14, 7, 7, 7},
            {21, 7, 7, 7}
        }
    );

    loadSpriteResource(
        "BEACH_BACKGROUND_SPRITE",
        *getResource<TextureResource>("BEACH_BACKGROUND_TEXTURE"),
        {
            {0, 0, 1200, 1000}
        }
    );
}

void ResourceCache::loadTextureResource(const std::string& id, const std::string& filename) {

    // load the texture from the file
    sf::Texture texture;
    texture.loadFromFile(filename);

    // make sure a resource with the id does not already exist, then make the resource
    assert(_resources.find(id) == _resources.end());
    _resources[id] = std::make_shared<TextureResource>(texture);
}

void ResourceCache::loadSpriteResource(const std::string& id,
        const TextureResource& textureResource, const std::vector<sf::IntRect>& textureRects) {

    // create underlying sprite
    sf::Sprite sprite;
    sprite.setTexture(textureResource.texture);

    // Make sure that there is at least one rectangle in textureRects, and set the sprite's
    // texture rectangle to the first one.
    assert(textureRects.size() > 0);
    sprite.setTextureRect(textureRects.at(0));

    // make sure a resource with the id does not already exist, then make the resource
    assert(_resources.find(id) == _resources.end());
    _resources[id] = std::make_shared<SpriteResource>(sprite, textureRects);
}
