#ifndef _BUTTON_CLICK_EVENT_HPP_
#define _BUTTON_CLICK_EVENT_HPP_

#include "Event.hpp"
#include "Button.hpp"

/**
 * Happens when a button is clicked. Contains a pointer to the button that was clicked.
 */
class ButtonClickEvent: public Event {

public:

    ButtonClickEvent(const Button& button) : button(&button) {}

    const EventType& getType() const override { return TYPE; }

    static const EventType TYPE;

    const Button* button;
};

#endif // _BUTTON_CLICK_EVENT_HPP_
