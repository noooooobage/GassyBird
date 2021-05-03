#include <iostream>
#include <cassert>
#include <string>

#include "PlayingActivity.hpp"
#include "PlayingMenuActivity.hpp"
#include "GameLogic.hpp"
#include "Globals.hpp"
#include "Resources/FontResource.hpp"
#include "Resources/SpriteResource.hpp"
#include "Utils.hpp"
#include "Event.hpp"
#include "Events/ButtonClickEvent.hpp"
#include "Events/GamePauseEvent.hpp"
#include "Events/WindowCloseEvent.hpp"

PlayingMenuActivity::PlayingMenuActivity() :

    _initialized(false),
    _activated(false),

    _playingActivity(nullptr),
    _logic(nullptr),

    _SCORE_CHANGE_TIME(0.65f),
    _SCORE_BASE_COLOR(sf::Color::Black),
    _SCORE_CHANGE_COLOR(sf::Color(150, 150, 150, 150)),
    _SCORE_CHANGE_SCALE(3.0f)
{
    _indicators = {&_bottomPoopIndicator, &_topPoopIndicator};
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

    // retrieve resources
    const FontResource& arcadeFont = *resourceCache.getResource<FontResource>("ARCADE_FONT");
    const FontResource& joystixFont = *resourceCache.getResource<FontResource>("JOYSTIX_FONT");
    const SpriteResource& indicatorSprite =
            *resourceCache.getResource<SpriteResource>("CIRCLE_INDICATOR_SPRITE");

    // set up indicators
    _indicatorRects = indicatorSprite.textureRects;
    for (sf::Sprite* indicator : _indicators) {
        *indicator = indicatorSprite.sprite;
        indicator->scale(indicatorSprite.scaleFactor, indicatorSprite.scaleFactor);
    }

    _topPoopIndicator.setPosition(50.0f, 15.0f);
    _bottomPoopIndicator.setPosition(50.0f, 55.0f);

    // set up score text, put origin in the top left
    _scoreText.setFont(arcadeFont.font);
    _scoreText.setFillColor(sf::Color::Black);
    _scoreText.setCharacterSize(72);
    _scoreText.setPosition(100, 15.0f);
    _scoreText.setString("0");
    sf::FloatRect bounds = _scoreText.getLocalBounds();
    _scoreText.setOrigin(bounds.left, bounds.top);

    // set up poop time left bar, origin in bottom left
    _poopTimeLeftBar.setSize(sf::Vector2f(12.0f, 64.0f));
    _poopTimeLeftBar.setOrigin(0.0f, _poopTimeLeftBar.getSize().y);
    _poopTimeLeftBar.setPosition(20.0f, 83.0f);
    _poopTimeLeftBar.setFillColor(sf::Color::Black);
    _poopTimeLeftOutline.setSize(_poopTimeLeftBar.getSize());
    _poopTimeLeftOutline.setOrigin(_poopTimeLeftBar.getOrigin());
    _poopTimeLeftOutline.setPosition(_poopTimeLeftBar.getPosition());
    _poopTimeLeftOutline.setOutlineColor(sf::Color::White);
    _poopTimeLeftOutline.setOutlineThickness(4.0f);
    _poopTimeLeftOutline.setFillColor(sf::Color::Transparent);

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

    // reset necessary variables
    _scoreChangeTimer = 0.0f;

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

    // fill the indicators up to the amount of poops that the bird has left
    int poopsLeft = _logic->getNumPoopsLeft();
    for (int i = 0; i < _indicators.size(); ++i)
        _indicators.at(i)->setTextureRect(_indicatorRects.at(i < poopsLeft ? 0 : 1));
    
    // fill the poop time left bar according to how much time the bird has left to poop
    _poopTimeLeftBar.setScale(1.0f, _logic->getPoopTimeLeft());

    // update the score text's string
    std::string scoreString = std::to_string(_logic->getPlayerScore());
    if (scoreString != _scoreText.getString()) {
        _scoreText.setString(scoreString);
        if (scoreString != "0")
            _scoreChangeTimer = _SCORE_CHANGE_TIME;
    }

    // update the score text's animation
    _scoreChangeTimer -= timeDelta;
    if (_scoreChangeTimer < 0.0f)
        _scoreChangeTimer = 0.0f;
    float t = _scoreChangeTimer / _SCORE_CHANGE_TIME;
    float scaleFactor = lerp(1.0f, _SCORE_CHANGE_SCALE, t);
    _scoreText.setScale(scaleFactor, scaleFactor);
    _scoreText.setFillColor(sf::Color(
        clamp((int)lerp((float)_SCORE_BASE_COLOR.r, (float)_SCORE_CHANGE_COLOR.r, t), 0, 255),
        clamp((int)lerp((float)_SCORE_BASE_COLOR.g, (float)_SCORE_CHANGE_COLOR.g, t), 0, 255),
        clamp((int)lerp((float)_SCORE_BASE_COLOR.b, (float)_SCORE_CHANGE_COLOR.b, t), 0, 255),
        clamp((int)lerp((float)_SCORE_BASE_COLOR.a, (float)_SCORE_CHANGE_COLOR.a, t), 0, 255)
    ));

    // Update the text in the pause button to reflect the paused state of the logic -- if the game
    // is paused, then it needs to show the ">" (play symbol); else, show the "||" (pause symbol).
    _pauseButton.setString(_logic->isPaused() ? ">" : "||");
}

void PlayingMenuActivity::draw(sf::RenderTarget& target) {
    
    assert(_initialized);
    assert(_activated);

    for (sf::Sprite* indicator : _indicators)
        target.draw(*indicator);
    target.draw(_poopTimeLeftOutline);
    target.draw(_poopTimeLeftBar);
    target.draw(_scoreText);
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
