#include <cassert>
#include <iostream>

#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>

#include "PhysicalActor.hpp"
#include "NPC.hpp"
#include "Globals.hpp"
#include "Utils.hpp"
#include "Resources/SpriteResource.hpp"
#include "Resources/PolygonResource.hpp"

NPC::NPC() :

    // call super constructor with NPC type
    PhysicalActor(PhysicalActor::TYPE::NPC),

    //Default settings for an entity
    _initialized(false),

    //Frame rate settings
    _frameChangeTimer(0.0f),
    _FRAME_CHANGE_TIME_DELTA(0.5f),
    _WIDTH_METERS(2),
    _HEIGHT_METERS(2),
    _WIDTH_PIXELS(_WIDTH_METERS * PIXELS_PER_METER),
    _HEIGHT_PIXELS(_HEIGHT_METERS * PIXELS_PER_METER),

    //The entity is "live" or not hit by default, even if it's not drawable
    isHit(false)
{}

void NPC::init() {

     // get the sprite and texture rectangles
    const SpriteResource& spriteResource =
            *resourceCache.getResource<SpriteResource>("NPC_SPRITE");
    _NPCsprite = spriteResource.sprite;
    _textureRects = spriteResource.textureRects;

    // set origin to the bottom middle
    _NPCsprite.setOrigin(_textureRects.at(0).width / 2.0f, (float)_textureRects.at(0).height);

    // put at graphical position (0, 0) so transformations work as intended
    _NPCsprite.setPosition(0.0f, 0.0f);

    // scale the sprite based on the resource's scaleFactor
    _NPCsprite.scale(spriteResource.scaleFactor, spriteResource.scaleFactor);

    // body definition -- make it have fixed rotation so the NPC is always upright
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.fixedRotation = true;
    setBodyDef(bodyDef);

    // shape definition
    // TODO: This is only temporarily a rectangular hitbox. Change this eventually to a better
    // fitting hitbox for the torso.
    b2PolygonShape torso = resourceCache.getResource<PolygonResource>("FULL_HITBOX")->polygon;
    fitPolygonToSprite(torso, _NPCsprite);
    addShape(torso);

    // fixture definition
    b2FixtureDef fixtureDef;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.5f;
    addFixtureDef(fixtureDef);

    _initialized = true;

}
//Call an update of the NPC
void NPC::update(const float& timeDelta) {

    assert(_initialized);

    //TODO: Add check for if the npc has been hit and display death frame

    // Advance the frame change timer and increment the current frame if it exceeds
    // _FRAME_CHANGE_TIME_DELTA
    _frameChangeTimer += timeDelta;
    if (_frameChangeTimer >= _FRAME_CHANGE_TIME_DELTA) {
        _spriteCurrentFrame = 0;
        _NPCsprite.setTextureRect(_textureRects.at(_spriteCurrentFrame));
        _frameChangeTimer = 0.0f;
    }

    //logic to increment movement time
    if (isMoving){
        _timeMoving += timeDelta;
    }

}

void NPC::draw(sf::RenderTarget& target, sf::RenderStates states) const {

    assert(_initialized);
    
    // draw sprite
    target.draw(_NPCsprite, states);
}

//trigger the moving right animation set
void NPC::moveRight() {
    assert(_initialized);
    //TODO: Flip the sprite to face the other way.
    std::cout << "moveRight Start" << std::endl;
    
}

//trigger the animation for throwing
void NPC::triggerAction() {
    assert(_initialized);
    //TODO trigger action animation for sprite
    std::cout << "triggerAction Start" << std::endl;
}
