#ifndef _GAME_LOGIC_HPP_
#define _GAME_LOGIC_HPP_

#include <memory>
#include <unordered_map>
#include <list>
#include <iostream>

#include <box2d/box2d.h>

#include "Actor.hpp"
#include "PlayableBird.hpp"
#include "NPC.hpp"
#include "PhysicalActor.hpp"
#include "DebugDrawer.hpp"
#include "Obstacle.hpp"
#include "EventListener.hpp"
#include "Event.hpp"
#include "ContactListener.hpp"

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
    void toGameOver();

    /**
     * Returns true if the game is paused, returns false otherwise.
     */
    bool isPaused() const;

    /**
     * Methods through which the debug drawer can set and exectue drawing.
     */
    void setDebugDrawer(DebugDrawer& debugDrawer);
    void debugDraw();

    /**
     * Returns all visible actors, in the order they should be drawn.
     */
    const std::list<PhysicalActor*>& getVisibleActors() const;

    /**
     * Returns all NPCs.
     */
    const std::list<std::shared_ptr<NPC>>& getNPCs() const;

    /**
     * Given a physical actor, return the corresponding body which exists in the physical world. If
     * the actor does not have an associated physical body, then return nullptr.
     */
    b2Body* getBody(const PhysicalActor* actor) const;

    // various getters
    int getNumPoopsLeft() const;
    float getPoopTimeLeft() const; // returns the value scaled to [0, 1]
    int getPlayerScore() const;
    float getDifficulty() const;

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

    /**
     * Called by NPCView to cause the specified NPC to do the specified action. The action starts
     * once `delay` seconds has elapsed, and that action lasts for `duration` seconds.
     */
    void requestNPCAction(NPC& npc, const NPC::ACTION& action, const float& delay,
            const float& duration);
    
    // Store collision filtering info here so other classes may access them. The bird and rock don't
    // collide with the npcGround, but they do collide with each other. The splatter doesn't
    // collide with the bird.
    static const int BIRD_CATEGORY_BIT =     0x2;
    static const int ROCK_CATEGORY_BIT =     0x4;
    static const int SPLATTER_CATEGORY_BIT = 0x8;

private:

    /**
     * Handler callbacks for event listeners.
     */
    void gamePauseHandler(const Event& event);
    void collisionHandler(const Event& event);

    /**
     * Helper functions which handle collisions with specific types of physical actors.
     */
    void handlePoopCollision(const CollisionEvent& e);
    void handleBirdCollision(const CollisionEvent& e);

    /**
     * Spawns the first physical actors into existence, i.e. creates the ground and sprinkles some
     * other entities in there as well. This does not add the playable bird to the world, so need to
     * do that separately.
     */
    void createMap();

    /**
     * Removes physical actors that are past a certain threshold to the left of the screen. This
     * does not affect the bird or the ground.
     */
    void removeOutOfBoundsActors();

    /**
     * Procedurally generates new physical actors, e.g. Obstacles and NPCs to the right of the
     * screen.
     */
    void generateNewActors(const float& timeDelta);

    /**
     * Helper method to generateNewActors(). Spawns a random Non-Playable Entity (Obstacle or NPC)
     * at the given position in the world. Also updates _rightmostObstacleLocation according to the
     * right edge of the spawned obstacle.
     */
    void spawnNPE(b2Vec2 position);

    /**
     * Creates a b2Body from the given physical actor, and adds it to the box2d world and to the
     * physical actors map. The user data of the created box2d body is set to the address of the
     * given actor.
     * 
     * @param actor the PhysicalActor to add to the world
     * @param position position at which the body is placed, defaults to (0, 0)
     * @param inheritWorldScroll whether or not to inherit the world scroll speed, defaults to true
     * @param drawInFront the added actor will be drawn in front of all other actors if true. If
     *                    false, then it's drawn behind. Defaults to true.
     * 
     * @return a pointer to the newly created body
     */
    b2Body* addToWorld(const PhysicalActor& actor, const b2Vec2& position = {0.0f, 0.0f},
            bool inheritWorldScroll = true, bool drawInFront = true);
    
    /**
     * Wipes the given actor from existence. More specifically, does the following:
     * - Destroys the actor's assiciated box2d body
     * - Removes the actor from the _physicalActors map
     * - Frees the actor's memory in whatever list it's stored in and removes it from the list
     * 
     * If the actor doesn't exist in the _physicalActors map, then it's ignored.
     * 
     * WARNING: Be careful, don't call this method while iterating through the _physicalActors map
     * or any of the physical actor lists like _obstacles, _grounds, etc, as those structures are
     * modified by this method.
     */
    void removeFromWorld(const PhysicalActor& actor);

    /**
     * Calls removeFromWorld() on every actor in the _physicalActors map.
     */
    void removeAllFromWorld();

    /**
     * Searches for the shared pointer which holds the given actor in the given list of shared
     * pointers. It frees the memory of all matches and removes the entries from the list.
     */
    template <typename T>
    void removeFromList(const PhysicalActor& actor, std::list<std::shared_ptr<T>>& list) {

        const T* actorAddress = (T*)&actor;

        for (auto i = list.begin(); i != list.end();) {
            if ((*i).get() == actorAddress) {
                i->reset();
                i = list.erase(i);
            } else {
                ++i;
            }
        }
    }
    
    /**
     * Updates stuff about the bird, e.g. whether it's pooping, whether it's flying, etc. Also calls
     * the bird's own update() method.
     */
    void updatePlayableBird(const float& timeDelta);

    /**
     * Sets the velocities of the NPCs based on their movement status.
     */
    void updateNPCs(const float& timeDelta);

    /**
     * Updates the ground obstacles so that the ground is always visible. If any of the ground
     * obstacles are behind the screen by a certain threshold, then they are placed to the right of
     * the rightmost ground.
     */
    void updateGround();

    /**Difficulty is calculated based on the total time elapsed
    */
    void updateDifficulty();

    /**
     * Sets the world scroll speed based on the current difficulty.
     */
    void setWorldScrollSpeed();

    bool _initialized;

    // event listeners
    EventListener _gamePauseListener;
    EventListener _collisionListener;

    // contact listener which creates CollisionEvents, this is NOT an EventListener
    ContactListener _contactListener;

    // different possible states
    enum STATE {DEMO, PLAYING, GAME_OVER};
    STATE _state;

    // if the game is paused, this is separate from the states above
    bool _isPaused;

    // physical world
    std::shared_ptr<b2World> _world;
    const b2Vec2 _GRAVITY;
    const float _MIN_WORLD_SCROLL_SPEED; // <- when the difficulty is lowest
    const float _MAX_WORLD_SCROLL_SPEED; // <- when the difficulty is highest
    const float _STEP_TIME;
    const float _ACTION_TIME;
    float _worldScrollSpeed; // Effectively the bird's horizontal speed (meters per second) --
                             // increasing this speed makes objects move faster to the left.

    // ground stuff
    const int _NUM_GROUNDS; // the overall ground is made up of mutiple ground obstacles
    const float _GROUND_WIDTH_METERS; // width of each ground obstacle in meters
    const float _GROUND_OFFSET_METERS; // amount which the ground protrudes from bottom of screen
    const float _BIG_GROUND_WIDTH_METERS;
    std::shared_ptr<Obstacle> _npcGround; // the ground which NPCs are on
    std::list<std::shared_ptr<Obstacle>> _grounds; // list of all non-big ground obstacles

    // playable bird stuff
    PlayableBird _playableBirdActor;
    b2Body* _playableBirdBody;
    const b2Vec2 _BIRD_DEMO_POSITION; // position of the bird in DEMO mode
    const float _BIRD_MAX_HEIGHT; // highest y-position that the bird can reach
    const float _BIRD_SLOW_HEIGHT; // height at which the bird starts slowing down
    const float _BIRD_MAX_VELOCITY; // fastest the bird can move in the y-direction
    const float _BIRD_POOP_DURATION; // player must wait for this amount until they can poop again
    const float _BIRD_DEATH_TIME;
    const int _BIRD_MAX_POOPS; // max number of poops that the bird can do in a row
    const float _POOP_DOWNWARD_VELOCITY; // a new poop will move downward away from the bird
    float _timeSinceLastPoop; // time elapsed since last poop
    int _numPoopsLeft; // number of poops the bird has left
    PhysicalActor* _lastPoop; // pointer to the most recent poop that the bird made; NEVER
                              // DEREFERENCE THIS!! for comparison purposes only

    // how many times the bird has successfully pooped on an NPC
    int _playerScore;

    // difficulty goes from 0 (easiest) to 1 (hardest) as time progresses
    float _difficulty;
    const int _MAX_DIFFICULTY_TIME; // time at which the difficulty is maximum

    int _lastObstacleSpawned; //index of last obstacle spawned

    const float _SPAWN_LOCATION_X; // x-coordinate where things are spawned
    float _rightmostObstacleLocation; // position after which it's safe to spawn
    double _totalTimePassed;
    double _playingTimePassed;

    // list of all obstacles except for the ground
    std::list<std::shared_ptr<Obstacle>> _obstacles;

    // NPC stuff
    std::list<std::shared_ptr<NPC>> _NPCs;
    const float _NPC_WALK_SPEED;
    const float _NPC_THROW_SPEED;
    const float _NO_THROW_ZONE_LEFT; // <- where the npc is unable to throw relative to bird
    const float _NO_THROW_ZONE_RIGHT;

    // list of all projectiles
    std::list<std::shared_ptr<Obstacle>> _projectiles;
    
    // stores all physical actors, maps them to their physical bodies
    std::unordered_map<PhysicalActor*, b2Body*> _physicalActors;

    // stores all visible actors -- actors appearing earlier get drawn first
    std::list<PhysicalActor*> _visibleActors;
};

#endif // _GAME_LOGIC_HPP_
