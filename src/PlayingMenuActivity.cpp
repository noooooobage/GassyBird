#include <iostream>
#include <cassert>
#include <string>

#include "PlayingMenuActivity.hpp"
#include "GameLogic.hpp"
#include "Globals.hpp"
#include "Resources/FontResource.hpp"

PlayingMenuActivity::PlayingMenuActivity() :
    _initialized(false)
{}

void PlayingMenuActivity::init(const GameLogic& logic) {
    
    _logic = &logic;

    // retrieve the font for the texts
    const sf::Font& font = resourceCache.getResource<FontResource>("ARCADE_FONT")->font;

    // set up poops left text
    _poopsLeftText.setFont(font);
    _poopsLeftText.setFillColor(sf::Color::Black);
    _poopsLeftText.setCharacterSize(40);
    _poopsLeftText.setPosition(10.0f, 0.0f);

    // set up score text
    _scoreText.setFont(font);
    _scoreText.setFillColor(sf::Color::Black);
    _scoreText.setCharacterSize(40);
    _scoreText.setPosition(NATIVE_RESOLUTION.x - 150.0f, 0.0f);

    _initialized = true;
}

void PlayingMenuActivity::update(const float& timeDelta) {
    
    assert(_initialized);

    // update both texts
    _poopsLeftText.setString("POOPS   LEFT  " + std::to_string(_logic->getNumPoopsLeft()));
    _scoreText.setString("SCORE  " + std::to_string(_logic->getPlayerScore()));
}

void PlayingMenuActivity::draw(sf::RenderTarget& target) {
    
    assert(_initialized);

    // draw both texts
    target.draw(_poopsLeftText);
    target.draw(_scoreText);
}
