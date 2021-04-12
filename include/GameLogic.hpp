#ifndef _GAME_LOGIC_HPP_
#define _GAME_LOGIC_HPP_

#include <memory>
#include <unordered_map>
#include <list>

#include <box2d/box2d.h>

#include "Actor.hpp"
#include "PlayableBird.hpp"
#include "NPC.hpp"
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
     * Returns all visible actors and their corresponding bodies.
     */
    const std::unordered_map<PhysicalActor*, b2Body*> getVisibleActors() const;

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
    b2Body* addToWorld(const PhysicalActor& physical, const b2Vec2& position = {0.0f, 0.0f});

    /**
     * Given a physical actor, return the corresponding body which exists in the physical world. If
     * the actor does not have an associated physical body, then return nullptr.
     */
    b2Body* getBody(const PhysicalActor& actor) const;
    
    /**
     * Updates stuff about the bird, e.g. whether it's pooping, whether it's flying, etc. Also calls
     * the bird's own update() method.
     */
    void updatePlayableBird(const float& timeDelta);

    /**
     * Updates the ground obstacles so that the ground is always visible.
     */
    void updateGround();

    /**
     * Increases the world scroll speed by the specified amount (can be negative to also decrease
     * the speed). This will affect the liear velocity of all bodies except the bird.
     */
    void increaseScrollSpeed(const float& amount);

    //Spawn an NPC into the world
    b2Body* spawnNPC();

    bool _initialized;

    // different possible states
    enum STATE {DEMO, PLAYING, GAME_OVER};
    STATE _state;

    // physical world
    std::shared_ptr<b2World> _world;
    const b2Vec2 _GRAVITY;
    const float _INITIAL_WORLD_SCROLL_SPEED;
    float _world_scroll_speed; // Effectively the bird's horizontal speed (meters per second) --
                               // increasing this speed makes objects move faster to the left.

    // playable bird stuff
    PlayableBird _playableBirdActor;
    b2Body* _playableBirdBody;
    const float _BIRD_POOP_DURATION; // player must wait for this amount until they can poop again
    const int _BIRD_MAX_POOPS; // max number of poops that the bird can do in a row
    float _timeSinceLastPoop; // time elapsed since last poop
    int _numPoopsLeft; // number of poops the bird has left

    // ground stuff
    const int _NUM_GROUNDS; // the overall ground is made up of mutiple ground obstacles
    const float _GROUND_WIDTH_METERS; // width of each ground obstacle in meters
    const float _GROUND_OFFSET_METERS; // amount which the ground protrudes from bottom of screen
    std::list<Obstacle> _grounds; // list of all ground obstacles
    std::list<Obstacle> _obstacles; // list of all obstacles except for the ground
    
    // stores all physical actors, maps them to their physical bodies
    std::unordered_map<PhysicalActor*, b2Body*> _physicalActors;
};

#endif // _GAME_LOGIC_HPP_
