#include <cassert>
#include <iostream>

#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>

#include "NPCView.hpp"
#include "GameLogic.hpp"
#include "Globals.hpp"
#include "Utils.hpp"
#include "Resources/SpriteResource.hpp"
#include "Events/WindowCloseEvent.hpp"
#include "Events/GamePauseEvent.hpp"
#include "ObstacleFactory.hpp"
#include "PhysicalActor.hpp"
#include "NPC.hpp"

NPCView::NPCView() :

    _initialized(false)

{}

NPCView::~NPCView() {

    // remove event listeners
    eventMessenger.removeListener(KeyPressEvent::TYPE, _keyPressListener);
    eventMessenger.removeListener(KeyReleaseEvent::TYPE, _keyReleaseListener);

    // nullify pointers
    _logic = nullptr;
}

void NPCView::init(GameLogic& logic) {

    _initialized = true;

    _logic = &logic;

    // initialize and add event listeners
    _keyPressListener.init(&NPCView::keyPressHandler, this);
    // _keyReleaseListener.init(&NPCView::keyReleaseHandler, this);

    eventMessenger.addListener(KeyPressEvent::TYPE, _keyPressListener);
    // eventMessenger.addListener(KeyReleaseEvent::TYPE, _keyReleaseListener);
}

void NPCView::update(const float& timeDelta) {
    assert(_initialized);
}

void NPCView::draw(sf::RenderTarget& target, sf::RenderStates states) const {

    assert(_initialized);

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

// intended to make random NPC throw something when bird poops

void NPCView::keyPressHandler(const Event& event) {

    // assert(isHit);

    assert(event.getType() == KeyPressEvent::TYPE);

    const KeyPressEvent& e = dynamic_cast<const KeyPressEvent&>(event);

    // if(e.key == _keyToPoop) {
        logic->requestTriggerAction();
    // }
}

