#ifndef _NPC_VIEW_HPP_
#define _NPC_VIEW_HPP_

#include <SFML/Graphics.hpp>

#include "GameLogic.hpp"

#include "EventListener.hpp"
#include "Event.hpp"


/**
 * Controls the NPCs. Will make them walk around and throw objects at the bird. The NPCs will throw
 * objects more frequently as the logic's difficulty increases.
 */
class NPCView {

public:

    NPCView();

    void init(GameLogic& logic);

    void update(const float& timeDelta);

private:

    bool _initialized;

    GameLogic* _logic;
};

#endif // _NPC_VIEW_HPP_
