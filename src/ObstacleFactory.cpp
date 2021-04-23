#include <iostream>
#include <memory>

#include <SFML/Graphics.hpp>

#include "ObstacleFactory.hpp"
#include "PhysicalActor.hpp"
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
        PhysicalActor::TYPE::GENERIC_OBSTACLE,
        *spriteResource.sprite.getTexture(),
        sf::Vector2f((faceLeft ? -1.0f : 1.0f) * spriteResource.scaleFactor, spriteResource.scaleFactor)
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
        PhysicalActor::TYPE::GROUND,
        *spriteResource.sprite.getTexture(),
        scale
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
            *resourceCache.getResource<SpriteResource>("POOP_SPRITE");
    
    // make the poop obstacle
    std::shared_ptr<Obstacle> poop(new Obstacle(
        PhysicalActor::TYPE::POOP,
        *spriteResource.sprite.getTexture(),
        spriteResource.scaleFactor
    ));

    // fixture definition to applied to all added shapes
    b2FixtureDef fixtureDef;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 1.0f;

    // poop only has one component, origin should be in the middle
    const sf::IntRect& textureRect = spriteResource.textureRects.at(0);
    sf::Vector2f origin(textureRect.width / 2.0f, textureRect.height / 2.0f);
    poop->addComponent(
        textureRect,
        fixtureDef,
        {resourceCache.getResource<PolygonResource>("FULL_HITBOX")->polygon},
        -origin
    );

    // set the body definition
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.bullet = true;
    bodyDef.angle = PI / 4.0f;
    bodyDef.linearVelocity.y = yVelocity;
    poop->setBodyDef(bodyDef);

    return poop;
}

std::shared_ptr<Obstacle> ObstacleFactory::makePoopSplatter() {
    //functions similarly to a ground obstacle but with a different texture
        // get the ground's sprite resource
    const SpriteResource& spriteResource =
            *resourceCache.getResource<SpriteResource>("SPLATTER_SPRITE");
    
    // determine scale such that the ground's width will be correct
    const sf::IntRect& textureRect = spriteResource.textureRects.at(0);
    // create the ground obstacle
    std::shared_ptr<Obstacle> ground(new Obstacle(
        PhysicalActor::TYPE::GROUND,
        *spriteResource.sprite.getTexture(),
        spriteResource.scaleFactor
    ));

    b2FixtureDef fixtureDef;
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
    bodyDef.type = b2_dynamicBody;
    ground->setBodyDef(bodyDef);

    return ground;
}

std::shared_ptr<Obstacle> ObstacleFactory::makeTree(const float& heightMeters, const bool& faceLeft) {
    const SpriteResource& spriteResource =
            *resourceCache.getResource<SpriteResource>("TREE_SPRITE");
    std::shared_ptr<Obstacle> tree(new Obstacle(
        PhysicalActor::TYPE::GENERIC_OBSTACLE,
        *spriteResource.sprite.getTexture(),
        spriteResource.scaleFactor
    ));
    const sf::IntRect& baseRect = spriteResource.textureRects.at(0);
    const sf::IntRect& shaftRect = spriteResource.textureRects.at(1);
    const sf::IntRect& topRect = spriteResource.textureRects.at(2);

    b2FixtureDef fixtureDef;

    sf::Vector2f baseOrigin(baseRect.width / 2.0f, baseRect.height);
    tree->addComponent(
        baseRect,
        fixtureDef,
        {resourceCache.getResource<PolygonResource>("STREETLIGHT_BASE_HITBOX")->polygon},
        -baseOrigin
    );
    int numShafts = (heightMeters * PIXELS_PER_METER / spriteResource.scaleFactor -
            baseRect.height - topRect.height) / shaftRect.height;
    if (numShafts < 1)
        numShafts = 1;
        
    sf::Vector2f bodyOrigin(baseOrigin.x, baseOrigin.y);

    for (int i = 0; i < numShafts; ++i) {
        bodyOrigin.y += shaftRect.height;
        bodyOrigin.x -= 8;
        tree->addComponent(
            shaftRect,
            fixtureDef,
            {resourceCache.getResource<PolygonResource>("FULL_HITBOX")->polygon},
            -bodyOrigin
        );
    }
    sf::Vector2f topOrigin(bodyOrigin.x+7, bodyOrigin.y+topRect.height-2);
    
    tree->addComponent(
        topRect,
        fixtureDef,
        {resourceCache.getResource<PolygonResource>("FULL_HITBOX")->polygon},
        -topOrigin
    );

    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    tree->setBodyDef(bodyDef);
    return tree;
}

std::shared_ptr<Obstacle> ObstacleFactory::makeCloud() {
    const SpriteResource& spriteResource =
            *resourceCache.getResource<SpriteResource>("CLOUD_SPRITE");
    std::shared_ptr<Obstacle> cloud(new Obstacle(
        PhysicalActor::TYPE::GENERIC_OBSTACLE,
        *spriteResource.sprite.getTexture(),
        spriteResource.scaleFactor
    ));

    b2FixtureDef fixtureDef;
    const sf::IntRect& textureRect = spriteResource.textureRects.at(0);
    sf::Vector2f origin(textureRect.width / 2.0f, textureRect.height / 2.0f);
    cloud->addComponent(
        textureRect,
        fixtureDef,
        {resourceCache.getResource<PolygonResource>("STREETLIGHT_BASE_HITBOX")->polygon},
        -origin
    );
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;

    cloud->setBodyDef(bodyDef);
    return cloud;
}

std::shared_ptr<Obstacle> ObstacleFactory::makeDocks(const int& widthMeters, const int& heightMeters) {
    const SpriteResource& spriteResource =
            *resourceCache.getResource<SpriteResource>("DOCKS_SPRITE");
    std::shared_ptr<Obstacle> docks(new Obstacle(
        PhysicalActor::TYPE::GENERIC_OBSTACLE,
        *spriteResource.sprite.getTexture(),
        spriteResource.scaleFactor
    ));
    const sf::IntRect& leftTopRect = spriteResource.textureRects.at(0);
    const sf::IntRect& middleTopRect = spriteResource.textureRects.at(1);
    const sf::IntRect& rightTopRect = spriteResource.textureRects.at(2);
    const sf::IntRect& leftBottomRect = spriteResource.textureRects.at(3);
    const sf::IntRect& middleBottomRect = spriteResource.textureRects.at(4);
    const sf::IntRect& rightBottomRect = spriteResource.textureRects.at(5);

    b2FixtureDef fixtureDef;
    float width = rightBottomRect.width;
    float height = rightBottomRect.height;
    for (int j = 0; j < heightMeters; j++) {
        sf::Vector2f leftBottom(width / 2.0f, height);
        docks->addComponent(
            rightBottomRect,
            fixtureDef,
            {},
            -leftBottom
        );
        height += leftBottomRect.height;
    }
    
    sf::Vector2f rightTop(width / 2.0f, height);
    docks->addComponent(
        rightTopRect,
        fixtureDef,
        {resourceCache.getResource<PolygonResource>("FULL_HITBOX")->polygon},
        -rightTop
    );
    width += rightBottomRect.width - 6;
    height = middleBottomRect.height;
    for (int i = 1; i < widthMeters-1; i++) {
        for (int j = 0; j < heightMeters; j++) {
            sf::Vector2f rightBottom(width - (rightBottomRect.width / 2.0f), height);
            docks->addComponent(
                middleBottomRect,
                fixtureDef,
                {},
                -rightBottom
            );
            height += middleBottomRect.height;
        }

        sf::Vector2f middleTop(width - (middleTopRect.width / 2.0f), height);
        docks->addComponent(
            middleTopRect,
            fixtureDef,
            {resourceCache.getResource<PolygonResource>("FULL_HITBOX")->polygon},
            -middleTop
        );
        width += middleBottomRect.width;
        height = middleBottomRect.height;
    }
    for (int j = 0; j < heightMeters; j++) {
        sf::Vector2f leftBottom(width - (leftBottomRect.width / 2.0f), height);
        docks->addComponent(
            leftBottomRect,
            fixtureDef,
            {},
            -leftBottom
        );
        height += leftBottomRect.height;
    }
    sf::Vector2f leftTop(width - (leftTopRect.width / 2.0f), height);
    docks->addComponent(
        leftTopRect,
        fixtureDef,
        {resourceCache.getResource<PolygonResource>("FULL_HITBOX")->polygon},
        -leftTop
    );


    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;

    docks->setBodyDef(bodyDef);
    return docks;
}
std::shared_ptr<Obstacle> ObstacleFactory::makeLifeguard(const bool& faceLeft) {
    const SpriteResource& spriteResource =
            *resourceCache.getResource<SpriteResource>("LIFEGUARD_SPRITE");
    std::shared_ptr<Obstacle> lifeguard(new Obstacle(
        PhysicalActor::TYPE::GENERIC_OBSTACLE,
        *spriteResource.sprite.getTexture(),
        sf::Vector2f((faceLeft ? -1.0f : 1.0f) * spriteResource.scaleFactor, spriteResource.scaleFactor)
    ));

    const sf::IntRect& textureRect = spriteResource.textureRects.at(0);

    b2FixtureDef fixtureDef;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.5f;

    sf::Vector2f origin(textureRect.width / 2.0f, textureRect.height);

    lifeguard->addComponent(
        textureRect,
        fixtureDef,
        {resourceCache.getResource<PolygonResource>("FULL_HITBOX")->polygon},
        -origin
    );

    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;

    lifeguard->setBodyDef(bodyDef);
    return lifeguard;
}

std::shared_ptr<Obstacle> ObstacleFactory::makeRock(float tAngle){
    const SpriteResource& spriteResource =
        *resourceCache.getResource<SpriteResource>("ROCK_SPRITE");
    
    std::shared_ptr<Obstacle> rock(new Obstacle(
        PhysicalActor::TYPE::PROJECTILE,*spriteResource.sprite.getTexture(),
        spriteResource.scaleFactor
    ));

    // fixture definition to applied to all added shapes
    b2FixtureDef fixtureDef;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 1.0f;

    // rock also only has one component, origin should be in the middle
    const sf::IntRect& textureRect = spriteResource.textureRects.at(0);
    sf::Vector2f origin(textureRect.width / 2.0f, textureRect.height / 2.0f);
    rock->addComponent(
        textureRect,
        fixtureDef,
        {resourceCache.getResource<PolygonResource>("FULL_HITBOX")->polygon},
        -origin
    );

    // set the body definition
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.gravityScale = (0.5f);
    bodyDef.bullet = true;
    bodyDef.angle = tAngle;
    bodyDef.linearVelocity.y = 2.f;
    rock->setBodyDef(bodyDef);

    return rock;

}