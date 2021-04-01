#include <SFML/Graphics.hpp>

#include "NPCFactory.hpp"
#include "NPC.hpp"
#include "Globals.hpp"
#include "Utils.hpp"
#include "Resources/SpriteResource.hpp"
#include "Resources/PolygonResource.hpp"

/*
This method creates the default NPC with no real constraints or abilities.
*/
NPC NPCFactory::makeDefault(){
    NPC _mob;
    _mob.init();
    return _mob;
}