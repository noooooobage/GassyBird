#ifndef _WINDOW_CLOSE_EVENT_
#define _WINDOW_CLOSE_EVENT_

#include "Event.hpp"

/**
 * Describes an SFML event whose type is sf::Event::Closed.
 */
class WindowCloseEvent : public Event {

public:

    WindowCloseEvent() {}

    const EventType& getType() const override { return TYPE; }

    static const EventType TYPE;
};

#endif // _WINDOW_CLOSE_EVENT_
