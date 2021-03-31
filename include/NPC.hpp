#ifndef _NPC_HPP_
#define _NPC_HPP_

#include <vector>

#include <SFML/Graphics.hpp>

#include "PhysicalActor.hpp"
#include "Resources/SpriteResource.hpp"

/**
 * Parent Class of The NPC people that serve as targets and enemies to the bird.
 */
class NPC : public PhysicalActor {

    //Public
    public:
        //Constructor
        NPC();

        void init();
        //Override update method
        void update(const float& timeDelta) override;

        //Override draw method
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;


        //The is hit boolean follows
        bool isHit;

        void triggerAction();

        void changeVelocity();

    private:

        //Variables 
        bool _initialized;
        bool _spriteSet;                    //Track if the sprite has been set before drawing
        bool _inScope;

        sf::Sprite _NPCsprite;
        std::vector<sf::IntRect> _textureRects;


        // This tells the game what sprite on the sheet to draw
        int _spriteCurrentFrame;

        // the sprite's frame will change after this amount of time in seconds passes
        const float _FRAME_CHANGE_TIME_DELTA;
        float _frameChangeTimer;

        const float _WIDTH_METERS; // Unit width in meters
        const float _HEIGHT_METERS; //Unit height in meters
        const float _WIDTH_PIXELS; // store the width in pixels after scaling
        const float _HEIGHT_PIXELS; //store the height in pixels after scaling

        b2FixtureDef _top; //Fixture for the head of the body
        b2FixtureDef _bottom;  //Fixture for the torso of the body

        
};

#endif