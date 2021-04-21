#ifndef _GAME_OVER_ACTIVITY_HPP_
#define _GAME_OVER_ACTIVITY_HPP_

#include <list>

#include <SFML/Graphics.hpp>

#include "Activity.hpp"
#include "GameLogic.hpp"
#include "Button.hpp"
#include "ButtonManager.hpp"
#include "EventListener.hpp"
#include "Event.hpp"

/**
 * Shows when the game is over. Flashes red, then shows game stats like the score, etc. Also
 * provides buttons to return the main menu or quit the game.
 */
class GameOverActivity : public Activity {

public:

    GameOverActivity();

    ~GameOverActivity();

    /**
     * Initialize with the parent PlayingActivity and GameLogic.
     */
    void init(class PlayingActivity& playingActivity, const GameLogic& logic);

    void activate() override;

    void deactivate() override;

    void update(const float& timeDelta) override;

    void draw(sf::RenderTarget& target) override;

private:

    void buttonClickHandler(const Event& event);

    bool _initialized;
    bool _activated;

    // event listeners
    EventListener _buttonClickListener;

    // PlayingActivity parent class and game logic
    class PlayingActivity* _playingActivity;
    const GameLogic* _logic;

    // animation stuff
    float _animationTimer;
    const float _FLASH_END_TIME; // time at which the flash fully fades out
    const float _FADE_IN_START_TIME; // time at which the text and buttons start to fade in
    const float _FADE_IN_END_TIME; // time at which the text and buttons are fully opaque
    const float _INITIAL_RED_SCREEN_ALPHA; // starting alpha value of the red screen

    // red screen that flashes
    sf::RectangleShape _redScreen;

    // texts
    sf::Text _gameOverText;
    sf::Text _scoreText;
    std::list<sf::Text*> _texts;

    // buttons
    Button _menuButton;
    Button _quitButton;
    std::list<Button*> _buttons;
    ButtonManager _buttonManager;
};

#endif // _GAME_OVER_ACTIVITY_HPP_
