#include <cassert>
#include <iostream>

#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>

#include "PlayableBird.hpp"
#include "Globals.hpp"

PlayableBird::PlayableBird() :

    _spriteSet(false),
    _frameChangeTimer(0.0f),
    _FRAME_CHANGE_TIME_DELTA(0.05f),
    _WIDTH_METERS(1),
    _WIDTH_PIXELS(_WIDTH_METERS * PIXELS_PER_METER),

    isFlying(false)
{
    // body definition
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.linearVelocity.Set(0.0f, 0.0f);
    bodyDef.angularVelocity = 8.0f;
    setBodyDef(bodyDef);

    // shape definition
    b2PolygonShape box;
    box.SetAsBox(_WIDTH_METERS / 2.0f, _WIDTH_METERS / 2.0f);
    addShape(box);

    // fixture definition
    b2FixtureDef fixtureDef;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.5f;
    addFixtureDef(fixtureDef);
    poopsLeft = maxNumberOfPoops;
}

void PlayableBird::update(const float& timeDelta) {

    assert(_spriteSet);

    // Advance the frame change timer and increment the current frame if it exceeds
    // _FRAME_CHANGE_TIME_DELTA
    _frameChangeTimer += timeDelta;
    if (_frameChangeTimer >= _FRAME_CHANGE_TIME_DELTA) {
        _spriteCurrentFrame = (_spriteCurrentFrame + 1) % _textureRects.size();
        _sprite.setTextureRect(_textureRects.at(_spriteCurrentFrame));
        _frameChangeTimer = 0.0f;
    }
}

void PlayableBird::draw(sf::RenderTarget& target, sf::RenderStates states) const {

    assert(_spriteSet);
    
    // draw sprite
    target.draw(_sprite, states);
}

void PlayableBird::setSprite(const SpriteResource& spriteResource) {

    // get the sprite resource and make sure that it has at least one texture rectangle
    _sprite = spriteResource.sprite;
    _textureRects = spriteResource.textureRects;
    assert(spriteResource.textureRects.size() > 0);

    // set the current frame to the first texture rectangle
    _spriteCurrentFrame = 0;
    _sprite.setTextureRect(_textureRects.at(_spriteCurrentFrame));

    // set origin to geometric center (based on texture rect!)
    float originalPixelWidth = _textureRects.at(0).width;
    _sprite.setOrigin(originalPixelWidth / 2.0f, originalPixelWidth / 2.0f);

    // put at position (0, 0)
    _sprite.setPosition(0.0f, 0.0f);

    // determine scale based on width and the pixel size of the first texture rectangle
    float scaleFactor = _WIDTH_PIXELS / originalPixelWidth;
    _sprite.scale(scaleFactor, scaleFactor);

    _spriteSet = true;
}
