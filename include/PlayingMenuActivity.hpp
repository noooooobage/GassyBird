#ifndef _PLAYING_MENU_ACTIVITY_HPP_
#define _PLAYING_MENU_ACTIVITY_HPP_

#include <list>
#include <vector>

#include <SFML/Graphics.hpp>

#include "Activity.hpp"
#include "GameLogic.hpp"
#include "Button.hpp"
#include "ButtonManager.hpp"
#include "EventListener.hpp"
#include "Event.hpp"

/**
 * Subactivity of PlayingActivity. Displays the score, number of poops left, and handles buttons
 * while the game is playing.
 */
class PlayingMenuActivity : public Activity {

public:

    PlayingMenuActivity();

    ~PlayingMenuActivity();

    /**
     * Initializes with the parent PlayingActivity and the GameLogic.
     * @param logic 
     */
    void init(class PlayingActivity& playingActivity, const GameLogic& logic);

    /**
     * Activates the activity such that buttons are able to be interacted with.
     */
    void activate() override;

    /**
     * Deactivates the activity. Buttons are not able to be interacted with, and no other methods
     * may be called before the activity is activated again.
     */
    void deactivate() override;

    void update(const float& timeDelta) override;

    void draw(sf::RenderTarget& target) override;

private:

    /**
     * Handles a ButtonClickEvent. Performs various actions such as pausing, unpausing, returning to
     * the main menu, exiting, etc.
     */
    void buttonClickHandler(const Event& event);

    /**
     * Called when a GamePauseEvent is triggered. If the action is PAUSE, then display the buttons
     * and other visual objects that should appear when the game is paused. If the action is
     * UNPAUSE, then make the aformentioned objects invisible an uniteractible.
     */
    void gamePauseHandler(const Event& event);

    bool _initialized;
    bool _activated;

    // event listeners
    EventListener _buttonClickListener;
    EventListener _gamePauseListener;

    class PlayingActivity* _playingActivity;
    const GameLogic* _logic;

    // indicators -- the top one is for the first poop, the bottom for the second
    sf::Sprite _topPoopIndicator;
    sf::Sprite _bottomPoopIndicator;
    std::vector<sf::IntRect> _indicatorRects;
    std::vector<sf::Sprite*> _indicators;

    // shows how much time is left before the bird dies from not pooping
    sf::RectangleShape _poopTimeLeftBar;
    sf::RectangleShape _poopTimeLeftOutline;
    
    // texts
    sf::Text _scoreText;
    sf::Text _pausedText;

    // variables for handling animation when the score changes
    const float _SCORE_CHANGE_TIME;
    float _scoreChangeTimer;
    const sf::Color _SCORE_BASE_COLOR;
    const sf::Color _SCORE_CHANGE_COLOR;
    const float _SCORE_CHANGE_SCALE;

    // buttons to display all the time
    Button _pauseButton;
    ButtonManager _playingButtonManager;

    // buttons to display only when paused
    Button _resumeButton;
    Button _menuButton;
    Button _quitButton;
    std::list<Button*> _pausedButtons;
    ButtonManager _pausedButtonManager;
};

#endif // _PLAYING_MENU_ACTIVITY_HPP_
