#include <list>
#include <iostream>

#include <SFML/Graphics.hpp>

#include "ButtonManager.hpp"
#include "Button.hpp"
#include "Globals.hpp"
#include "Event.hpp"
#include "Events/MouseMoveEvent.hpp"
#include "Events/MousePressEvent.hpp"
#include "Events/MouseReleaseEvent.hpp"
#include "Events/KeyPressEvent.hpp"
#include "Events/KeyReleaseEvent.hpp"
#include "Events/ButtonClickEvent.hpp"

ButtonManager::ButtonManager() :

    _initialized(false),
    _activated(false),

    _buttonHovered(nullptr),
    _buttonIsClicked(false),

    _CONTROL_KEYS({

        {sf::Keyboard::Up,     CONTROL_KEY_TYPE::UP},
        {sf::Keyboard::W,      CONTROL_KEY_TYPE::UP},

        {sf::Keyboard::Right,  CONTROL_KEY_TYPE::RIGHT},
        {sf::Keyboard::D,      CONTROL_KEY_TYPE::RIGHT},

        {sf::Keyboard::Down,   CONTROL_KEY_TYPE::DOWN},
        {sf::Keyboard::S,      CONTROL_KEY_TYPE::DOWN},

        {sf::Keyboard::Left,   CONTROL_KEY_TYPE::LEFT},
        {sf::Keyboard::A,      CONTROL_KEY_TYPE::LEFT},
        
        {sf::Keyboard::Return, CONTROL_KEY_TYPE::SELECT},
        {sf::Keyboard::Space,  CONTROL_KEY_TYPE::SELECT},
    })
{
    // initalize event listeners
    _mouseMoveListener.init(   &ButtonManager::mouseMoveHandler,    this);
    _mousePressListener.init(  &ButtonManager::mousePressHandler,   this);
    _mouseReleaseListener.init(&ButtonManager::mouseReleaseHandler, this);
    _keyPressListener.init(    &ButtonManager::keyPressHandler,     this);
    _keyReleaseListener.init(  &ButtonManager::keyReleaseHandler,   this);
}

ButtonManager::~ButtonManager() {
    // must remove listeners when object is destroyed
    deactivate();
}

void ButtonManager::init(const bool& canUseKeyboard) {

    _canUseKeyboard = canUseKeyboard;

    _initialized = true;
}

void ButtonManager::activate() {

    assert(_initialized);

    // add event listeners; only add key listeners if _canUseKeyboard is true
    eventMessenger.addListener(MouseMoveEvent::TYPE,    _mouseMoveListener);
    eventMessenger.addListener(MousePressEvent::TYPE,   _mousePressListener);
    eventMessenger.addListener(MouseReleaseEvent::TYPE, _mouseReleaseListener);
    if (_canUseKeyboard) {
        eventMessenger.addListener(KeyPressEvent::TYPE,   _keyPressListener);
        eventMessenger.addListener(KeyReleaseEvent::TYPE, _keyReleaseListener);
    }

    // if the keyboard can be used and there's at least one button, then set the first button to be
    // the currently hovered button
    if (_canUseKeyboard && !_buttons.empty()) {
        _buttonHovered = _buttons.begin()->first;
        _buttonHovered->toHovered();
    } else {
        _buttonHovered = nullptr;
    }
    
    // when starting out, the hovered button should not be clicked
    _buttonIsClicked = false;

    // now the button manager is activated
    _activated = true;
}

void ButtonManager::deactivate() {

    // remove event listeners
    eventMessenger.removeListener(MouseMoveEvent::TYPE,    _mouseMoveListener);
    eventMessenger.removeListener(MousePressEvent::TYPE,   _mousePressListener);
    eventMessenger.removeListener(MouseReleaseEvent::TYPE, _mouseReleaseListener);
    eventMessenger.removeListener(KeyPressEvent::TYPE,     _keyPressListener);
    eventMessenger.removeListener(KeyReleaseEvent::TYPE,   _keyReleaseListener);

    _activated = false;
}

void ButtonManager::addButton(Button* button, ButtonAdjacencies adjacencies) {

    assert(_initialized);

    // adding buttons should happen before activation
    assert(!_activated);

    // make sure button isn't nullptr
    assert(button);

    // if the button doesn't already exist, then add it to the map
    if (_buttons.find(button) == _buttons.end())
        _buttons[button] = adjacencies;
}

Button* ButtonManager::getButtonContaining(const sf::Vector2f& point) {

    assert(_activated);

    for (auto i : _buttons)
        if (i.first->contains(point))
            return i.first;
    
    return nullptr;
}

void ButtonManager::mouseMoveHandler(const Event& event) {

    assert(_initialized);
    assert(_activated);
    assert(event.getType() == MouseMoveEvent::TYPE);

    const MouseMoveEvent& e = dynamic_cast<const MouseMoveEvent&>(event);

    // determine which button the mouse is inside, if any
    Button* insideButton = getButtonContaining(e.graphical);

    // if the mouse has gone outside the hovered button...
    if (_buttonHovered && insideButton != _buttonHovered) {

        // if the keyboard can be used, then the hovered button can remain hovered even if the mouse
        // isn't inside a button anymore
        if (_canUseKeyboard && !insideButton) {
            _buttonHovered->toHovered();
        } else {
            _buttonHovered->toBase();
            _buttonHovered = nullptr;
        }

        // no matter what, no button can be clicked if the mouse has gone outside the hovered button
        _buttonIsClicked = false;
    }

    // if the mouse is inside a button, then set that one to be hovered (as long as it's not
    // clicked)
    if (insideButton) {
        _buttonHovered = insideButton;
        if (!_buttonIsClicked)
            _buttonHovered->toHovered();
    }
}

void ButtonManager::mousePressHandler(const Event& event) {

    assert(_initialized);
    assert(_activated);
    assert(event.getType() == MousePressEvent::TYPE);

    const MousePressEvent& e = dynamic_cast<const MousePressEvent&>(event);

    // if the mouse pressed the currently hovered button, then set it to clicked
    if (_buttonHovered && getButtonContaining(e.graphical) == _buttonHovered) {
        _buttonHovered->toClicked();
        _buttonIsClicked = true;
    }
}

void ButtonManager::mouseReleaseHandler(const Event& event) {

    assert(_initialized);
    assert(_activated);
    assert(event.getType() == MouseReleaseEvent::TYPE);

    const MouseReleaseEvent& e = dynamic_cast<const MouseReleaseEvent&>(event);

    // if the mouse is inside the currently hovered button and that button is clicked, then trigger
    // a ButtonClickEvent
    if (_buttonHovered && getButtonContaining(e.graphical) == _buttonHovered && _buttonIsClicked) {
        _buttonHovered->toHovered();
        _buttonIsClicked = false;
        eventMessenger.triggerEvent(ButtonClickEvent(*_buttonHovered));
    }
}

void ButtonManager::keyPressHandler(const Event& event) {

    assert(_initialized);
    assert(_activated);
    assert(event.getType() == KeyPressEvent::TYPE);

    const KeyPressEvent& e = dynamic_cast<const KeyPressEvent&>(event);

    // stop if the pressed key is not a control key
    auto itr = _CONTROL_KEYS.find(e.key);
    if (itr == _CONTROL_KEYS.end())
        return;
    
    // determine control type
    CONTROL_KEY_TYPE controlType = itr->second;

    // if it was SELECT, then make the current hovered button clicked
    if (_buttonHovered && controlType == CONTROL_KEY_TYPE::SELECT) {
        _buttonHovered->toClicked();
        _buttonIsClicked = true;

    // if it wasn't SELECT, then move to the correct adjacent button
    } else if (_buttonHovered) {
        Button* adjacentButton = _buttons[_buttonHovered].getAdjacent(controlType);
        if (adjacentButton) {
            _buttonHovered->toBase();
            _buttonHovered = adjacentButton;
            _buttonHovered->toHovered();
            _buttonIsClicked = false;
        }
    }
}

void ButtonManager::keyReleaseHandler(const Event& event) {

    assert(_initialized);
    assert(_activated);
    assert(event.getType() == KeyReleaseEvent::TYPE);

    const KeyReleaseEvent& e = dynamic_cast<const KeyReleaseEvent&>(event);

    // stop if the pressed key is not a control key
    auto itr = _CONTROL_KEYS.find(e.key);
    if (itr == _CONTROL_KEYS.end())
        return;
    
    // determine control type
    CONTROL_KEY_TYPE controlType = itr->second;

    // if it was SELECT and the hovered button is clicked, then trigger ButtonClickEvent
    if (_buttonHovered && controlType == CONTROL_KEY_TYPE::SELECT && _buttonIsClicked) {
        _buttonHovered->toHovered();
        _buttonIsClicked = false;
        eventMessenger.triggerEvent(ButtonClickEvent(*_buttonHovered));
    }
}
