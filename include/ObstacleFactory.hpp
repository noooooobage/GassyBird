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
    static std::shared_ptr<Obstacle> makeStreetlight(const float& heightMeters,
            const bool& faceLeft);

    /**
     * Creates and returns a ground obstacle whose width is the given eidth in meters.
     */
    static std::shared_ptr<Obstacle> makeGround(const float& widthMeters);

    /**
     * Creates and returns a poop obstacle.
     */
    static std::shared_ptr<Obstacle> makePoop();
};

#endif // _OBSTACLE_FACTORY_HPP_
