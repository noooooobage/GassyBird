#include <cassert>

#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>

#include "PlayingActivity.hpp"
#include "Globals.hpp"
#include "Resources/SpriteResource.hpp"

PlayingActivity::PlayingActivity() :
    _initialized(false)
{}

void PlayingActivity::init(sf::RenderTarget& target) {

    _initialized = true;

    // initialize logic; if in DEBUG mode, also set its debug drawer
    _logic.init();
    if (DEBUG) {
        _debugDrawer.init(target);
        _debugDrawer.SetFlags(b2Draw::e_shapeBit);
        _logic.setDebugDrawer(_debugDrawer);
    }

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

void PlayingActivity::draw(sf::RenderTarget& target) {

    assert(_initialized);

    // draw a blue rectangle
    // TODO: remove this at some point
    sf::RectangleShape rect((sf::Vector2f)NATIVE_RESOLUTION);
    rect.setFillColor(sf::Color::Blue);
    target.draw(rect);

    // draw the human view
    target.draw(_humanView);

    // if in DEBUG mode, call the logic's debug draw
    if (DEBUG)
        _logic.debugDraw();
}
