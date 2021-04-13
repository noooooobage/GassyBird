#ifndef _NPC_FACTORY_HPP_
#define _NPC_FACTORY_HPP_

#include <memory>

#include "NPC.hpp"

class NPCFactory {

public:

    NPCFactory() {}

    /**
     * This method creates the default NPC with no real constraints or abilities.
     */
    static std::shared_ptr<NPC> makeDefault();

    //static std::shared_ptr<NPC> makeAttacker();
};

#endif // _NPC_FACTORY_HPP_