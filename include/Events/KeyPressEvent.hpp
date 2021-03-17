#ifndef _KEY_PRESS_EVENT
#define _KEY_PRESS_EVENT
#include <SFML/Graphics.hpp>

class KeyPressEvent: public Event {
public:
    KeyPressEvent(const sf::Keyboard::Key key) {}

    const EventType& getType() const override { return TYPE; }

    static const EventType TYPE;

    sf::Keyboard::Key keyPress;
};
#endif