#ifndef _GAME_LOGIC_HPP_
#define _GAME_LOGIC_HPP_

#include <memory>
#include <unordered_map>

#include <box2d/box2d.h>
#include "PlayableBird.hpp"
#include "PhysicalActor.hpp"

/**
 * Encodes the mechanics of the game and stores actors with physical properties. Provides an API
 * which views can use to influence actors.
 */
class GameLogic {

public:

    GameLogic();

    ~GameLogic();

    void init();

    /**
     * Advances the state of the game forward in time by the amount given by timeDelta.
     */
    void update(const float& timeDelta);

    /**
     * Given a physical actor, return the corresponding body which exists in the physical world. If
     * the actor does not have an associated physical body, then return nullptr.
     */
    const b2Body* getBody(const PhysicalActor& actor);

    PlayableBird& getPlayableBird() { return _playableBirdActor; }

    /**
     * These methods are called by the HumanView to start and stop the bird from flying. When the
     * bird is flying, an upward force is applied to it. When it is not flying, gravity makes the
     * bird move downward.
     */
    void requestBirdStartFly();
    void requestBirdStopFly();
    void requestBirdPoop();
private:

    /**
     * Creates a b2Body from the given physical actor and adds it to the world at the specified
     * position (defaults to 0,0). Also updates the body map.
     * 
     * Returns a pointer to the newly created body.
     */
    b2Body* addToWorld(const PhysicalActor& physical,
            const b2Vec2& position = {0.0f, 0.0f});

    bool _initialized;

    // physical world
    std::shared_ptr<b2World> _world;

    // playable bird stuff
    PlayableBird _playableBirdActor;
    b2Body* _playableBirdBody;
    const float _BIRD_FLIGHT_FORCE; // upward force to apply to the bird's body when bird is flying

    // maps actor addresses to physical bodies
    std::unordered_map<void*, b2Body*> _actorToBody;

    const b2Vec2 _GRAVITY;

};

#endif // _GAME_LOGIC_HPP_
