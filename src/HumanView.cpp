#include <cassert>
#include <iostream>

#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>

#include "HumanView.hpp"
#include "GameLogic.hpp"
#include "Globals.hpp"
#include "Resources/SpriteResource.hpp"

HumanView::HumanView() : _initialized(false) {}

void HumanView::init(GameLogic* logic) {

    _initialized = true;

    _logic = logic;

    // set the bird's sprite
    _logic->getPlayableBird().setSprite(
            resourceCache.getResource<SpriteResource>("TEST_BIRD_SPRITE"));
}

HumanView::~HumanView() {
    // nullify pointers
    _logic = nullptr;
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
