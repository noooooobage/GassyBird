#ifndef _ACTOR_HPP_
#define _ACTOR_HPP_

#include <SFML/Graphics.hpp>

/**
 * An Actor is an entity that exists in the game that knows how to update it's local state and draw
 * itself on the screen (however, Actors don't have to be visible).
 */
class Actor : public sf::Drawable {

public:

    virtual ~Actor() {}

    /**
     * Does everything necessary to update this actor's state within its own scope. This method does
     * not need to know anything about any other actors, the game state, the position of the mouse,
     * or anything else that is outside the scope of this actor. That stuff will be updated by some
     * other object that has greater knowledge of the game, e.g. the GameLogic.
     * 
     * Example of how this method should be used: advancing the actor's sprite animation according
     * to its internal state.
     * 
     * Example of how this method should not be used: changing the actor's state based on the
     * position of the mouse.
     */
    virtual void update(const float& timeDelta) {}

    /**
     * Draws the actor onto the given render target; therefore, everything needed to know how to
     * draw the actor should be stored in the derived class's members. This method does not update
     * the actor's state.
     */
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override {}
};

#endif // _ACTOR_HPP_
