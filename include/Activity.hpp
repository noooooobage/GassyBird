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
     * Step the activity forward in time according to the given timeDelta (in seconds).
     */
    virtual void update(const float& timeDelta) = 0;

    /**
     * Override of sf::Drawable::draw(). Draws the screen onto the given target.
     */
    virtual void draw(sf::RenderTarget& target) = 0;
    
};

#endif // _ACTIVITY_HPP_
