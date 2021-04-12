#ifndef _OBSTACLE_FACTORY_HPP_
#define _OBSTACLE_FACTORY_HPP_

#include "Obstacle.hpp"

class ObstacleFactory {

public:

    ObstacleFactory() {}

    /**
     * Creates and returns a streetlight obstacle whose height as close as possible to the given
     * height in meters. faceLeft denotes whether the streetlight is facing left (true) or rught
     * (false). Facing left means that the light is on the left of the pole.
     */
    static Obstacle makeStreetlight(const float& heightMeters, const bool& faceLeft);

    /**
     * Creates and returns a ground obstacle whose width is the given eidth in meters.
     */
    static Obstacle makeGround(const float& widthMeters);
};

#endif // _OBSTACLE_FACTORY_HPP_
