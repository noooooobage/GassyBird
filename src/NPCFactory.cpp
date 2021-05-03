#include <memory>
#include <iostream>

#include <SFML/Graphics.hpp>

#include "NPCFactory.hpp"
#include "NPC.hpp"
#include "Globals.hpp"
#include "Utils.hpp"
#include "Resources/SpriteResource.hpp"
#include "Resources/PolygonResource.hpp"

std::shared_ptr<NPC> NPCFactory::makeMale() {
    std::shared_ptr<NPC> _mob(new NPC(NPC::TYPE::MALE));
    _mob->init();
    return _mob;
}

std::shared_ptr<NPC> NPCFactory::makeFemale() {
    std::shared_ptr<NPC> _mob(new NPC(NPC::TYPE::FEMALE));
    _mob->init();
    return _mob;
}
