#include <cassert>
#include <memory>
#include <vector>
#include <unordered_map>
#include <string>
#include <type_traits>
#include <iostream>

#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>

#include "Globals.hpp"
#include "ResourceCache.hpp"
#include "Resource.hpp"
#include "Resources/TextureResource.hpp"
#include "Resources/SpriteResource.hpp"
#include "Resources/FontResource.hpp"
#include "Resources/PolygonResource.hpp"

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

    
    // TEXTURES ////////////////////////////////////////////
    
    loadTextureResource(
        "BIRD_TEXTURE",
        "../data/bird_texture.png"
    );

    loadTextureResource(
        "BEACH_BACKGROUND_TEXTURE",
        "../data/beach-background-large.gif"
    );

    // SPRITES /////////////////////////////////////////////

    loadSpriteResource(
        "BIRD_SPRITE",
        *getResource<TextureResource>("BIRD_TEXTURE"),
        {
            { 0,  0, 16, 16}, //  0 standing -- tall
            {16,  0, 16, 16}, //  1     medium height
            {32,  0, 16, 16}, //  2     low
            {48,  0, 16, 16}, //  3     very low
            {64,  0, 16, 16}, //  4 pecking
            { 0, 16, 16, 16}, //  5 flying mouth closed -- wings down
            {16, 16, 16, 16}, //  6     wings down-middle
            {32, 16, 16, 16}, //  7     wings middle
            {48, 16, 16, 16}, //  8     wings up-middle
            {64, 16, 16, 16}, //  9     wings up
            { 0, 32, 16, 16}, // 10 flying mouth open -- wings down
            {16, 32, 16, 16}, // 11     wings down-middle
            {32, 32, 16, 16}, // 12     wings middle
            {48, 32, 16, 16}, // 13     wings up-middle
            {64, 32, 16, 16}  // 14     wings up
        },
        75.0f // width in pixels
    );

    loadSpriteResource(
        "BEACH_BACKGROUND_SPRITE",
        *getResource<TextureResource>("BEACH_BACKGROUND_TEXTURE"),
        {{0, 0, 1200, 600}},
        1200.0f
    );

    // FONTS ///////////////////////////////////////////////

    loadFontResource("ARCADE_FONT", "../data/ARCADECLASSIC.ttf");

    // POLYGONS ////////////////////////////////////////////

    loadPolygonResource(
        "BIRD_HITBOX",
        {
            { 7.5f / 16,  0.5f / 16},
            { 4.5f / 16,  3.5f / 16},
            { 1.5f / 16,  3.5f / 16},
            {-7.5f / 16, -0.5f / 16},
            {-7.5f / 16, -1.5f / 16},
            {-4.5f / 16, -3.5f / 16},
            { 1.5f / 16, -3.5f / 16}
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
        const TextureResource& textureResource, const std::vector<sf::IntRect>& textureRects,
        const float& widthPixels) {

    // create underlying sprite
    sf::Sprite sprite;
    sprite.setTexture(textureResource.texture);

    // Make sure that there is at least one rectangle in textureRects, and set the sprite's
    // texture rectangle to the first one.
    assert(textureRects.size() > 0);
    sprite.setTextureRect(textureRects.at(0));

    // make sure a resource with the id does not already exist, then make the resource
    assert(_resources.find(id) == _resources.end());
    _resources[id] = std::make_shared<SpriteResource>(sprite, textureRects, widthPixels);
}

void ResourceCache::loadFontResource(const std::string& id, const std::string& filename) {
    sf::Font font;

    font.loadFromFile(filename);

    assert(_resources.find(id) == _resources.end());
    _resources[id] = std::make_shared<FontResource>(font);
}

void ResourceCache::loadPolygonResource(const std::string& id,
        const std::vector<b2Vec2>& vertices) {

    // make sure there are at least 3 vertices, then make the polygon shape
    assert(vertices.size() >= 3);
    b2PolygonShape polygon;
    polygon.Set(vertices.data(), vertices.size());

    // make sure that the polygon is convex
    assert(polygon.Validate());

    // make sure a resource with the id doesn't already exist, then make the resource
    assert(_resources.find(id) == _resources.end());
    _resources[id] = std::make_shared<PolygonResource>(polygon);
}
