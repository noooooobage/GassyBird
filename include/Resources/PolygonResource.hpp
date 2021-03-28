#ifndef _POLYGON_RESOURCE_HPP_
#define _POLYGON_RESOURCE_HPP_

#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>

#include "Resource.hpp"

/**
 * Stores a box2d PolygonShape (must be convex).
 */
class PolygonResource : public Resource {

public:

    PolygonResource(const b2PolygonShape& polygon) : polygon(polygon) {}

    const ResourceType& getType() const override { return TYPE; }

    static const ResourceType TYPE;
    
    const b2PolygonShape polygon;
};

#endif // _POLYGON_RESOURCE_HPP_
