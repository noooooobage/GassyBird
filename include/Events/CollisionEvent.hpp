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
        PhysicalActor* actorA,
        PhysicalActor* actorB,
        const b2Vec2& position
    ) :
        actorA(actorA),
        actorB(actorB),
        position(position)
    {}

    const EventType& getType() const override { return TYPE; }

    /**
     * Returns true if at least one of the physical actors involved in the collision is of the given
     * type; returns false otherwise.
     */
    bool involvesType(const PhysicalActor::TYPE& type) const {
        return actorA->getType() == type || actorB->getType() == type;
    }

    /**
     * Returns true if both given types match the types of the physical actors involved in the
     * collision; returns false otherwise.
     */
    bool involvesTypes(const PhysicalActor::TYPE& type1, const PhysicalActor::TYPE& type2) const {
        return (
            actorA->getType() == type1 && actorB->getType() == type2 ||
            actorA->getType() == type2 && actorB->getType() == type1
        );
    }

    static const EventType TYPE;

    PhysicalActor* actorA;
    PhysicalActor* actorB;
    const b2Vec2 position;
};

#endif // _COLLISION_EVENT_HPP_
