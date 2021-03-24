#include <cassert>
#include <memory>
#include <iostream>

#include <SFML/Graphics.hpp>

#include "Game.hpp"
#include "Globals.hpp"
#include "ResourceCache.hpp"
#include "EventListener.hpp"
#include "Event.hpp"
#include "Events/WindowResizeEvent.hpp"
#include "Events/WindowCloseEvent.hpp"
#include "Events/KeyPressEvent.hpp"
#include "Events/KeyReleaseEvent.hpp"
#include "Events/ButtonPressEvent.hpp"

Game::Game() :
    _initialized(false),
    _timeDelta(0.0f)
{
    // init event listeners
    _windowResizeListener.init(&Game::windowResizeHandler, this);
    _windowCloseListener.init(&Game::windowCloseHandler, this);
    _buttonPressListener.init(&Game::buttonHandler, this);

    // add listeners to event messenger
    eventMessenger.addListener(WindowResizeEvent::TYPE, _windowResizeListener);
    eventMessenger.addListener(WindowCloseEvent::TYPE, _windowCloseListener);
    eventMessenger.addListener(ButtonPressEvent::TYPE, _buttonPressListener);
}

Game::~Game() {

    // remove listeners
    eventMessenger.removeListener(WindowCloseEvent::TYPE, _windowCloseListener);
    eventMessenger.removeListener(WindowResizeEvent::TYPE, _windowResizeListener);

    // free memory
    _window.reset();

    // nullify pointers
    _currentActivity = nullptr;
}

void Game::init() {

    _initialized = true;

    // turn antialiasing on
    sf::ContextSettings settings;
    settings.antialiasingLevel = 4;

    // make the window
    _window = std::make_shared<sf::RenderWindow>(
        sf::VideoMode(NATIVE_RESOLUTION.x, NATIVE_RESOLUTION.y, 32),
        "Gassy Bird",
        sf::Style::Default,
        settings
    );
    _window->setActive();
    _window->setKeyRepeatEnabled(false);

    // initialize resource cache
    resourceCache.init();

    // init playing activity
    _playingActivity.init();

    _menuActivity.init();

    // set current activity to playing activity
    _currentActivity = &_menuActivity;

    // restart game clock
    _clock.restart();
}

bool Game::update() {

    assert(_initialized);

    // poll events
    sf::Event sfmlEvent;
    while (_window->pollEvent(sfmlEvent)) {
        
        switch (sfmlEvent.type) {

        // queue a WindowCloseEvent if the window was requested to be closed
        case sf::Event::Closed:
            eventMessenger.queueEvent(WindowCloseEvent());
            break;

        // Trigger a WindowResizeEvent if the window was resized; this event is triggered instead of
        // queued because activities might want to know about it to reduce choppiness.
        case sf::Event::Resized:
            eventMessenger.triggerEvent(WindowResizeEvent(sfmlEvent));
            break;

        case sf::Event::KeyPressed:
            //we trigger the event to anyone who is listening
            eventMessenger.triggerEvent(KeyPressEvent(sfmlEvent.key.code));
            break;
        
        case sf::Event::KeyReleased:
            eventMessenger.triggerEvent(KeyReleaseEvent(sfmlEvent.key.code));
            break;
        case sf::Event::MouseButtonPressed:
            std::cout << sfmlEvent.mouseButton.x << std::endl;
            eventMessenger.triggerEvent(ButtonPressEvent(sfmlEvent.mouseButton.x, sfmlEvent.mouseButton.y));
            break;
        }
    }

    // determine time delta and divert update call to current activity
    float timeDelta = _clock.restart().asSeconds();
    _currentActivity->update(timeDelta);

    // trigger all queued events
    eventMessenger.triggerQueuedEvents();
    
    return _window->isOpen();
}

void Game::draw() const {

    assert(_initialized);

    // clear the window, let the current activity draw onto the window, then display the window
    _window->clear();
    _window->draw(*_currentActivity);
    _window->display();
}

void Game::windowResizeHandler(const Event& e) {

    assert(e.getType() == WindowResizeEvent::TYPE);

    // cast e to its proper type
    const WindowResizeEvent& event = dynamic_cast<const WindowResizeEvent&>(e);

    // actual window size in pixels
    sf::Vector2i windowSize(event.width, event.height);

    // Ideal size of the visible rectangle in pixels such that area is maximized and native aspect
    // ratio is maintained
    sf::Vector2f idealSize = (sf::Vector2f)windowSize;

    // determine ideal size
    if (windowSize.y * NATIVE_ASPECT_RATIO > windowSize.x)
        // window is taller than it should be
        idealSize.y = windowSize.x / NATIVE_ASPECT_RATIO;
    else
        // window is wider than it should be
        idealSize.x = windowSize.y * NATIVE_ASPECT_RATIO;
    
    // change the window's viewport to match idealSize -- this requires setting the whole view
    sf::View windowView = _window->getView();
    windowView.setViewport(sf::FloatRect(
        (windowSize.x - idealSize.x) / 2.0f / windowSize.x,
        (windowSize.y - idealSize.y) / 2.0f / windowSize.y,
        idealSize.x / windowSize.x,
        idealSize.y / windowSize.y
    ));
    _window->setView(windowView);
}

void Game::windowCloseHandler(const Event& e) {
    
    assert(e.getType() == WindowCloseEvent::TYPE);

    _window->close();
}

void Game::buttonHandler(const Event& event) {
    assert(event.getType() == ButtonPressEvent::TYPE);

    const ButtonPressEvent& e = dynamic_cast<const ButtonPressEvent&>(event);

    std::cout << "Processing button" << std::endl;
    std::cout << e.xCoord << " " << e.yCoord << std::endl;
    if(_menuActivity.getButton().isInsideBounds(e.xCoord, e.yCoord)) {
        std::cout << "Switch activity" << std::endl;
    } else {
        std::cout << "Same activity" << std::endl;
    }
}
