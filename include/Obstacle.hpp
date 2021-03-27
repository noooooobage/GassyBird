#ifndef _OBSTACLE_HPP_
#define _OBSTACLE_HPP_

#include <vector>

#include <SFML/Graphics.hpp>

#include "PhysicalActor.hpp"
#include "Resources/SpriteResource.hpp"


class Obstacle : public PhysicalActor {

public:

    Obstacle();

    void update(const float& timeDelta) override;

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    void setSprite(SpriteResource* spriteResource);

private:

    // if the sprite has been set or not, make sure this is set before updating or drawing
    bool _spriteSet;

    // sprite to be used and its texture rectangles
    sf::Sprite _sprite;
    std::vector<sf::IntRect> _textureRects;

    // index of the frame of the sprite to draw; different frames refer to different texture rects
    // an obstacle only has one frame
    int _spriteCurrentFrame;


    const float _WIDTH_METERS; // ideal width in meters
    const float _WIDTH_PIXELS; // width in pixels after scaling
};

#endif // _OBSTACLE_HPP_

