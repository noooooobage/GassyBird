#ifndef _PLAYING_ACTIVITY_HPP_
#define _PLAYING_ACTIVITY_HPP_

#include <SFML/Graphics.hpp>

#include "Activity.hpp"
#include "GameLogic.hpp"
#include "HumanView.hpp"
#include "NPCView.hpp"
#include "DebugDrawer.hpp"
#include "MainMenuActivity.hpp"
#include "PlayingMenuActivity.hpp"
#include "GameOverActivity.hpp"
#include "EventListener.hpp"

/**
 * The PlayingActivity is the core activity which is run by the game. It contains sub-activities
 * which overlay various user interfaces on top of the playing activity.
 */
class PlayingActivity : public Activity {

public:

    PlayingActivity();

    ~PlayingActivity();

    /**
     * Initializes with a render target.
     */
    void init(sf::RenderTarget& target);

    /**
     * Updates views and then game logic.
     */
    void update(const float& timeDelta) override;

    void draw(sf::RenderTarget& target) override;

    /**
     * Methods to transition to different subactivities. These also affect the state of the logic.
     */
    void toMain();
    void toPlaying();
    void toGameOver(const Event& event); // this one is actually an event listener

private:

    bool _initialized;
    
    // event listener
    EventListener _gameOverListener;

    DebugDrawer _debugDrawer;

    GameLogic _logic;

    HumanView _humanView;
    NPCView _npcView;

    // PlayingActivity has activities of its own, these act as user interfaces
    MainMenuActivity _mainMenuActivity;
    PlayingMenuActivity _playingMenuActivity;
    GameOverActivity _gameOverActivity;
    Activity* _currentActivity;
};

#endif // _PLAYING_ACTIVITY_HPP_
