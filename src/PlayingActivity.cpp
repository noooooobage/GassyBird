#include <cassert>

#include <SFML/Graphics.hpp>

#include "PlayingActivity.hpp"
#include "Globals.hpp"
#include "Resources/SpriteResource.hpp"

PlayingActivity::PlayingActivity() :
    _initialized(false)
{}

void PlayingActivity::init() {

    _initialized = true;

    // initialize logic
    _logic.init();

    // initialize views with logic
    _humanView.init(&_logic);
}

void PlayingActivity::update(const float& timeDelta) {

    assert(_initialized);

    // update views
    _humanView.update(timeDelta);

    // update logic
    _logic.update(timeDelta);
}

void PlayingActivity::draw(sf::RenderTarget& target, sf::RenderStates states) const {

    assert(_initialized);

    // draw a blue rectangle
    // TODO: remove this at some point
    sf::RectangleShape rect((sf::Vector2f)NATIVE_RESOLUTION);
    rect.setFillColor(sf::Color::Blue);
    target.draw(rect);

    // draw the human view
    target.draw(_humanView, states);
}
