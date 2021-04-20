#ifndef _OBSTACLE_FACTORY_HPP_
#define _OBSTACLE_FACTORY_HPP_

#include "Obstacle.hpp"

class ObstacleFactory {

public:

    ObstacleFactory() {}

    /**
     * Creates and returns a streetlight obstacle whose height as close as possible to (but not
     * above) to the given height in meters. faceLeft denotes whether the streetlight is facing left
     * (true) or right (false). Facing left means that the light is on the left of the pole.
     */
    static std::shared_ptr<Obstacle> makeStreetlight(const float& heightMeters,
            const bool& faceLeft);

    /**
     * Creates and returns a ground obstacle whose width is the given eidth in meters.
     */
    static std::shared_ptr<Obstacle> makeGround(const float& widthMeters);

    /**
     * Creates and returns a poop obstacle, which has the given y-velocity.
     */
    static std::shared_ptr<Obstacle> makePoop(const float& yVelocity);

    static std::shared_ptr<Obstacle> makeTree(const float& heightMeters,
            const bool& faceLeft);

    static std::shared_ptr<Obstacle> makeCloud();

    static std::shared_ptr<Obstacle> makeDocks(const float& widthMeters, const float& heightMeters);

    static std::shared_ptr<Obstacle> makeLifeguard();

    static std::shared_ptr<Obstacle> makePoopSplatter();
};

#endif // _OBSTACLE_FACTORY_HPP_
