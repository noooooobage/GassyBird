#include <iostream>
#include <cassert>
#include <string>

#include "PlayingActivity.hpp"
#include "PlayingMenuActivity.hpp"
#include "GameLogic.hpp"
#include "Globals.hpp"
#include "Resources/FontResource.hpp"
#include "Utils.hpp"
#include "Event.hpp"
#include "Events/ButtonClickEvent.hpp"
#include "Events/GamePauseEvent.hpp"
#include "Events/WindowCloseEvent.hpp"

PlayingMenuActivity::PlayingMenuActivity() :

    _initialized(false),
    _activated(false),

    _playingActivity(nullptr),
    _logic(nullptr)
{
    // insert buttons that show up when the game is paused into the paused buttons list
    _pausedButtons = {&_resumeButton, &_menuButton, &_quitButton};
}

PlayingMenuActivity::~PlayingMenuActivity() {
    // deactivate button upon destruction
    deactivate();
}

void PlayingMenuActivity::init(PlayingActivity& playingActivity, const GameLogic& logic) {
    
    _playingActivity = &playingActivity;
    _logic = &logic;

    // initialize event listeners
    _buttonClickListener.init(&PlayingMenuActivity::buttonClickHandler, this);
    _gamePauseListener.init(&PlayingMenuActivity::gamePauseHandler, this);

    // retrieve the used font resources
    const FontResource& arcadeFont = *resourceCache.getResource<FontResource>("ARCADE_FONT");
    const FontResource& joystixFont = *resourceCache.getResource<FontResource>("JOYSTIX_FONT");

    // set up poops left text
    _poopsLeftText.setFont(arcadeFont.font);
    _poopsLeftText.setFillColor(sf::Color::Black);
    _poopsLeftText.setCharacterSize(40);
    _poopsLeftText.setPosition(10.0f, 0.0f);

    // set up score text
    _scoreText.setFont(arcadeFont.font);
    _scoreText.setFillColor(sf::Color::Black);
    _scoreText.setCharacterSize(40);
    _scoreText.setPosition(NATIVE_RESOLUTION.x - 250.0f, 0.0f);

    // set up paused text
    _pausedText.setFont(arcadeFont.font);
    _pausedText.setFillColor(sf::Color::Black);
    _pausedText.setCharacterSize(72);
    _pausedText.setString("GAME PAUSED");
    centerTextOnPoint(_pausedText, sf::Vector2f(NATIVE_RESOLUTION.x / 2.0f, 150.0f));
    
    // set pause button attributes
    _pauseButton.setFont(joystixFont);
    _pauseButton.setCharacterHeight(24);
    _pauseButton.setString("||");
    _pauseButton.setSize(sf::Vector2f(50.0f, 50.0f));
    _pauseButton.setPosition(sf::Vector2f(NATIVE_RESOLUTION.x - 60.0f, 10.0f));

    // set up button manager for pause button
    _playingButtonManager.init();
    _playingButtonManager.addButton(&_pauseButton);

    // set other button attributes
    for (Button* button : _pausedButtons) {
        button->setFont(arcadeFont);
        button->setCharacterHeight(48);
        button->setSize(sf::Vector2f(325.0f, 75.0f));
    }

    // x position such that paused buttons are centered on screen
    float xPosition = NATIVE_RESOLUTION.x / 2.0f - 325.0f / 2.0f;

    _resumeButton.setString("RESUME");
    _resumeButton.setPosition(sf::Vector2f(xPosition, 225.0f));

    _menuButton.setString("MAIN MENU");
    _menuButton.setPosition(sf::Vector2f(xPosition, 325.0f));

    _quitButton.setString("QUIT");
    _quitButton.setPosition(sf::Vector2f(xPosition, 425.0f));

    // set up button manager for paused screen buttons
    _pausedButtonManager.init(true);                // above        // right        // below        // left
    _pausedButtonManager.addButton(&_resumeButton, {&_quitButton,   &_menuButton,   &_menuButton,   &_quitButton});
    _pausedButtonManager.addButton(&_menuButton,   {&_resumeButton, &_quitButton,   &_quitButton,   &_resumeButton});
    _pausedButtonManager.addButton(&_quitButton,   {&_menuButton,   &_resumeButton, &_resumeButton, &_menuButton});

    _initialized = true;
}

void PlayingMenuActivity::activate() {

    assert(_initialized);

    _activated = true;

    // add event listeners
    eventMessenger.addListener(ButtonClickEvent::TYPE, _buttonClickListener);
    eventMessenger.addListener(GamePauseEvent::TYPE, _gamePauseListener);

    // activate the playing button manager
    _playingButtonManager.activate();

    // only activate the paused button manager if the game is paused
    if (_logic->isPaused())
        _pausedButtonManager.activate();
    else
        _pausedButtonManager.deactivate();
}

void PlayingMenuActivity::deactivate() {

    // Intentionally do not have an assert(_initialized) here as this method can be called by the
    // destructor.

    // remove event listeners
    eventMessenger.removeListener(ButtonClickEvent::TYPE, _buttonClickListener);
    eventMessenger.removeListener(GamePauseEvent::TYPE, _gamePauseListener);

    // deactivate all button managers
    _playingButtonManager.deactivate();
    _pausedButtonManager.deactivate();

    _activated = false;
}

void PlayingMenuActivity::update(const float& timeDelta) {
    
    assert(_initialized);
    assert(_activated);

    // update both texts
    _poopsLeftText.setString("POOPS   LEFT  " + std::to_string(_logic->getNumPoopsLeft()));
    _scoreText.setString("SCORE  " + std::to_string(_logic->getPlayerScore()));

    // Update the text in the pause button to reflect the paused state of the logic -- if the game
    // is paused, then it needs to show the ">" (play symbol); else, show the "||" (pause symbol).
    _pauseButton.setString(_logic->isPaused() ? ">" : "||");
}

void PlayingMenuActivity::draw(sf::RenderTarget& target) {
    
    assert(_initialized);
    assert(_activated);

    // draw both texts
    target.draw(_poopsLeftText);
    target.draw(_scoreText);

    // always draw the pause button
    target.draw(_pauseButton);

    // only draw paused stuff if the game is paused
    if (_logic->isPaused()) {
        for (Button* button : _pausedButtons)
            target.draw(*button);
        target.draw(_pausedText);
    }
}

void PlayingMenuActivity::buttonClickHandler(const Event& event) {

    assert(_initialized);
    assert(_activated);

    // make sure that the event is a ButtonClickEvent and then convert it
    assert(event.getType() == ButtonClickEvent::TYPE);
    const ButtonClickEvent& e = dynamic_cast<const ButtonClickEvent&>(event);

    if (e.button == &_pauseButton)
        // Queue a pause event whose action is the opposite of whether the game is currently paused.
        // Have to queue the event instead of triggering it, because triggering the event might
        // deactivate the paused button manager, and we want to deactivate that at a safer time.
        eventMessenger.queueEvent(GamePauseEvent(_logic->isPaused() ? 
                GamePauseEvent::ACTION::UNPAUSE : GamePauseEvent::ACTION::PAUSE));
        
    else if (e.button == &_resumeButton)
        // Queue a pause (unpause) event -- queue instead of trigger for same reason as above
        eventMessenger.queueEvent(GamePauseEvent(GamePauseEvent::ACTION::UNPAUSE));
    
    else if (e.button == &_menuButton)
        // tell the parent activity to go back to the main menu
        _playingActivity->toMain();
    
    else if (e.button == &_quitButton)
        // queue a window close event
        eventMessenger.queueEvent(WindowCloseEvent());
}

void PlayingMenuActivity::gamePauseHandler(const Event& event) {

    assert(_initialized);
    assert(_activated);

    // make sure that the event is a GamePauseEvent and then convert it
    assert(event.getType() == GamePauseEvent::TYPE);
    const GamePauseEvent& e = dynamic_cast<const GamePauseEvent&>(event);

    // if the game was paused, then need to activate the paused button manager; else, deactivate it
    if (e.action == GamePauseEvent::ACTION::PAUSE)
        _pausedButtonManager.activate();
    else
        _pausedButtonManager.deactivate();
}
