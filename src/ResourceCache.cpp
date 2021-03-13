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
#include "Resources/SpriteResource.hpp"
#include "Resources/TextureResource.hpp"

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
    sf::Texture testTexture;
    testTexture.loadFromFile("../data/test_texture.png");
    _resources.insert({"TEST_TEXTURE", std::make_shared<TextureResource>(testTexture)});

    // load test bird sprite
    sf::Sprite testBirdSprite;
    testBirdSprite.setTexture(getResource<TextureResource>("TEST_TEXTURE")->texture);
    std::vector<sf::IntRect> textureRects = {
        {0,  0, 7, 7},
        {7,  0, 7, 7},
        {14, 0, 7, 7},
        {21, 0, 7, 7},
        {0,  7, 7, 7},
        {7,  7, 7, 7},
        {14, 7, 7, 7},
        {21, 7, 7, 7}
    };
    testBirdSprite.setTextureRect(textureRects.at(0));
    _resources.insert({"TEST_BIRD_SPRITE",
            std::make_shared<SpriteResource>(testBirdSprite, textureRects)});
}
