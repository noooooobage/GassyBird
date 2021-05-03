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

    _initialized(false),

    _EASY_THROW_CHANCE(0.35f),
    _HARD_THROW_CHANCE(0.65f),
    _EASY_THROW_DURATION(0.8f),
    _HARD_THROW_DURATION(0.5f)
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

        // if the NPC is idle, then choose to do something
        } else if (npc->isIdle()) {

            // choose whether to make the NPC walk or throw
            float throwChance = clamp(lerp(_EASY_THROW_CHANCE, _HARD_THROW_CHANCE,
                    _logic->getDifficulty()), _EASY_THROW_CHANCE, _HARD_THROW_CHANCE);
            bool shouldThrow = npc->isVisible && randomFloat(0.0f, 1.0f) <= throwChance;

            if (shouldThrow) {
                float throwDuration = clamp(lerp(_EASY_THROW_DURATION, _HARD_THROW_DURATION,
                        _logic->getDifficulty()), _HARD_THROW_DURATION, _EASY_THROW_DURATION);
                _logic->requestNPCAction(*npc, NPC::ACTION::START_THROW, 0.0f, throwDuration);

            } else {
                float walkDelay = randomFloat(0.15f, 1.0f);
                float walkDuration = 0.95f + randomFloat(-0.25f, 0.25f);
                npc->setFacingLeft(randomBool());
                _logic->requestNPCAction(*npc, NPC::ACTION::WALK, walkDelay, walkDuration);
            }
        }
    }
}
