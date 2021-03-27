#ifndef _PLAYING_MENU_ACTIVITY_HPP_
#define _PLAYING_MENU_ACTIVITY_HPP_

#include "Activity.hpp"

/**
 * Subactivity of PlayingActivity. Displays overlay and handles buttons while the game is playing.
 */
class PlayingMenuActivity : public Activity {

public:

    PlayingMenuActivity() {}

    void update(const float& timeDelta) override;

    void draw(sf::RenderTarget& target) override;
};

#endif // _PLAYING_MENU_ACTIVITY_HPP_
