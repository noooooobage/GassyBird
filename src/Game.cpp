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
#include "Events/MouseMoveEvent.hpp"
#include "Events/MousePressEvent.hpp"
#include "Events/MouseReleaseEvent.hpp"
#include "Events/GamePauseEvent.hpp"

Game::Game() :
    _initialized(false),
    _timeDelta(0.0f)
{
    // init event listeners
    _windowResizeListener.init(&Game::windowResizeHandler, this);
    _windowCloseListener.init(&Game::windowCloseHandler, this);

    // add listeners to event messenger
    eventMessenger.addListener(WindowResizeEvent::TYPE, _windowResizeListener);
    eventMessenger.addListener(WindowCloseEvent::TYPE, _windowCloseListener);
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

    // TODO: remove
    sf::View view;
    view.zoom(2.5f);
    _window->setView(view);

    // initialize resource cache
    resourceCache.init();

    // init playing activity
    _playingActivity.init(*_window.get());

    // set current activity to playing activity
    _currentActivity = &_playingActivity;

    // restart game clock
    _clock.restart();
}

bool Game::update() {

    assert(_initialized);

    // poll events
    sf::Event event;
    while (_window->pollEvent(event)) {
        
        switch (event.type) {

        // queue a WindowCloseEvent if the window was requested to be closed
        case sf::Event::Closed:
            eventMessenger.queueEvent(WindowCloseEvent());
            break;
        
        // trigger a pause event if the window lost focus
        case sf::Event::LostFocus:
            eventMessenger.triggerEvent(GamePauseEvent(GamePauseEvent::ACTION::PAUSE));
            break;

        // trigger a pause event and queue a resize event if the window is resized
        case sf::Event::Resized:
            eventMessenger.triggerEvent(GamePauseEvent(GamePauseEvent::ACTION::PAUSE));
            eventMessenger.queueEvent(WindowResizeEvent(event));
            break;

        case sf::Event::KeyPressed:
            eventMessenger.triggerEvent(KeyPressEvent(event.key.code));
            break;
        
        case sf::Event::KeyReleased:
            eventMessenger.triggerEvent(KeyReleaseEvent(event.key.code));
            break;
        
        case sf::Event::MouseMoved:
            { // need a block here because we declare a variable
                sf::Vector2i pixelCoord(event.mouseMove.x, event.mouseMove.y);
                eventMessenger.triggerEvent(MouseMoveEvent(pixelCoord,
                        _window->mapPixelToCoords(pixelCoord)));
            }
            break;
        
        case sf::Event::MouseButtonPressed:
            {
                sf::Vector2i pixelCoord(event.mouseButton.x, event.mouseButton.y);
                eventMessenger.triggerEvent(MousePressEvent(event.mouseButton.button,
                        pixelCoord, _window->mapPixelToCoords(pixelCoord)));
            }
            break;

        case sf::Event::MouseButtonReleased:
            {
                sf::Vector2i pixelCoord(event.mouseButton.x, event.mouseButton.y);
                eventMessenger.triggerEvent(MouseReleaseEvent(event.mouseButton.button,
                        pixelCoord, _window->mapPixelToCoords(pixelCoord)));
            }
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
    _currentActivity->draw(*_window.get());
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
