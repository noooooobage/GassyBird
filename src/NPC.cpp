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

NPC::NPC(const NPC::TYPE& type) :

    // call super constructor
    PhysicalActor(PhysicalActor::TYPE::NPC),

    _TYPE(type),
    _initialized(false),
    _state(IDLE),

    _IDLE_FRAME_DURATION(0.2f),
    _IDLE_START_FRAME(0),
    _NUM_IDLE_FRAMES(4),

    _WALK_FRAME_DURATION(0.15f),
    _WALK_START_FRAME(4),
    _NUM_WALK_FRAMES(6),

    _THROW_FRAME_DURATION(0.1f),
    _START_THROW_START_FRAME(10),
    _NUM_START_THROW_FRAMES(3),
    _FINISH_THROW_START_FRAME(13),
    _NUM_FINISH_THROW_FRAMES(3),

    _frameDuration(1.0f),
    _startFrame(0),
    _numFrames(1),
    _currentFrame(0),
    _frameTimer(0.0f),

    _isFacingLeft(false),
    _isReadyToFinishThrowing(false),
    _actionTimeRemaining(0.0f),
    _prepareTimeRemaining(0.0f),
    _nextAction(ACTION::IDLE),
    _nextActionDuration(0.0f)
{}

void NPC::init() {

    // get the sprite and texture rectangles according to the type
    const SpriteResource* spriteResource;
    if (_TYPE == TYPE::MALE)
        spriteResource = resourceCache.getResource<SpriteResource>("NPC_MALE_SPRITE");
    else
        spriteResource = resourceCache.getResource<SpriteResource>("NPC_FEMALE_SPRITE");
    _sprite = spriteResource->sprite;
    _textureRects = spriteResource->textureRects;

    // set origin to the bottom middle
    _sprite.setOrigin(_textureRects.at(0).width / 2.0f, (float)_textureRects.at(0).height);

    // scale the sprite based on the resource's scaleFactor, then face the sprite to the left
    _sprite.scale(spriteResource->scaleFactor, spriteResource->scaleFactor);
    setFacingLeft(true);

    // body definition -- make it have fixed rotation so the NPC is always upright
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.fixedRotation = true;
    setBodyDef(bodyDef);

    // shape definitions
    b2PolygonShape body = resourceCache.getResource<PolygonResource>("NPC_HITBOX_BODY")->polygon;
    fitPolygonToSprite(body, _sprite);
    addShape(body);

    // fixture definition
    b2FixtureDef fixtureDef;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.5f;
    addFixtureDef(fixtureDef);

    // NPC should start out in the idle state
    toIdle();

    _initialized = true;
}

void NPC::update(const float& timeDelta) {

    assert(_initialized);

    //TODO: Add check for if the npc has been hit and display death frame

    // set the sprite to the current frame
    _sprite.setTextureRect(_textureRects.at(_currentFrame));

    // Increment the frame. Loop the animation only if the NPC isn't starting to throw.
    _frameTimer += timeDelta;
    _currentFrame = _startFrame + _frameTimer / _frameDuration;
    int endFrame = _startFrame + _numFrames - 1;
    if (_currentFrame > endFrame) {
        if (_state == STARTING_THROW)
            _currentFrame = endFrame;
        else
            _currentFrame = _startFrame + (_currentFrame - _startFrame) % _numFrames;
    }

    // Decrement the action timer, and if it reaches 0, then set the NPC to idle. Don't do this if
    // the NPC is starting to throw.
    _actionTimeRemaining -= timeDelta;
    if (_actionTimeRemaining <= 0.0f && _state != IDLE && _state != PREPARING) {
        _actionTimeRemaining = 0.0f;
        if (_state == STARTING_THROW)
            _isReadyToFinishThrowing = true;
        else
            toIdle();
    }

    // Decrement the prepare timer, and when it reaches 0 do the next action, unless that action is
    // idle.
    _prepareTimeRemaining -= timeDelta;
    if (_prepareTimeRemaining <= 0.0f && _nextAction != ACTION::IDLE) {

        _prepareTimeRemaining = 0.0f;

        if (_nextAction == ACTION::WALK)
            walk(_nextActionDuration);
        else if (_nextAction == ACTION::START_THROW)
            startThrowing(_nextActionDuration);

        _nextAction = ACTION::IDLE;
        _nextActionDuration = 0.0f;

    // set the state to PREPARING if we're waiting to do an action and the current state is IDLE
    } else if (_prepareTimeRemaining > 0.0f && _state == IDLE) {
        _state = PREPARING;
    }
}

void NPC::draw(sf::RenderTarget& target, sf::RenderStates states) const {

    assert(_initialized);
    
    target.draw(_sprite, states);
}

void NPC::doAction(const NPC::ACTION& action, const float& delay, const float& duration) {

    if (action == ACTION::FINISH_THROW) {
        finishThrowing();

    } else {
        _nextAction = action;
        _prepareTimeRemaining = delay;
        _nextActionDuration = duration;
    }
}

void NPC::setFacingLeft(const bool& faceLeft) {
    if (faceLeft != _isFacingLeft) {
        _sprite.scale(-1.0f, 1.0f);
        _isFacingLeft = faceLeft;
    }
}

void NPC::stopWalking() {

    if (isWalking()) {

        toIdle();
        
        if (_nextAction == ACTION::WALK) {
            _nextAction == ACTION::IDLE;
            _prepareTimeRemaining = 0.0f;
        }
    }
}

void NPC::walk(const float& duration) {

    if (!isThrowing()) {

        _state = WALKING;
        _isReadyToFinishThrowing = false;

        _frameDuration = _WALK_FRAME_DURATION;
        _startFrame = _WALK_START_FRAME;
        _numFrames = _NUM_WALK_FRAMES;
        _currentFrame = _startFrame;

        _frameTimer = 0.0f;
        _actionTimeRemaining = duration;
    }
}

void NPC::startThrowing(const float& duration) {

    _state = STARTING_THROW;
    _isReadyToFinishThrowing = false;

    _frameDuration = _THROW_FRAME_DURATION;
    _startFrame = _START_THROW_START_FRAME;
    _numFrames = _NUM_START_THROW_FRAMES;
    _currentFrame = _startFrame;

    _frameTimer = 0.0f;
    _actionTimeRemaining = duration;
}

void NPC::finishThrowing() {

    _state = FINISHING_THROW;
    _isReadyToFinishThrowing = false;

    _frameDuration = _THROW_FRAME_DURATION;
    _startFrame = _FINISH_THROW_START_FRAME;
    _numFrames = _NUM_FINISH_THROW_FRAMES;
    _currentFrame = _startFrame;

    _frameTimer = 0.0f;
    _actionTimeRemaining = _numFrames * _frameDuration;
}

void NPC::toIdle() {

    _state = IDLE;
    _isReadyToFinishThrowing = false;

    _frameDuration = _IDLE_FRAME_DURATION;
    _startFrame = _IDLE_START_FRAME;
    _numFrames = _NUM_IDLE_FRAMES;
    _currentFrame = randomInt(_startFrame, _startFrame + _numFrames - 1);

    _frameTimer = 0.0f;
}
