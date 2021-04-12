#include <iostream>
#include <memory>

#include <SFML/Graphics.hpp>

#include "ObstacleFactory.hpp"
#include "Obstacle.hpp"
#include "Globals.hpp"
#include "Utils.hpp"
#include "Resources/SpriteResource.hpp"
#include "Resources/PolygonResource.hpp"
#include "Resources/TextureResource.hpp"

std::shared_ptr<Obstacle> ObstacleFactory::makeStreetlight(const float& heightMeters,
        const bool& faceLeft) {

    // retrieve the streetlight sprite resource
    const SpriteResource& spriteResource =
            *resourceCache.getResource<SpriteResource>("STREETLIGHT_SPRITE");

    // create the streetlight obstacle
    std::shared_ptr<Obstacle> streetlight(new Obstacle(
        *spriteResource.sprite.getTexture(),
        sf::Vector2f((faceLeft ? -1.0f : 1.0f) * spriteResource.scaleFactor, spriteResource.scaleFactor),
        "STREETLIGHT"
    ));

    // get texture rectangles and rename them for convenience
    const sf::IntRect& baseRect = spriteResource.textureRects.at(0);
    const sf::IntRect& shaftRect = spriteResource.textureRects.at(1);
    const sf::IntRect& topRect = spriteResource.textureRects.at(2);

    // fixture definition which will associated with all added shapes
    b2FixtureDef fixtureDef;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.3f;

    // add the base
    sf::Vector2f baseOrigin(baseRect.width / 2.0f, baseRect.height);
    streetlight->addComponent(
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
        streetlight->addComponent(
            shaftRect,
            fixtureDef,
            {resourceCache.getResource<PolygonResource>("FULL_HITBOX")->polygon},
            -shaftOrigin
        );
    }

    // add the top
    sf::Vector2f topOrigin(shaftOrigin.x, topRect.height + shaftOrigin.y);
    streetlight->addComponent(
        topRect,
        fixtureDef,
        {
            resourceCache.getResource<PolygonResource>("STREETLIGHT_TOP_HITBOX_1")->polygon,
            resourceCache.getResource<PolygonResource>("STREETLIGHT_TOP_HITBOX_2")->polygon,
            resourceCache.getResource<PolygonResource>("STREETLIGHT_TOP_HITBOX_3")->polygon
        },
        -topOrigin
    );

    // set the body definition
    b2BodyDef bodyDef;
    bodyDef.type = b2_kinematicBody;
    streetlight->setBodyDef(bodyDef);

    return streetlight;
}

std::shared_ptr<Obstacle> ObstacleFactory::makeGround(const float& widthMeters) {

    // get the ground's sprite resource
    const SpriteResource& spriteResource =
            *resourceCache.getResource<SpriteResource>("TEST_GROUND_SPRITE");
    
    // determine scale such that the ground's width will be correct
    const sf::IntRect& textureRect = spriteResource.textureRects.at(0);
    float scale = widthMeters / (textureRect.width * spriteResource.scaleFactor * METERS_PER_PIXEL);
    
    // create the ground obstacle
    std::shared_ptr<Obstacle> ground(new Obstacle(
        *spriteResource.sprite.getTexture(),
        scale,
        "GROUND"
    ));

    // fixture definition which will be associated with all added shapes
    b2FixtureDef fixtureDef;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.5f;

    // ground only has one component; it's origin should be at the top right
    sf::Vector2f origin(textureRect.width, 0.0f);
    ground->addComponent(
        textureRect,
        fixtureDef,
        {resourceCache.getResource<PolygonResource>("FULL_HITBOX")->polygon},
        -origin
    );

    // set the body definition
    b2BodyDef bodyDef;
    bodyDef.type = b2_kinematicBody;
    ground->setBodyDef(bodyDef);

    return ground;
}

std::shared_ptr<Obstacle> ObstacleFactory::makePoop(const float& yVelocity) {

    // get the poop's sprite resource
    const SpriteResource& spriteResource =
            *resourceCache.getResource<SpriteResource>("TEST_POOP_SPRITE");
    
    // make the poop obstacle
    std::shared_ptr<Obstacle> poop(new Obstacle(
        *spriteResource.sprite.getTexture(),
        spriteResource.scaleFactor,
        "POOP"
    ));

    // fixture definition to applied to all added shapes
    b2FixtureDef fixtureDef;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 1.0f;

    // poop only has one component, origin should be in the middle
    const sf::IntRect& textureRect = spriteResource.textureRects.at(0);
    sf::Vector2f baseOrigin(textureRect.width, textureRect.height);
    poop->addComponent(
        textureRect,
        fixtureDef,
        {resourceCache.getResource<PolygonResource>("FULL_HITBOX")->polygon},
        -baseOrigin
    );

    // set the body definition
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.bullet = true;
    bodyDef.linearVelocity.y = yVelocity;
    poop->setBodyDef(bodyDef);

    return poop;
}
