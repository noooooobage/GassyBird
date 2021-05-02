#include <cassert>
#include <iostream>

#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>

#include "HumanView.hpp"
#include "GameLogic.hpp"
#include "Globals.hpp"
#include "Utils.hpp"
#include "Resources/SpriteResource.hpp"
#include "Events/KeyPressEvent.hpp"
#include "Events/KeyReleaseEvent.hpp"
#include "Events/WindowCloseEvent.hpp"
#include "Events/GamePauseEvent.hpp"
#include "ObstacleFactory.hpp"
#include "PhysicalActor.hpp"

HumanView::HumanView() :

    _initialized(false),
    
    _keyToPoop(sf::Keyboard::Key::Space),
    _keyToPause(sf::Keyboard::Key::P)
{}

HumanView::~HumanView() {

    // remove event listeners
    eventMessenger.removeListener(KeyPressEvent::TYPE, _keyPressListener);
    eventMessenger.removeListener(KeyReleaseEvent::TYPE, _keyReleaseListener);
    
    // nullify pointers
    _logic = nullptr;
}

void HumanView::init(GameLogic& logic) {

    _initialized = true;

    _logic = &logic;
        
    // set the beach background sprite and scale it according to the beach background resource
    const SpriteResource* beachSpriteResource =
            resourceCache.getResource<SpriteResource>("BEACH_BACKGROUND_SPRITE");
    _beachBackground = beachSpriteResource->sprite;
    _beachBackground.scale(beachSpriteResource->scaleFactor, beachSpriteResource->scaleFactor);

    // initialize and add event listeners
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

    // draw beach background
    target.draw(_beachBackground);

    // draw all visible actors given by the logic
    for (auto& pair : _logic->getVisibleActors()) {

        const PhysicalActor* actor = pair.first;
        const b2Body* body = pair.second;

        assert(actor);
        assert(body);

        // Set a transform to draw the actor in the correct position and rotation graphically. This
        // assumes that the actor is at graphical position (0, 0).
        sf::RenderStates statesCopy = states;
        statesCopy.transform *= physicalToGraphicalTransform(*body);
        target.draw(*actor, statesCopy);
    }
}

void HumanView::keyPressHandler(const Event& event) {
    assert(event.getType() == KeyPressEvent::TYPE);

    const KeyPressEvent& e = dynamic_cast<const KeyPressEvent&>(event);

    if (e.key == _keyToPoop)
        _logic->requestBirdPoop();
    
    else if (e.key == _keyToPause)
        eventMessenger.queueEvent(GamePauseEvent(_logic->isPaused() ?
                GamePauseEvent::ACTION::UNPAUSE : GamePauseEvent::ACTION::PAUSE));
    
    // all keys besides those cause the bird to start flying
    else
        _logic->requestBirdStartFly();
}

void HumanView::keyReleaseHandler(const Event& event) {
    assert(event.getType() == KeyReleaseEvent::TYPE);

    const KeyReleaseEvent& e = dynamic_cast<const KeyReleaseEvent&>(event);

    if(e.key != _keyToPoop && e.key != _keyToPause)
        _logic->requestBirdStopFly();
}