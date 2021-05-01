#ifndef _NPC_FACTORY_HPP_
#define _NPC_FACTORY_HPP_

#include <memory>

#include "NPC.hpp"

class NPCFactory {

public:

    NPCFactory() {}

    static std::shared_ptr<NPC> makeMale();

    static std::shared_ptr<NPC> makeFemale();
};

#endif // _NPC_FACTORY_HPP_