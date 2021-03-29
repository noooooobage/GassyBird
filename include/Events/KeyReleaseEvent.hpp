#ifndef _KEY_RELEASE_EVENT
#define _KEY_RELEASE_EVENT
#include <SFML/Graphics.hpp>

class KeyReleaseEvent: public Event {
public:
    KeyReleaseEvent(const sf::Keyboard::Key key) : key(key) {}

    const EventType& getType() const override { return TYPE; }

    static const EventType TYPE;

    const sf::Keyboard::Key key;
};
#endif