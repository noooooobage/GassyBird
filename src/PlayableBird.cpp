#include <cassert>
#include <iostream>

#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>

#include "PhysicalActor.hpp"
#include "PlayableBird.hpp"
#include "Globals.hpp"
#include "Utils.hpp"
#include "Resources/SpriteResource.hpp"
#include "Resources/PolygonResource.hpp"

PlayableBird::PlayableBird() :

    PhysicalActor(PhysicalActor::TYPE::PLAYABLE_BIRD),

    _initialized(false),

    _FLYING_CLOSED_START_FRAME(5),
    _FLYING_OPEN_START_FRAME(10),
    _FLYING_FRAMES({0, 1, 2, 3, 4, 2}),
    _FALLING_FRAME(2),
    _FLYING_FRAME_DURATION(0.04f),
    _currentFlyingFrame(0),
    _frameTimer(0.0f),

    _isFlying(false),
    _isPooping(false),
    _isDead(false)
{}

void PlayableBird::init() {

    // get the sprite and texture rectangles
    const SpriteResource& spriteResource =
            *resourceCache.getResource<SpriteResource>("BIRD_SPRITE");
    _sprite = spriteResource.sprite;
    _textureRects = spriteResource.textureRects;

    // set origin to geometric center (based on texture rect width in pixels)
    float originalPixelWidth = _textureRects.at(0).width;
    _sprite.setOrigin(originalPixelWidth / 2.0f, originalPixelWidth / 2.0f);

    // scale the sprite based on the resource's scaleFactor
    _sprite.scale(spriteResource.scaleFactor, spriteResource.scaleFactor);

    // body definition
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    setBodyDef(bodyDef);

    // shape definition -- just get the bird hitbox stored in the resource cache and scale it to fit
    // correctly around the sprite
    b2PolygonShape hitbox = resourceCache.getResource<PolygonResource>("BIRD_HITBOX")->polygon;
    fitPolygonToSprite(hitbox, _sprite);
    addShape(hitbox);

    // fixture definition
    b2FixtureDef fixtureDef;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.5f;
    fixtureDef.restitution = 0.2f;
    addFixtureDef(fixtureDef);

    _initialized = true;
}

void PlayableBird::update(const float& timeDelta) {

    assert(_initialized);

    // If the bird is flying, advance the frame change timer and increment the current frame if it
    // exceeds _FLYING_FRAME_DURATION
    if (_isFlying) {
        _frameTimer += timeDelta;
        if (_frameTimer >= _FLYING_FRAME_DURATION) {
            _currentFlyingFrame = (_currentFlyingFrame + 1) % _FLYING_FRAMES.size();
            _frameTimer = 0.0f;
        }
    }

    // only set the bird's frame if it isn't dead
    if (!_isDead) {

        // determine which starting frame to use; is either the start of the mouth closed flying
        // sequence or the start of the mouth open one
        int startFrame = _isPooping ? _FLYING_OPEN_START_FRAME : _FLYING_CLOSED_START_FRAME;

        // determine the actual frame and set the texture rectangle
        int frame = startFrame + _FLYING_FRAMES.at(_currentFlyingFrame);
        _sprite.setTextureRect(_textureRects.at(frame));
    }
}

void PlayableBird::draw(sf::RenderTarget& target, sf::RenderStates states) const {

    assert(_initialized);
    
    // draw sprite
    target.draw(_sprite, states);
}

void PlayableBird::startFlying() {

    // set the frame to where the wings are pointed slightly upward, so that the bird looks more
    // natural when it starts to flap its wings
    _currentFlyingFrame = _FLYING_FRAMES.at(3);

    // reset the frame timer
    _frameTimer = 0.0f;

    _isFlying = true;
    _isDead = false;
}

void PlayableBird::stopFlying() {

    // set the frame to where the wings are in the middle, so it's kind of like the wings are
    // "tucked in" while falling
    _currentFlyingFrame = _FLYING_FRAMES.at(2);

    _isFlying = false;
}

void PlayableBird::startPooping() {
    _isPooping = true;
    _isDead = false;
}

void PlayableBird::stopPooping() {
    _isPooping = false;
}

void PlayableBird::die() {

    // set the frame to the dead frame
    _sprite.setTextureRect(_textureRects.at(0));

    _isDead = true;
}
