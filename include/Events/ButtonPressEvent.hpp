#ifndef _BUTTON_PRESS_EVENT
#define _BUTTON_PRESS_EVENT

#include <SFML/Graphics.hpp>
#include "Button.hpp"

class ButtonPressEvent: public Event {
public:
    ButtonPressEvent(int x, int y) {xCoord = x, yCoord = y; }

    const EventType& getType() const override { return TYPE; }

    const Button& getButton() {return button; }

    static const EventType TYPE;

    Button button;
    int xCoord, yCoord;
};
#endif