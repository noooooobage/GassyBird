#ifndef _NPC_HPP_
#define _NPC_HPP_

#include <vector>

#include <SFML/Graphics.hpp>

#include "PhysicalActor.hpp"

/**
 * Parent Class of The NPC people that serve as targets and enemies to the bird.
 */
class NPC : public PhysicalActor {

    //Public
    public:
        //Constructor
        NPC();

        //Override update method
        void update(const float& timeDelta) override;

        //Override draw method
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;


        /**
        * Sets the sprite of the bird from a SpriteResource.
        */
        void setSprite(const SpriteResource& spriteResource);

        //The is hit boolean follows
        bool isHit;

        void triggerAction();

    private:

        //Variables 

        bool _spriteSet;                    //Track if the sprite has been set before drawing

        sf::Sprite _NPCsprite;
        std::vector<sf::IntRect> _textureRect;


        // This tells the game what sprite on the sheet to draw
        int _spriteCurrentFrame;

        // the sprite's frame will change after this amount of time in seconds passes
        const float _FRAME_CHANGE_TIME_DELTA;
        float _frameChangeTimer;

        const float _WIDTH_METERS; // Unit width in meters
        const float _HEIGHT_METERS; //Unit height in meters
        const float _WIDTH_PIXELS; // store the width in pixels after scaling
        const float _HEIGHT_PIXELS; //store the height in pixels after scaling
};

#endif