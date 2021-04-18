#include <cassert>

#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>

#include "PlayingActivity.hpp"
#include "Globals.hpp"
#include "Resources/SpriteResource.hpp"
#include "Event.hpp"
#include "Events/GameOverEvent.hpp"

PlayingActivity::PlayingActivity() :
    _initialized(false),
    _currentActivity(nullptr)
{}

PlayingActivity::~PlayingActivity() {
    // remove event listeners upon destruction
    eventMessenger.removeListener(GameOverEvent::TYPE, _gameOverListener);
}

void PlayingActivity::init(sf::RenderTarget& target) {

    _initialized = true;
    
    // initialize event listeners and add them to event messenger
    _gameOverListener.init(&PlayingActivity::toGameOver, this);
    eventMessenger.addListener(GameOverEvent::TYPE, _gameOverListener);

    // initialize logic; if in DEBUG mode, also set its debug drawer
    _logic.init();
    if (DEBUG) {
        _debugDrawer.init(target);
        _debugDrawer.SetFlags(b2Draw::e_shapeBit | b2Draw::e_centerOfMassBit);
        _logic.setDebugDrawer(_debugDrawer);
    }

    // initialize views with logic
    _humanView.init(_logic);

    // initialize activities
    _mainMenuActivity.init(*this);
    _playingMenuActivity.init(*this, _logic);
    _gameOverActivity.init(*this, _logic);

    // start with the main menu
    toMain();
}

void PlayingActivity::update(const float& timeDelta) {

    assert(_initialized);
    assert(_currentActivity);

    // update views
    _humanView.update(timeDelta);

    // update logic
    _logic.update(timeDelta);

    // update subactivity
    _currentActivity->update(timeDelta);
}

void PlayingActivity::draw(sf::RenderTarget& target) {

    assert(_initialized);
    assert(_currentActivity);

    // draw the human view, then the current subactivity on top
    target.draw(_humanView);
    _currentActivity->draw(target);

    // if in DEBUG mode, call the logic's debug draw
    if (DEBUG)
        _logic.debugDraw();
}

void PlayingActivity::toMain() {

    assert(_initialized);

    // deactivate old activity
    if (_currentActivity)
        _currentActivity->deactivate();
    
    // set current activity to main menu and activate it
    _currentActivity = &_mainMenuActivity;
    _currentActivity->activate();

    // set logic to demo mode
    _logic.toDemo();
}

void PlayingActivity::toPlaying() {

    assert(_initialized);

    // deactivate old activity
    if (_currentActivity)
        _currentActivity->deactivate();
    
    // set current activity to playing menu and activate it
    _currentActivity = &_playingMenuActivity;
    _currentActivity->activate();

    // set logic to playing
    _logic.toPlaying();
}

void PlayingActivity::toGameOver(const Event& event) {

    assert(_initialized);

    // make sure that the passed event was a GameOverEvent
    assert(event.getType() == GameOverEvent::TYPE);

    // deactivate old activity
    if (_currentActivity)
        _currentActivity->deactivate();
    
    // set current activity to playing menu and activate it
    _currentActivity = &_gameOverActivity;
    _currentActivity->activate();

    // transition the logic to game over state
    _logic.toGameOver();
}
