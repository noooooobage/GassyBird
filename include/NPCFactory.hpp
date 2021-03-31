#ifndef _NPC_FACTORY_HPP_
#define _NPC_FACTORY_HPP_

#include "NPC.hpp"

class NPCFactory {

public:

    NPCFactory() {}

    /**
     * Creates and returns a streetlight obstacle which will make its height as close as possible to
     * the given height in meters. faceLeft denotes whether the streetlight is facing left (true) or
     * rught (false). Facing left means that the light is on the left of the pole.
     */
    static NPC makeDefault();

    //static NPC makeAttacker();
};

#endif // _NPC_FACTORY_HPP_