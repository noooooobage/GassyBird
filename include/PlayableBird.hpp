#ifndef _PLAYABLE_BIRD_HPP_
#define _PLAYABLE_BIRD_HPP_

#include <vector>

#include <SFML/Graphics.hpp>

#include "PhysicalActor.hpp"
#include "Resources/SpriteResource.hpp"

/**
 * Represents the main bird character which the player controls.
 */
class PlayableBird : public PhysicalActor {

public:

    PlayableBird();

    void update(const float& timeDelta) override;

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    /**
     * Sets the sprite of the bird from a SpriteResource.
     */
    void setSprite(SpriteResource* spriteResource);

private:

    // if the sprite has been set or not, make sure this is set before updating or drawing
    bool _spriteSet;

    // sprite to be used and its texture rectangles
    sf::Sprite _sprite;
    std::vector<sf::IntRect> _textureRects;

    // index of the frame of the sprite to draw; different frames refer to different texture rects
    int _spriteCurrentFrame;

    // the sprite's frame will change after this amount of time in seconds passes
    const float _FRAME_CHANGE_TIME_DELTA;
    float _frameChangeTimer;

    const float _WIDTH_METERS; // ideal width in meters
    const float _WIDTH_PIXELS; // width in pixels after scaling
};

#endif // _PLAYABLE_BIRD_HPP_
