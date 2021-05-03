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
     * These grounds are the ones that move along the screen.
     */
    static std::shared_ptr<Obstacle> makeGround(const float& widthMeters);

    /**
     * Returns an npcGround, which can cover a very large area. The distinction between normal
     * ground and npcGround exists so that the NPCs don't get caught on the places where the normal
     * grounds meet -- they instead just rest on top of the npcGround.
     */
    static std::shared_ptr<Obstacle> makeNPCGround(const float& widthMeters);

    /**
     * Creates and returns a poop obstacle, which has the given y-velocity.
     */
    static std::shared_ptr<Obstacle> makePoop(const float& yVelocity);

    static std::shared_ptr<Obstacle> makeTree(const float& heightMeters);

    static std::shared_ptr<Obstacle> makeCloud();

    static std::shared_ptr<Obstacle> makeDocks(const int& widthMeters, const int& heightMeters);

    static std::shared_ptr<Obstacle> makeUmbrella(const float angle);

    static std::shared_ptr<Obstacle> makeLifeguard(const bool& faceLeft);

    static std::shared_ptr<Obstacle> makePoopSplatter();

    static std::shared_ptr<Obstacle> makeRock();
    
    static std::shared_ptr<Obstacle> makeBeachBall(float tAngle);
};

#endif // _OBSTACLE_FACTORY_HPP_
