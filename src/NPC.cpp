#include <cassert>
#include <iostream>

#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>

#include "NPC.hpp"
#include "Globals.hpp"
#include "Utils.hpp"
#include "Resources/SpriteResource.hpp"
#include "Resources/PolygonResource.hpp"

NPC::NPC() :

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

    //This section defines the default body of an NPC using box2d
{}

void NPC::init() {

     // get the sprite and texture rectangles
    const SpriteResource& spriteResource =
            *resourceCache.getResource<SpriteResource>("NPC_SPRITE");
    _NPCsprite = spriteResource.sprite;
    _textureRects = spriteResource.textureRects;

    // set origin to geometric center (based on texture rect width in pixels)
    float originalPixelWidth = _textureRects.at(0).width;
    _NPCsprite.setOrigin(originalPixelWidth / 2.0f, originalPixelWidth / 2.0f);

    // put at graphical position (0, 0) so transformations work as intended
    _NPCsprite.setPosition(0.0f, 0.0f);

    // scale the sprite based on the resource's scaleFactor
    _NPCsprite.scale(spriteResource.scaleFactor, spriteResource.scaleFactor);


    // body definition
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.linearVelocity.Set(-2.0f, 0.0f);
    bodyDef.angularVelocity = 0.0f;
    bodyDef.gravityScale = 0.0f;
    setBodyDef(bodyDef);

    // shape definition
    b2PolygonShape torso;
    torso.SetAsBox(_WIDTH_METERS / 2.0f, _HEIGHT_METERS / 2.0f);
    addShape(torso);

    // fixture definition
    b2FixtureDef fixtureDef;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.5f;
    addFixtureDef(fixtureDef);

    // shape definition

    //Comment extra fixtures out until we get the bug under control
    
    /**
    b2PolygonShape torso;
    torso.SetAsBox(_WIDTH_METERS / 2.0f, _HEIGHT_METERS / 2.0f);
    addShape(torso);

    // fixture definition
    b2FixtureDef fixtureDef1;
    fixtureDef1.density = 0.0f;
    fixtureDef1.friction = 0.5f;
    addFixtureDef(fixtureDef1);
    */

    //_top = fixtureDef;
    //_bottom = fixtureDef1;

    _initialized = true;

}
//Call an update of the NPC
void NPC::update(const float& timeDelta) {

    assert(_initialized);

    // Advance the frame change timer and increment the current frame if it exceeds
    // _FRAME_CHANGE_TIME_DELTA
    _frameChangeTimer += timeDelta;
    if (_frameChangeTimer >= _FRAME_CHANGE_TIME_DELTA) {
        _spriteCurrentFrame = 0;
        _NPCsprite.setTextureRect(_textureRects.at(_spriteCurrentFrame));
        _frameChangeTimer = 0.0f;
    }
}

void NPC::draw(sf::RenderTarget& target, sf::RenderStates states) const {

    assert(_initialized);
    
    // draw sprite
    target.draw(_NPCsprite, states);
}

