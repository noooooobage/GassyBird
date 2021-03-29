#ifndef _MOUSE_PRESS_EVENT_HPP_
#define _MOUSE_PRESS_EVENT_HPP_

#include <SFML/Graphics.hpp>

#include "Event.hpp"

/**
 * Describes an SFML MousePressEvent. Contains both the pixel coordinates (relative to window) and
 * the graphical world coordinates (relative to window's current view).
 */
class MousePressEvent : public Event {

public:

    MousePressEvent(const sf::Mouse::Button& button, const sf::Vector2i& pixelCoord,
            const sf::Vector2f& graphicalCoord) :
        button(button),
        pixel(pixelCoord),
        graphical(graphicalCoord)
    {}

    const EventType& getType() const override { return TYPE; }

    static const EventType TYPE;

    const sf::Mouse::Button button; // which mouse button was pressed
    const sf::Vector2i pixel; // pixel coordinate relative to window
    const sf::Vector2f graphical; // graphical world coordinate relative to window's view
};

#endif // _MOUSE_PRESS_EVENT_HPP_
