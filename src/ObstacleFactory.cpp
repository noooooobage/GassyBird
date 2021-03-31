#include <SFML/Graphics.hpp>
#include <iostream>
#include "ObstacleFactory.hpp"
#include "Obstacle.hpp"
#include "Ground.hpp"
#include "Globals.hpp"
#include "Utils.hpp"
#include "Resources/SpriteResource.hpp"
#include "Resources/PolygonResource.hpp"
#include "Resources/TextureResource.hpp"

Obstacle ObstacleFactory::makeStreetlight(const float& heightMeters, const bool& faceLeft) {

    // retrieve the streetlight sprite resource and create the initial obstacle from its texture
    const SpriteResource& spriteResource =
            *resourceCache.getResource<SpriteResource>("STREETLIGHT_SPRITE");
    Obstacle streetlight(*spriteResource.sprite.getTexture(),
            sf::Vector2f((faceLeft ? -1.0f : 1.0f) * spriteResource.scaleFactor,
            spriteResource.scaleFactor));

    // get texture rectangles and rename them for convenience
    const sf::IntRect& baseRect = spriteResource.textureRects.at(0);
    const sf::IntRect& shaftRect = spriteResource.textureRects.at(1);
    const sf::IntRect& topRect = spriteResource.textureRects.at(2);

    // fixture definition which will associated with all added shapes
    b2FixtureDef fixtureDef;

    // add the base
    sf::Vector2f baseOrigin(baseRect.width / 2.0f, baseRect.height);
    streetlight.addComponent(
        baseRect,
        fixtureDef,
        {resourceCache.getResource<PolygonResource>("STREETLIGHT_BASE_HITBOX")->polygon},
        -baseOrigin
    );

    // determine how many times the shaft should appear
    int numShafts = (heightMeters * PIXELS_PER_METER / spriteResource.scaleFactor -
            baseRect.height - topRect.height) / shaftRect.height;
    if (numShafts < 1)
        numShafts = 1;

    // add repeating shaft
    sf::Vector2f shaftOrigin(shaftRect.width / 2.0f, baseOrigin.y);
    for (int i = 0; i < numShafts; ++i) {
        shaftOrigin.y += shaftRect.height;
        streetlight.addComponent(
            shaftRect,
            fixtureDef,
            {resourceCache.getResource<PolygonResource>("FULL_HITBOX")->polygon},
            -shaftOrigin
        );
    }

    // add the top
    sf::Vector2f topOrigin(shaftOrigin.x, topRect.height + shaftOrigin.y);
    streetlight.addComponent(
        topRect,
        fixtureDef,
        {
            resourceCache.getResource<PolygonResource>("STREETLIGHT_TOP_HITBOX_1")->polygon,
            resourceCache.getResource<PolygonResource>("STREETLIGHT_TOP_HITBOX_2")->polygon,
            resourceCache.getResource<PolygonResource>("STREETLIGHT_TOP_HITBOX_3")->polygon
        },
        -topOrigin
    );

    // add body definition
    b2BodyDef bodyDef;
    streetlight.setBodyDef(bodyDef);

    return streetlight;
}

Obstacle ObstacleFactory::makeGround() {
    const SpriteResource& spriteResource =
            *resourceCache.getResource<SpriteResource>("TEST_GROUND_SPRITE");
    Obstacle ground(*spriteResource.sprite.getTexture(), sf::Vector2f(spriteResource.scaleFactor, 1.0f));
    const sf::IntRect& baseRect = spriteResource.textureRects.at(0);
    float width = spriteResource.textureRects.at(0).width;
    float height = spriteResource.textureRects.at(0).height;
    std::cout << width << std::endl;
    b2FixtureDef fixtureDef;
    sf::Vector2f baseOrigin(0.0f, baseRect.height);
    std::cout << baseOrigin.x << " " << baseOrigin.y << std::endl;
    ground.addComponent(
        baseRect,
        fixtureDef,
        {resourceCache.getResource<PolygonResource>("FULL_HITBOX")->polygon},
        -baseOrigin
    );
    sf::Vector2f repeatOrigin(-baseRect.width, baseRect.height);
    ground.addComponent(
        baseRect,
        fixtureDef,
        {resourceCache.getResource<PolygonResource>("FULL_HITBOX")->polygon},
        -repeatOrigin
    );
    repeatOrigin.x -= baseRect.width;
    ground.addComponent(
        baseRect,
        fixtureDef,
        {resourceCache.getResource<PolygonResource>("FULL_HITBOX")->polygon},
        -repeatOrigin
    );
    b2BodyDef bodyDef;

    ground.setBodyDef(bodyDef);
    return ground;
}

