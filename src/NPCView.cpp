#include <cassert>
#include <iostream>

#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>

#include "NPCView.hpp"
#include "GameLogic.hpp"
#include "Globals.hpp"
#include "Utils.hpp"
#include "Resources/SpriteResource.hpp"
#include "Events/KeyPressEvent.hpp"
#include "Events/KeyReleaseEvent.hpp"
#include "ObstacleFactory.hpp"
#include "PhysicalActor.hpp"
#include "NPC.hpp"

NPCView::NPCView() :
    _initialized(false)
{}

void NPCView::init(GameLogic& logic) {

    _initialized = true;

    _logic = &logic;
}

void NPCView::update(const float& timeDelta) {

    assert(_initialized);

    for (auto npc : _logic->getNPCs()) {

        // finish throwing if the npc is ready to throw
        if (npc->isThrowing()) {
            if (npc->isReadyToFinishThrowing())
                _logic->requestNPCAction(*npc, NPC::ACTION::FINISH_THROW, 0.0f, 0.0f);

        } else if (npc->isIdle()) {

            // choose whether to make the NPC walk or throw
            // TODO: base this on the game's difficulty
            bool shouldWalk = randomFloat(0.0f, 1.0f) >= 0.4f;

            if (shouldWalk) {
                npc->setFacingLeft(randomBool());
                _logic->requestNPCAction(*npc, NPC::ACTION::WALK, 0.5f, 1.2f);

            } else {
                _logic->requestNPCAction(*npc, NPC::ACTION::START_THROW, 0.0f, 0.5f);
            }
        }
    }
}
