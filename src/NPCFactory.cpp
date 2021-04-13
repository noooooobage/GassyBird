#include <memory>

#include <SFML/Graphics.hpp>

#include "NPCFactory.hpp"
#include "NPC.hpp"
#include "Globals.hpp"
#include "Utils.hpp"
#include "Resources/SpriteResource.hpp"
#include "Resources/PolygonResource.hpp"

std::shared_ptr<NPC> NPCFactory::makeDefault(){
    std::shared_ptr<NPC> _mob(new NPC());
    _mob->init();
    return _mob;
}