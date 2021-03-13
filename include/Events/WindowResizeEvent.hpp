#ifndef _WINDOW_RESIZE_EVENT_
#define _WINDOW_RESIZE_EVENT_

#include <SFML/Graphics.hpp>

#include "Event.hpp"

/**
 * Describes an SFML event whose type is sf::Event::Resized.
 */
class WindowResizeEvent : public Event {

public:

    WindowResizeEvent(const sf::Event& sfmlResizeEvent) :
        width(sfmlResizeEvent.size.width),
        height(sfmlResizeEvent.size.height)
    {}

    const EventType& getType() const override { return TYPE; }

    static const EventType TYPE;
    
    int width;  // width of the window after resize in pixels
    int height; // height of the window after resize in pixels
};

#endif // _WINDOW_RESIZE_EVENT_
