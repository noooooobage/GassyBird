#ifndef _GAME_LOGIC_HPP_
#define _GAME_LOGIC_HPP_

#include <memory>
#include <unordered_map>
#include <list>

#include <box2d/box2d.h>

#include "Actor.hpp"
#include "PlayableBird.hpp"
#include "NPC.hpp"
#include "NPCFactory.hpp"
#include "PhysicalActor.hpp"
#include "DebugDrawer.hpp"
#include "Obstacle.hpp"





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
     * Methods to transition to different states.
     */
    void toDemo();
    void toPlaying();

    /**
     * Methods through which the debug drawer can set and exectue drawing.
     */
    void setDebugDrawer(DebugDrawer& debugDrawer);
    void debugDraw();

    /**
     * Given a physical actor, return the corresponding body which exists in the physical world. If
     * the actor does not have an associated physical body, then return nullptr.
     */
    const b2Body* getBody(const PhysicalActor& actor) const;

    /**
     * Returns all visible actors and their corresponding bodies.
     */
    const std::unordered_map<PhysicalActor*, b2Body*> getVisibleActors() const;

    NPC& getNPC() {return _NPCActor; }

    void clearWorld();

    //Spawn an NPC into the world
    void createMap();



    /**
     * These methods are called by the HumanView to start and stop the bird from flying. When the
     * bird is flying, an upward force is applied to it. When it is not flying, gravity makes the
     * bird move downward.
     */
    void requestBirdStartFly();
    void requestBirdStopFly();

    /**
     * Called by the HumanView to cause the bird to poop.
     */
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
    
    void removeFromWorld(const PhysicalActor& physical);
    /**
     * Updates stuff about the bird, e.g. whether it's pooping, whether it's flying, etc. Also calls
     * the bird's own update() method.
     */
    void updatePlayableBird(const float& timeDelta);

    bool _initialized;

    // different possible states
    enum STATE {DEMO, PLAYING, GAME_OVER};
    STATE _state;

    // physical world
    std::shared_ptr<b2World> _world;
    const b2Vec2 _GRAVITY;
    float _world_scroll_speed; // effectively the bird's horizontal speed (meters per second)

    // playable bird stuff
    PlayableBird _playableBirdActor;
    b2Body* _playableBirdBody;
    const float _BIRD_POOP_DURATION; // player must wait for this amount until they can poop again
    const int _BIRD_MAX_POOPS; // max number of poops that the bird can do in a row
    float _timeSinceLastPoop; // time elapsed since last poop
    int _numPoopsLeft; // number of poops the bird has left

    // list of all obstacles
    std::list<Obstacle> _obstacles;
    //NPC Stuff
    std::list<NPC> _Entities; //Create a list and store pointers to NPC objects and obstacles
    NPC _NPCActor;
    b2Body* _NPCBody;

    // stores all physical actors, maps them to their physical bodies
    std::unordered_map<PhysicalActor*, b2Body*> _physicalActors;
};

#endif // _GAME_LOGIC_HPP_
