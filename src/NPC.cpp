#include <cassert>
#include <iostream>

#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>

#include "NPC.hpp"
#include "Globals.hpp"

NPC::NPC() :

    //Default settings for an entity
    _spriteSet(false),

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
{
    // body definition
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.linearVelocity.Set(0.0f, 0.0f);
    bodyDef.angularVelocity = 0.0f;
    setBodyDef(bodyDef);

    // shape definition
    b2PolygonShape head;
    head.SetAsBox(_WIDTH_METERS / 2.0f, _WIDTH_METERS / 2.0f);
    addShape(head);

    // fixture definition
    b2FixtureDef fixtureDef;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.5f;
    addFixtureDef(fixtureDef);

    // shape definition
    b2PolygonShape torso;
    torso.SetAsBox(_WIDTH_METERS / 2.0f, _HEIGHT_METERS / 2.0f);
    addShape(torso);

    // fixture definition
    b2FixtureDef fixtureDef1;
    fixtureDef1.density = 1.0f;
    fixtureDef1.friction = 0.5f;
    addFixtureDef(fixtureDef1);


    _top = fixtureDef;
    _bottom = fixtureDef1;

}


//Call an update of the NPC
void NPC::update(const float& timeDelta) {

    assert(_spriteSet);

    // Advance the frame change timer and increment the current frame if it exceeds
    // _FRAME_CHANGE_TIME_DELTA
    _frameChangeTimer += timeDelta;
    if (_frameChangeTimer >= _FRAME_CHANGE_TIME_DELTA) {
        _spriteCurrentFrame = (_spriteCurrentFrame + 1) % _textureRect.size();
        _NPCsprite.setTextureRect(_textureRect.at(_spriteCurrentFrame));
        _frameChangeTimer = 0.0f;
    }
}

void NPC::draw(sf::RenderTarget& target, sf::RenderStates states) const {

    assert(_spriteSet);
    
    // draw sprite
    target.draw(_NPCsprite, states);
}

void NPC::setSprite(const SpriteResource& spriteResource) {

    // get the sprite resource and make sure that it has at least one texture rectangle
    _NPCsprite = spriteResource.sprite;
    _textureRect = spriteResource.textureRects;
    assert(spriteResource.textureRects.size() > 0);

    // set the current frame to the first texture rectangle
    _spriteCurrentFrame = 0;
    _NPCsprite.setTextureRect(_textureRect.at(_spriteCurrentFrame));

    // set origin to geometric center (based on texture rect!)
    float originalPixelWidth = _textureRect.at(0).width;
    _NPCsprite.setOrigin(originalPixelWidth / 2.0f, originalPixelWidth / 2.0f);

    // put at position (0, 0)
    _NPCsprite.setPosition(0.0f, 0.0f);

    // determine scale based on width and the pixel size of the first texture rectangle
    float scaleFactor = _WIDTH_PIXELS / originalPixelWidth;
    _NPCsprite.scale(scaleFactor, scaleFactor);

    _spriteSet = true;
}