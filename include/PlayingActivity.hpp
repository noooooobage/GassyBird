#ifndef _PLAYING_ACTIVITY_HPP_
#define _PLAYING_ACTIVITY_HPP_

#include <SFML/Graphics.hpp>

#include "Activity.hpp"
#include "GameLogic.hpp"
#include "HumanView.hpp"

/**
 * The PlayingActivity is the core activity which is run by the game. It contains sub-activities
 * which overlay various user interfaces on top of the playing activity.
 */
class PlayingActivity : public Activity {

public:

    PlayingActivity();

    void init();

    /**
     * Updates views and then game logic.
     */
    void update(const float& timeDelta) override;

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:

    bool _initialized;

    GameLogic _logic;

    HumanView _humanView;
};

#endif // _PLAYING_ACTIVITY_HPP_
