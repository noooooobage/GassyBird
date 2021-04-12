#ifndef _PLAYING_ACTIVITY_HPP_
#define _PLAYING_ACTIVITY_HPP_

#include <SFML/Graphics.hpp>

#include "Activity.hpp"
#include "GameLogic.hpp"
#include "HumanView.hpp"
#include "DebugDrawer.hpp"
#include "MainMenuActivity.hpp"
#include "PlayingMenuActivity.hpp"

/**
 * The PlayingActivity is the core activity which is run by the game. It contains sub-activities
 * which overlay various user interfaces on top of the playing activity.
 */
class PlayingActivity : public Activity {

public:

    PlayingActivity();

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

private:

    bool _initialized;

    DebugDrawer _debugDrawer;

    GameLogic _logic;

    HumanView _humanView;

    // PlayingActivity has activities of its own, these act as user interfaces
    MainMenuActivity _mainMenuActivity;
    PlayingMenuActivity _playingMenuActivity;
    Activity* _currentActivity;
};

#endif // _PLAYING_ACTIVITY_HPP_
