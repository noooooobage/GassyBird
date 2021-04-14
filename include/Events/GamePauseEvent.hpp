#ifndef _GAME_PAUSE_EVENT_HPP_
#define _GAME_PAUSE_EVENT_HPP_

#include "Event.hpp"

/**
 * Triggering this event means the game should either be paused or unpaused -- the specific action
 * is determined by the action attribute.
 */
class GamePauseEvent : public Event {

public:

    // the action can either be pausing or unpausing
    enum class ACTION {PAUSE, UNPAUSE};

    /**
     * Creates a GamePauseEvent.
     * @param action whether the action should be PAUSE or UNPAUSE
     */
    GamePauseEvent(const ACTION& action) : action(action) {}

    const EventType& getType() const override { return TYPE; }

    static const EventType TYPE;

    const ACTION action;
};

#endif // _GAME_PAUSE_EVENT_HPP_
