#ifndef _GAME_OVER_EVENT_HPP_
#define _GAME_OVER_EVENT_HPP_

#include "Event.hpp"

/**
 * Triggered when the game ends.
 */
class GameOverEvent : public Event {

public:

    GameOverEvent() {}

    const EventType& getType() const override { return TYPE; }

    static const EventType TYPE;
};

#endif // _GAME_OVER_EVENT_HPP_
