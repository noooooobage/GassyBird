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
    void setSprite(const SpriteResource& spriteResource);

    /**
     * Methods which are called by the game logic to cause various behavior in the bird. These
     * methods only affect graphical properties of the bird; the logical changes (e.g. force applied
     * to the bird) happen in the logic.
     */
    void startFlying();
    void stopFlying();
    void startPooping();
    void stopPooping();

    bool isFlying() { return _isFlying; }
    bool isPooping() { return _isPooping; }

private:

    // if the sprite has been set or not, make sure this is set before updating or drawing
    bool _spriteSet;

    // sprite to be used and its texture rectangles
    sf::Sprite _sprite;
    std::vector<sf::IntRect> _textureRects;

    // stuff specifying animation
    const int _FLYING_CLOSED_START_FRAME;  // flying with mouth closed
    const int _FLYING_OPEN_START_FRAME;    // flying with mouth open
    const std::vector<int> _FLYING_FRAMES; // relative frame indices in the flying sequence
    const int _FALLING_FRAME;              // relative frame to display when bird is falling
    const float _FLYING_FRAME_DURATION;    // duration of one frame of flying
    int _currentFlyingFrame; // an index in the _FLYING_FRAMES vector
    float _frameTimer; // keeps track of when to advance the frame for flying

    // whether the bird is flying or not / pooping or not.
    bool _isFlying;
    bool _isPooping;

    const float _WIDTH_METERS; // ideal width in meters
    const float _WIDTH_PIXELS; // width in pixels after scaling
};

#endif // _PLAYABLE_BIRD_HPP_
