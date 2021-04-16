#ifndef _COLLISION_EVENT_HPP_
#define _COLLISION_EVENT_HPP_

#include <box2d/box2d.h>

#include "Event.hpp"
#include "PhysicalActor.hpp"

/**
 * Describes a collision event between two physical actors. These actors are accessible through the
 * members actorA and actorB (in no particular order). There is also a position member which
 * describes the approximate position of the collision.
 */
class CollisionEvent: public Event {

public:

    CollisionEvent(
        const PhysicalActor* actorA,
        const PhysicalActor* actorB,
        const b2Vec2& position
    ) :
        actorA(actorA),
        actorB(actorB),
        position(position)
    {}

    const EventType& getType() const override { return TYPE; }

    static const EventType TYPE;

    const PhysicalActor* actorA;
    const PhysicalActor* actorB;
    const b2Vec2 position;
};

#endif // _COLLISION_EVENT_HPP_
