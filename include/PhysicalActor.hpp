#ifndef _PHYSICAL_ACTOR_HPP_
#define _PHYSICAL_ACTOR_HPP_

#include <vector>
#include <memory>
#include <string>

#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>

#include "Actor.hpp"

/**
 * A PhysicalActor is an actor that can exist in the game logic's physics engine. It provides
 * methods for setting and accessing structures that the logic uses to create physical bodies.
 */
class PhysicalActor : public Actor {

public:

    // Various types of physical actors
    enum class TYPE {
        PLAYABLE_BIRD,
        NPC,
        POOP,
        GROUND,
        GENERIC_OBSTACLE,
        PROJECTILE
    };

    /**
     * Constructor -- sets the type.
     */
    PhysicalActor(const PhysicalActor::TYPE& type) : _TYPE(type) {}

    /**
     * Destructor -- frees allocated memory.
     */
    virtual ~PhysicalActor() {
        for (std::shared_ptr<b2Shape>& shape : _shapes)
            shape.reset();
    }

    /**
     * Modify and access the body definition
     */
    void setBodyDef(const b2BodyDef& bodyDef) { _bodyDef = bodyDef; }
    const b2BodyDef& getBodyDef() const { return _bodyDef; }

    /**
     * Modify and access the shapes.
     */
    template <typename T> void 
    addShape(const T& shape) { _shapes.push_back(std::make_shared<T>(shape)); }
    const std::vector<std::shared_ptr<b2Shape>>& getShapes() const { return _shapes; }

    /**
     * Modify and acces the fixture definitions.
     */
    void addFixtureDef(const b2FixtureDef& fixtureDef) { _fixtureDefs.push_back(fixtureDef); }
    const std::vector<b2FixtureDef>& getFixtureDefs() const { return _fixtureDefs; }

    PhysicalActor::TYPE getType() const { return _TYPE; }

    /**
     * Returns the type represented as a string, used only for debug output.
     */
    std::string getTypeStr() const {
        switch (_TYPE) {
        case TYPE::GENERIC_OBSTACLE: return "GENERIC_OBSTACLE";
        case TYPE::GROUND:           return "GROUND";
        case TYPE::NPC:              return "NPC";
        case TYPE::PLAYABLE_BIRD:    return "PLAYABLE_BIRD";
        case TYPE::POOP:             return "POOP";
        default:                     return "undefined";
        }
    }

private:

    const PhysicalActor::TYPE _TYPE;

    b2BodyDef _bodyDef;

    // These are the shapes and fixtures that will be added to the body. The shapes and the fixtures
    // must match up, i.e. be the same length.    
    std::vector<std::shared_ptr<b2Shape>> _shapes;
    std::vector<b2FixtureDef> _fixtureDefs;

};

#endif // _PHYSICAL_ACTOR_HPP_
