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
        "../data/beach-background.gif"
    );

    loadTextureResource(
        "STREETLIGHT_TEXTURE",
        "../data/streetlight_texture.png"
    );

    loadTextureResource(
        "GROUND_TEXTURE",
        "../data/ground_texture.png" 
    );

    loadTextureResource(
        "NPC_TEXTURE",
        "../data/tempNPCtexture.png"
    );

    loadTextureResource(
        "TREE_TEXTURE",
        "../data/tree_texture.png"
    );

    loadTextureResource(
        "CLOUD_TEXTURE",
        "../data/cloud_texture.png"
    );
    loadTextureResource(
        "POOP_TEXTURE",
        "../data/poop_texture.png"
    );

    loadTextureResource(
        "POOP_SPLATTER_TEXTURE",
        "../data/poop_splatter_texture.png"
    );

    loadTextureResource(
        "LIFEGUARD_TEXTURE",
        "../data/lifeguard.png"
    );

    loadTextureResource(
        "DOCKS_TEXTURE",
        "../data/docks.png"
    );
    // SPRITES /////////////////////////////////////////////

    loadSpriteResource(
        "BIRD_SPRITE",
        *getResource<TextureResource>("BIRD_TEXTURE"),
        {
            { 0,  0, 16, 16}, //  0 dead
            {16,  0, 16, 16}, //  1 standing -- tall
            {32,  0, 16, 16}, //  2     medium height
            {48,  0, 16, 16}, //  3     low
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
        "POOP_SPRITE",
        *getResource<TextureResource>("POOP_TEXTURE"),
        {{0, 0, 31, 41}},
        1.0f
    );

    loadSpriteResource(
        "NPC_SPRITE",
        *getResource<TextureResource>("NPC_TEXTURE"),
        {
            {10, 18, 57, 65} // 0 idle still
        },
        1.5f
    );

    loadSpriteResource(
        "TREE_SPRITE",
        *getResource<TextureResource>("TREE_TEXTURE"),
        {
            {38, 65, 25, 9},
            {46, 49, 19, 12},
            {3, 13, 72, 31}
        },
        4.0f
    );

    loadSpriteResource(
        "CLOUD_SPRITE",
        *getResource<TextureResource>("CLOUD_TEXTURE"),
        {
            {3, 4, 27, 14}
        },
        4.0f
    );

    loadSpriteResource(
        "SPLATTER_SPRITE",
        *getResource<TextureResource>("POOP_SPLATTER_TEXTURE"),
        {
            {2, 12, 43, 9}
        },
        1.0f
    );

    loadSpriteResource(
        "LIFEGUARD_SPRITE",
        *getResource<TextureResource>("LIFEGUARD_TEXTURE"),
        {
            {0, 0, 109, 56}
        },
        2.0f
    );

    loadSpriteResource(
        "DOCKS_SPRITE",
        *getResource<TextureResource>("DOCKS_TEXTURE"),
        {
            {4, 6, 47, 20},
            {58, 6, 44, 20},
            {119, 6, 51, 20},
            {4, 30, 47, 16},
            {58, 30, 44, 16},
            {119, 30, 48, 16}
        },
        2.0f
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
