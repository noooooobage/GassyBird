#include <SFML/Graphics.hpp>

#include "NPCFactory.hpp"
#include "NPC.hpp"
#include "Globals.hpp"
#include "Utils.hpp"
#include "Resources/SpriteResource.hpp"
#include "Resources/PolygonResource.hpp"

NPC NPCFactory::makeDefault(){
    NPC _mob;
    _mob.init();
    return _mob;
}