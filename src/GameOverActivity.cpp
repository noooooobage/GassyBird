#include <cassert>
#include <string>

#include <SFML/Graphics.hpp>

#include "GameOverActivity.hpp"
#include "PlayingActivity.hpp"
#include "GameLogic.hpp"
#include "Globals.hpp"
#include "Utils.hpp"
#include "Resources/FontResource.hpp"
#include "Events/ButtonClickEvent.hpp"
#include "Events/WindowCloseEvent.hpp"

GameOverActivity::GameOverActivity() :

    _initialized(false),

    _texts({&_gameOverText, &_scoreText}),
    _buttons({&_menuButton, &_quitButton}),

    _FLASH_END_TIME(0.5f),
    _FADE_IN_START_TIME(0.25f),
    _FADE_IN_END_TIME(1.0f),
    _INITIAL_RED_SCREEN_ALPHA(190.0f)
{}

GameOverActivity::~GameOverActivity() {
    // deactivate upon destruction
    deactivate();
}

void GameOverActivity::init(class PlayingActivity& playingActivity, const GameLogic& logic) {

    _playingActivity = &playingActivity;
    _logic = &logic;

    // initialize event listeners
    _buttonClickListener.init(&GameOverActivity::buttonClickHandler, this);

    // set up red flashing rectangle
    _redScreen.setSize((sf::Vector2f)NATIVE_RESOLUTION);

    // get front resources
    const FontResource& arcadeFont = *resourceCache.getResource<FontResource>("ARCADE_FONT");
    const FontResource& joystixFont = *resourceCache.getResource<FontResource>("JOYSTIX_FONT");

    // set up texts
    for (sf::Text* text : _texts)
        text->setFont(joystixFont.font);
    
    _gameOverText.setString("GAME OVER");
    _gameOverText.setCharacterSize(72);
    centerTextOnPoint(_gameOverText, sf::Vector2f(NATIVE_RESOLUTION.x / 2.0f, 125.0f));

    _scoreText.setCharacterSize(60);

    // set up buttons
    for (Button* button : _buttons) {
        button->setFont(arcadeFont);
        button->setCharacterHeight(48);
        button->setSize(sf::Vector2f(325.0f, 75.0f));
    }

    // x position such that buttons are centered on screen
    float xPosition = NATIVE_RESOLUTION.x / 2.0f - 325.0f / 2.0f;

    _menuButton.setString("MAIN MENU");
    _menuButton.setPosition(sf::Vector2f(xPosition, 300.0f));

    _quitButton.setString("QUIT");
    _quitButton.setPosition(sf::Vector2f(xPosition, 400.0f));

    // set up button manager
    _buttonManager.init(true);           // above         right         below         left
    _buttonManager.addButton(&_menuButton, {&_quitButton, &_quitButton, &_quitButton, &_quitButton});
    _buttonManager.addButton(&_quitButton, {&_menuButton, &_menuButton, &_menuButton, &_menuButton});
    
    _initialized = true;
}

void GameOverActivity::activate() {

    assert(_initialized);

    _activated = true;

    // add event listeners
    eventMessenger.addListener(ButtonClickEvent::TYPE, _buttonClickListener);

    // deactivate the button manager -- it should be activated when the buttons fully appear
    _buttonManager.deactivate();

    // set score text and center it on the screen
    _scoreText.setString("SCORE: " + std::to_string(_logic->getPlayerScore()));
    centerTextOnPoint(_scoreText, sf::Vector2f(NATIVE_RESOLUTION.x / 2.0f, 215.0f));

    // reset values and call update() with 0 timeDelta so that other values can be reset
    _animationTimer = 0.0f;
    _menuButton.toHovered();
    update(0.0f);
}

void GameOverActivity::deactivate() {

    // Intentionally do not have an assert(_initialized) here as this method can be called by the
    // destructor.

    // remove event listeners
    eventMessenger.removeListener(ButtonClickEvent::TYPE, _buttonClickListener);

    // deactivate button manager
    _buttonManager.deactivate();

    _activated = false;
}

void GameOverActivity::update(const float& timeDelta) {

    assert(_initialized);
    assert(_activated);

    // set the opacity of the red screen rectangle
    float alpha = lerp(_INITIAL_RED_SCREEN_ALPHA, 0.0f, _animationTimer / _FLASH_END_TIME);
    _redScreen.setFillColor(sf::Color(255, 0, 0, clamp((int)alpha, 0, 255)));

    // set the opacity of the texts
    float t = (_animationTimer - _FADE_IN_START_TIME) / (_FADE_IN_END_TIME - _FADE_IN_START_TIME);
    for (sf::Text* text : _texts)
        text->setFillColor(sf::Color(0, 0, 0, clamp((int)(255 * t), 0, 255)));

    // set the opacity of the buttons
    float opacity = clamp(t, 0.0f, 1.0f);
    for (Button* button : _buttons)
        button->setOpacity(opacity);
    
    // activate the button manager if the buttons have fully appeared; otherwise, add to timer
    if (_animationTimer >= _FADE_IN_END_TIME && !_buttonManager.isActivated())
        _buttonManager.activate();
    else
        _animationTimer += timeDelta;
}

void GameOverActivity::draw(sf::RenderTarget& target) {

    assert(_initialized);
    assert(_activated);

    // draw red screen
    target.draw(_redScreen);

    // draw texts
    for (sf::Text* text : _texts)
        target.draw(*text);

    // draw buttons
    for (Button* button : _buttons)
        target.draw(*button);
}

void GameOverActivity::buttonClickHandler(const Event& event) {

    assert(_initialized);
    assert(_activated);

    // make sure the event is a ButtonClickEvent and convert it
    assert(event.getType() == ButtonClickEvent::TYPE);
    const ButtonClickEvent& e = dynamic_cast<const ButtonClickEvent&>(event);

    // go back to the main menu if main menu button was clicked
    if (e.button == &_menuButton)
        _playingActivity->toMain();
    
    // queue a window close event if the quit button was clicked
    else if (e.button == &_quitButton)
        eventMessenger.queueEvent(WindowCloseEvent());
}
