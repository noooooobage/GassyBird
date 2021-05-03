#ifndef _NPC_HPP_
#define _NPC_HPP_

#include <vector>

#include <SFML/Graphics.hpp>

#include "PhysicalActor.hpp"
#include "Resources/SpriteResource.hpp"

/**
 * Parent Class of The NPC people that serve as targets and enemies to the bird. Currently there are
 * two possible NPC types and they are both contained by this parent class, so no subclasses are
 * necessary yet.
 */
class NPC : public PhysicalActor {

//Public
public:

    // defines the possible types of the NPC, for now there are two
    enum class TYPE {
        MALE,
        FEMALE
    };

    // defines various actions that the NPC can do
    enum class ACTION {
        IDLE,
        WALK,
        START_THROW,
        FINISH_THROW
    };

    //Override update method
    void update(const float& timeDelta) override;

    //Override draw method
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    /**
     * Does the specified action after the given delay in seconds has passed. The action will last
     * for the given duration. If the action is FINISH_THROW, then delay and duration are ignored.
     */
    void doAction(const NPC::ACTION& action, const float& delay, const float& duration);

    /**
     * Sets the sprite to face either left or right if faceLeft is true or false, respectively.
     */
    void setFacingLeft(const bool& faceLeft);

    /**
     * Stops the walking animation and returns to the idle state, if the NPC was indeed walking.
     * This may need to be called if the NPC bumps into an obstacle.
     */
    void stopWalking();

    // various accessors
    bool isFacingLeft() { return _isFacingLeft; }
    bool isIdle() { return _state == IDLE; }
    bool isWalking() { return _state == WALKING; }
    bool isThrowing() { return _state == STARTING_THROW || _state == FINISHING_THROW; }
    bool isReadyToFinishThrowing() { return _isReadyToFinishThrowing; }
    NPC::ACTION getNextAction() { return _nextAction; }

    // only allow NPCFactory to access private constructor
    friend class NPCFactory;

    // if the NPC is on the screen, this is set externally by the logic
    bool isVisible;

private:

    // Constructor and initializer are private so that only the NPCFactory is able to make NPCs
    NPC(const NPC::TYPE& type);
    void init();

    /**
     * Walks for the given duration in seconds, in the direction given by walkLeft. Once the NPC has
     * finished walking, it returns to the idle state. This method does nothing if the NPC is
     * throwing something.
     */
    void walk(const float& duration);

    /**
     * Causes the NPC to raise their arm like they're taking aim. This will interrupt any walking
     * that may be taking place. After duration seconds has elapsed, isReadyToFinishThrowing() will
     * return true.
     */
    void startThrowing(const float& duration);

    /**
     * Finishes the throwing animation, then returns to the idle state.
     */
    void finishThrowing();

    /**
     * Transitions to the idle state.
     */
    void toIdle();

    const TYPE _TYPE;

    bool _initialized;

    sf::Sprite _sprite;
    std::vector<sf::IntRect> _textureRects;

    // different possible states
    enum STATE {
        IDLE,
        PREPARING, // about to do an action, but visually is idling
        WALKING,
        STARTING_THROW,
        FINISHING_THROW
    };
    STATE _state;

    // animation stuff
    const float _IDLE_FRAME_DURATION;
    const int _IDLE_START_FRAME;
    const int _NUM_IDLE_FRAMES;

    const float _WALK_FRAME_DURATION;
    const int _WALK_START_FRAME;
    const int _NUM_WALK_FRAMES;

    const float _THROW_FRAME_DURATION;
    const int _START_THROW_START_FRAME;
    const int _NUM_START_THROW_FRAMES;
    const int _FINISH_THROW_START_FRAME;
    const int _NUM_FINISH_THROW_FRAMES;

    float _frameDuration;
    int _startFrame;
    int _numFrames;
    int _currentFrame;
    float _frameTimer;

    // state stuff
    bool _isFacingLeft;
    bool _isReadyToFinishThrowing;
    float _actionTimeRemaining;
    float _prepareTimeRemaining;
    ACTION _nextAction;
    float _nextActionDuration;
};

#endif