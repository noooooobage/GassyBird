#include <cassert>

#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>

#include "PlayingActivity.hpp"
#include "Globals.hpp"
#include "Resources/SpriteResource.hpp"

PlayingActivity::PlayingActivity() :
    _initialized(false),
    _currentActivity(nullptr)
{}

void PlayingActivity::init(sf::RenderTarget& target) {

    _initialized = true;

    // initialize logic; if in DEBUG mode, also set its debug drawer
    _logic.init();
    if (DEBUG) {
        _debugDrawer.init(target);
        _debugDrawer.SetFlags(b2Draw::e_shapeBit);
        _logic.setDebugDrawer(_debugDrawer);
    }

    // initialize views with logic
    _humanView.init(&_logic);

    // initialize activities
    _mainMenuActivity.init(*this);

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

    // deactivate old activity
    if (_currentActivity)
        _currentActivity->deactivate();
    
    // set current activity to playing menu and activate it
    _currentActivity = &_playingMenuActivity;
    _currentActivity->activate();

    // set logic to playing
    _logic.toPlaying();
}
