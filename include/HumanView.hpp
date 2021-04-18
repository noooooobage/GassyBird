#ifndef _HUMAN_VIEW_HPP_
#define _HUMAN_VIEW_HPP_

#include <SFML/Graphics.hpp>

#include "GameLogic.hpp"

#include "EventListener.hpp"
#include "Event.hpp"


/**
 * Receives user input to control playable actors via the game logic. Also is resposible for
 * displaying visuals and sound to the user.
 */
class HumanView : public sf::Drawable {

public:

    HumanView();

    ~HumanView();

    void init(GameLogic& logic);

    /**
     * 
     */
    void update(const float& timeDelta);

    /**
     * Draws all actors.
     */
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:

    /**
     * Called when a KeyPressEvent is triggered. Asks the logic to start or stop bird flight or make
     * the bird poop depending on what key what pressed.
     */
    void keyPressHandler(const Event& event);
    
    /**
     * Called when a KeyReleaseEvent is triggered.
     */
    void keyReleaseHandler(const Event& event);

    bool _initialized;

    // pointer to game logic; make sure it isn't nullptr before doing stuff with it
    GameLogic* _logic;

    EventListener _keyPressListener;
    EventListener _keyReleaseListener;

    sf::Keyboard::Key _keyToFly; // key which causes the bird to fly
    sf::Keyboard::Key _keyToPoop; // key which causes the bird to poop
    sf::Keyboard::Key _keyToPause; // key which pauses/unpauses the game

    sf::Sprite _beachBackground; // beach background sprite
};

#endif // _HUMAN_VIEW_HPP_
