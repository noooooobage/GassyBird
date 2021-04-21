#ifndef _NPC_VIEW_HPP_
#define _NPC_VIEW_HPP_

#include <SFML/Graphics.hpp>

#include "GameLogic.hpp"

#include "EventListener.hpp"
#include "Event.hpp"


/**
 * Receives user input to control playable actors via the game logic. Also is resposible for
 * displaying visuals and sound to the user.
 */
class NPCView : public sf::Drawable {

public:

    NPCView();

    ~NPCView();

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
     * Called when a KeyPressEvent is triggered.
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

    // NPC list
    // std::list<std:shared_ptr<NPC>> _NPCs;

    sf::Sprite _NPCSprite;
};

#endif // _NPC_VIEW_HPP_
