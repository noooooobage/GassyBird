#ifndef _PLAYING_MENU_ACTIVITY_HPP_
#define _PLAYING_MENU_ACTIVITY_HPP_

#include <SFML/Graphics.hpp>

#include "Activity.hpp"
#include "GameLogic.hpp"

/**
 * Subactivity of PlayingActivity. Displays the score, number of poops left, and handles buttons
 * while the game is playing.
 */
class PlayingMenuActivity : public Activity {

public:

    PlayingMenuActivity();

    void init(const GameLogic& logic);

    void update(const float& timeDelta) override;

    void draw(sf::RenderTarget& target) override;

private:

    bool _initialized;

    const GameLogic* _logic;

    sf::Text _poopsLeftText;
    sf::Text _scoreText;
};

#endif // _PLAYING_MENU_ACTIVITY_HPP_
