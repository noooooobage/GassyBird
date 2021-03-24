
#include <iostream>
#include "Globals.hpp"
#include "MenuActivity.hpp"
#include "EventListener.hpp"
#include "Events/ButtonPressEvent.hpp"

MenuActivity::MenuActivity() : _initialized(false) {

}

void MenuActivity::init() {
    _initialized = true;
    std::cout << "Initialized MenuActivity" << std::endl;
    _logic.init();
    // initialize views with logic
    Button butt(350, 650, 450, 550);
    _goToPlaying = butt;
    _humanView.init(&_logic);
}

void MenuActivity::update(const float& timeDelta) {

}

void MenuActivity::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    assert(_initialized);

    // draw a blue rectangle
    // TODO: remove this at some point
    sf::RectangleShape rect((sf::Vector2f)NATIVE_RESOLUTION);
    rect.setFillColor(sf::Color::Red);
    target.draw(rect);

    _goToPlaying.draw(target);

}

