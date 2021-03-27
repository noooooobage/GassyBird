#include <cassert>
#include <iostream>
#include <functional>

#include "MainMenuActivity.hpp"
#include "PlayingActivity.hpp"
#include "Globals.hpp"
#include "Events/WindowCloseEvent.hpp"

MainMenuActivity::MainMenuActivity() :
    _initialized(false),
    _activated(false)
{
    // set attributes of play button
    _playButton.setString("PLAY");
    _playButton.setPosition(sf::Vector2f(800.0f, 175.0f));

    // set attributes of exit button
    _exitButton.setString("EXIT");
    _exitButton.setPosition(sf::Vector2f(800.0f, 325.0f));

    // put buttons in list
    _buttons.push_back(&_playButton);
    _buttons.push_back(&_exitButton);
}

void MainMenuActivity::init(PlayingActivity& playingActivity) {

    // set playing activity
    _playingActivity = &playingActivity;

    // set attributes common to all buttons
    for (Button* button : _buttons) {
        button->setFont(*resourceCache.getResource<FontResource>("ARCADE_FONT"));
        button->setCharacterHeight(60);
    }

    // initialize the button manager
    _buttonClickCallback = std::bind(&MainMenuActivity::buttonClickCallback, this,
            std::placeholders::_1);
    _buttonManager.init(_buttonClickCallback, true);

    // add buttons                          above         right         below         left
    _buttonManager.addButton(&_playButton, {&_exitButton, &_exitButton, &_exitButton, &_exitButton});
    _buttonManager.addButton(&_exitButton, {&_playButton, &_playButton, &_playButton, &_playButton});

    _initialized = true;
}

void MainMenuActivity::activate() {

    assert(_initialized);

    _activated = true;
    _buttonManager.activate();
}

void MainMenuActivity::deactivate() {

    assert(_initialized);
    assert(_activated);

    _buttonManager.deactivate();
    _activated = false;
}

void MainMenuActivity::update(const float& timeDelta) {
    assert(_initialized);
    assert(_activated);
}

void MainMenuActivity::draw(sf::RenderTarget& target) {

    assert(_initialized);
    assert(_activated);

    // draw all buttons
    for (Button* button : _buttons)
        target.draw(*button);
}

void MainMenuActivity::buttonClickCallback(const Button* button) {

    assert(_initialized);
    assert(_activated);
    
    if (button == &_playButton) {
        // transition the playing activity to playing
        _playingActivity->toPlaying();

    } else if (button == &_exitButton) {
        // queue a WindowCloseEvent
        eventMessenger.queueEvent(WindowCloseEvent());
    }
}
