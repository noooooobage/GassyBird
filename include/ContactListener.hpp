#ifndef _CONTACT_LISTENER_HPP_
#define _CONTACT_LISTENER_HPP_

#include <iostream>

#include <box2d/box2d.h>

#include "Globals.hpp"
#include "Events/CollisionEvent.hpp"

/**
 * Custom ContactListener class which implements callbacks for box2d contacts/collisions.
 */
class ContactListener : public b2ContactListener {

public:

    /**
     * Called after a collision is solved. Queues a CollisionEvent containing the two PhysicalActors
     * which were involved in the collision and the position of the collision.
     */
    void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) override {

        // Get the collision's position, which can be approximated using the first point in the
        // contact manifold.
        b2WorldManifold manifold;
        contact->GetWorldManifold(&manifold);
        b2Vec2 position = manifold.points[0];
        
        // create the collision event
        eventMessenger.queueEvent(CollisionEvent(
            (PhysicalActor*)contact->GetFixtureA()->GetBody()->GetUserData().pointer,
            (PhysicalActor*)contact->GetFixtureB()->GetBody()->GetUserData().pointer,
            position
        ));
    }
};

#endif // _CONTACT_LISTENER_HPP_
