#include <cassert>
#include <iostream>
#include <string>
#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>

#include "HumanView.hpp"
#include "GameLogic.hpp"
#include "Globals.hpp"
#include "Utils.hpp"
#include "Resources/SpriteResource.hpp"
#include "Resources/FontResource.hpp"
#include "Events/KeyPressEvent.hpp"
#include "Events/KeyReleaseEvent.hpp"
#include "ObstacleFactory.hpp"
#include "PhysicalActor.hpp"

HumanView::HumanView() :

    _initialized(false),
    
    _keyToFly(sf::Keyboard::Key::W),
    _keyToPoop(sf::Keyboard::Key::Space)
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
        
    // set the beach background sprite
    _beachBackground = resourceCache.getResource<SpriteResource>("BEACH_BACKGROUND_SPRITE")->sprite;

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
    if(_logic->getState() == GameLogic::PLAYING) {
        sf::Font font = resourceCache.getResource<FontResource>("ARCADE_FONT")->font;
        sf::Text text;
        text.setFont(font);
        text.setFillColor(sf::Color::Black);
        text.setString("SCORE " + std::to_string(_logic->_playerScore));
        text.setCharacterSize(40);
        text.setPosition(sf::Vector2f(10.0f, 0.0f));
        target.draw(text);
        sf::Text poopsText;
        poopsText.setFont(font);
        poopsText.setFillColor(sf::Color::Black);
        poopsText.setString("POOPS LEFT " + std::to_string(_logic->getNumberOfPoopsLeft()));
        poopsText.setCharacterSize(40);
        poopsText.setPosition(sf::Vector2f(NATIVE_RESOLUTION.x-poopsText.getGlobalBounds().width-10, 0.0f));
        target.draw(poopsText);
    }
}

void HumanView::keyPressHandler(const Event& event) {
    assert(event.getType() == KeyPressEvent::TYPE);

    const KeyPressEvent& e = dynamic_cast<const KeyPressEvent&>(event);

    if(e.key == _keyToFly)
        _logic->requestBirdStartFly();

    else if (e.key == _keyToPoop)
        _logic->requestBirdPoop();
}

void HumanView::keyReleaseHandler(const Event& event) {
    assert(event.getType() == KeyReleaseEvent::TYPE);

    const KeyReleaseEvent& e = dynamic_cast<const KeyReleaseEvent&>(event);

    if(e.key == _keyToFly) {
        _logic->requestBirdStopFly();
    }
}