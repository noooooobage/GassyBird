#include <cassert>
#include <iostream>

#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>

#include "HumanView.hpp"
#include "GameLogic.hpp"
#include "Globals.hpp"
#include "Resources/SpriteResource.hpp"
#include "Events/KeyPressEvent.hpp"
#include "Events/KeyReleaseEvent.hpp"

HumanView::HumanView() :
    _initialized(false),
    _keyToFly(sf::Keyboard::Key::W)
{}

HumanView::~HumanView() {

    // remove event listeners
    eventMessenger.removeListener(KeyPressEvent::TYPE, _keyPressListener);
    eventMessenger.removeListener(KeyReleaseEvent::TYPE, _keyReleaseListener);
    
    // nullify pointers
    _logic = nullptr;
}

void HumanView::init(GameLogic* logic) {

    _initialized = true;

    _logic = logic;

    // set the bird's sprite
    _logic->getPlayableBird().setSprite(
            *resourceCache.getResource<SpriteResource>("TEST_BIRD_SPRITE"));

    _keyPressListener.init(&HumanView::keyPressHandler, this);
    _keyReleaseListener.init(&HumanView::keyReleaseHandler, this);

    eventMessenger.addListener(KeyPressEvent::TYPE, _keyPressListener);
    eventMessenger.addListener(KeyReleaseEvent::TYPE, _keyReleaseListener);
}

void HumanView::update(const float& timeDelta) {
    assert(_initialized);
}

void HumanView::draw(sf::RenderTarget& target, sf::RenderStates states) const {

    assert(_initialized);

    // Set a transform to draw the actor in the correct position and rotation graphically. This
    // assumes that the actor is at position (0, 0).
    const b2Body* birdBody = _logic->getBody(_logic->getPlayableBird());
    assert(birdBody != nullptr);
    sf::Transform transform;
    transform.translate(
        birdBody->GetPosition().x * PIXELS_PER_METER,
        NATIVE_RESOLUTION.y - birdBody->GetPosition().y * PIXELS_PER_METER
    ).rotate(-180.0f / PI * birdBody->GetAngle());
    states.transform *= transform;

    // draw actor
    target.draw(_logic->getPlayableBird(), states);
}

void HumanView::keyPressHandler(const Event& event) {
    assert(event.getType() == KeyPressEvent::TYPE);

    const KeyPressEvent& e = dynamic_cast<const KeyPressEvent&>(event);

    if(e.keyPress == _keyToFly) {
        _logic->requestBirdStartFly();
    }
}

void HumanView::keyReleaseHandler(const Event& event) {
    assert(event.getType() == KeyReleaseEvent::TYPE);

    const KeyReleaseEvent& e = dynamic_cast<const KeyReleaseEvent&>(event);

    if(e.keyPress == _keyToFly) {
        _logic->requestBirdStopFly();
    }
}