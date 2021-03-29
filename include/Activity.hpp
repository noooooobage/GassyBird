#ifndef _ACTIVITY_HPP_
#define _ACTIVITY_HPP_

#include <SFML/Graphics.hpp>

/**
 * An Activity describes one "screen" of the game. Classes that implement the Activity interface
 * are able to update themselves (step their internal state forward) and draw themselves. Activities
 * can contain other Activities. This may be desirable if a UI or HUD should be displayed on top of
 * the current screen.
 */
class Activity {

public:

    virtual ~Activity() {}

    /**
     * Activate and deactivate the activity. This is useful when switching between activities.
     * Subclasses do not have to implement these methods. E.g. the PlayingActivity doesn't need
     * these methods because it is the only activity that directly runs from Game; therefore, Game
     * never needs to switch to or from PlayingActivity.
     */
    virtual void activate() {}
    virtual void deactivate() {}

    /**
     * Step the activity forward in time according to the given timeDelta (in seconds).
     */
    virtual void update(const float& timeDelta) = 0;

    /**
     * Draws the screen onto the given target.
     */
    virtual void draw(sf::RenderTarget& target) = 0;
    
};

#endif // _ACTIVITY_HPP_
