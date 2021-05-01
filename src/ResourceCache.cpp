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
        "TEST_TEXTURE",
        "../data/test_texture.png"
    );

    loadTextureResource(
        "BIRD_TEXTURE",
        "../data/bird_texture.png"
    );

    loadTextureResource(
        "BEACH_BACKGROUND_TEXTURE",
        "../data/beach-background-redone.png"
    );

    loadTextureResource(
        "STREETLIGHT_TEXTURE",
        "../data/streetlight_texture.png"
    );

    loadTextureResource(
        "GROUND_TEXTURE",
        "../data/simple-sand.png"
    );

    loadTextureResource(
        "NPC_TEXTURE",
        "../data/tempNPCtexture.png"
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
        60.0f / 16 // scale so that the final width is 60 pixels
    );

    loadSpriteResource(
        "BEACH_BACKGROUND_SPRITE",
        *getResource<TextureResource>("BEACH_BACKGROUND_TEXTURE"),
        {{0, 0, 200, 100}},
        NATIVE_RESOLUTION.x / 200.0f
    );

    loadSpriteResource(
        "TEST_GROUND_SPRITE",
        *getResource<TextureResource>("GROUND_TEXTURE"),
        {{0, 0, 400, 100}},
        1.0f
    );

    loadSpriteResource(
        "STREETLIGHT_SPRITE",
        *getResource<TextureResource>("STREETLIGHT_TEXTURE"),
        {
            {0, 16, 26, 7}, // base
            {9,  9,  8, 7}, // shaft
            {9,  0, 40, 9}  // tip ;)
        },
        3.5f
    );

    loadSpriteResource(
        "TEST_POOP_SPRITE",
        *getResource<TextureResource>("TEST_TEXTURE"),
        {{7, 0, 7, 7}},
        2.5f
    );

    loadSpriteResource(
        "NPC_SPRITE",
        *getResource<TextureResource>("NPC_TEXTURE"),
        {
            {10, 18, 57, 65} // 0 idle still
        },
        1.5f
    );

    // FONTS ///////////////////////////////////////////////

    loadFontResource("ARCADE_FONT", "../data/ARCADECLASSIC.ttf");

    loadFontResource("JOYSTIX_FONT", "../data/joystix.monospace.ttf");

    // POLYGONS ////////////////////////////////////////////

    // this hitbox will always encompass the full texture recangle in a rectangle shape
    loadPolygonResource(
        "FULL_HITBOX",
        {
            { 0.5f,  0.5f},
            {-0.5f,  0.5f},
            {-0.5f, -0.5f},
            { 0.5f, -0.5f}
        }
    );

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

    loadPolygonResource(
        "STREETLIGHT_BASE_HITBOX",
        {
            {  4.5f / 26,  3.0f / 7},
            {- 4.5f / 26,  3.0f / 7},
            {-12.5f / 26, -3.0f / 7},
            { 12.5f / 26, -3.0f / 7}
        }
    );

    loadPolygonResource(
        "STREETLIGHT_TOP_HITBOX_1",
        {
            {-12.5f / 40,  4.0f / 9},
            {-17.5f / 40,  4.0f / 9},
            {-19.5f / 40,  2.0f / 9},
            {-19.5f / 40, -4.0f / 9},
            {-12.5f / 40, -4.0f / 9}
        }
    );

    loadPolygonResource(
        "STREETLIGHT_TOP_HITBOX_2",
        {
            { 17.5f / 40,  0.0f / 9},
            {  5.5f / 40,  4.0f / 9},
            {-12.5f / 40,  4.0f / 9},
            {-12.5f / 40, -1.0f / 9},
            {  3.5f / 40, -1.0f / 9}
        }
    );

    loadPolygonResource(
        "STREETLIGHT_TOP_HITBOX_3",
        {
            {17.5f / 40,  0.0f / 9},
            { 3.5f / 40, -1.0f / 9},
            { 3.5f / 40, -4.0f / 9},
            {19.5f / 40, -4.0f / 9},
            {19.5f / 40, -2.0f / 9}
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
        const float& scaleFactor) {

    // create underlying sprite
    sf::Sprite sprite;
    sprite.setTexture(textureResource.texture);

    // Make sure that there is at least one rectangle in textureRects, and set the sprite's
    // texture rectangle to the first one.
    assert(textureRects.size() > 0);
    sprite.setTextureRect(textureRects.at(0));
    // sprite.setOrigin(textureRects.at(0).width/2.0f, textureRects.at(0).height/2.0f);
    // make sure a resource with the id does not already exist, then make the resource
    assert(_resources.find(id) == _resources.end());
    _resources[id] = std::make_shared<SpriteResource>(sprite, textureRects, scaleFactor);
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
