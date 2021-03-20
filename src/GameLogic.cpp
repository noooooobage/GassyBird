#include <cassert>
#include <vector>
#include <memory>
#include <iostream>

#include <box2d/box2d.h>

#include "GameLogic.hpp"

GameLogic::GameLogic() :
    _initialized(false),
    _GRAVITY(0.0f, -10.0f)
{}

GameLogic::~GameLogic() {

    // free memory
    _world.reset();

    // nullify pointers
    _playableBirdBody = nullptr;
}

void GameLogic::init() {

    _initialized = true;

    // create world from gravity
    _world = std::make_shared<b2World>(_GRAVITY);

    // create bird body and add to world
    _playableBirdBody = addToWorld(_playableBirdActor, b2Vec2(8.0f, 6.0f));
}

void GameLogic::update(const float& timeDelta) {

    assert(_initialized);

    // update all actors
    _playableBirdActor.update(timeDelta);

    // increment physics
    _world->Step(timeDelta, 8, 3);
}

b2Body* GameLogic::getBody(const PhysicalActor& actor) {

    // return nullptr if actor does not have a physical body
    void* actorAddress = (void*)&actor;
    if (_actorToBody.find(actorAddress) == _actorToBody.end())
        return nullptr;
    
    // return cprresponding body pointer
    return _actorToBody.at(actorAddress);
}

b2Body* GameLogic::addToWorld(const PhysicalActor& actor, const b2Vec2& position) {

    // make sure that the actor hasn't already been added
    assert(_actorToBody.find((void*)&actor) == _actorToBody.end());

    // get items from physical properties
    b2BodyDef bodyDef = actor.getBodyDef();
    std::vector<std::shared_ptr<b2Shape>> shapes = actor.getShapes();
    std::vector<b2FixtureDef> fixtureDefs = actor.getFixtureDefs();

    // make sure that the number of shapes and fixtures are equal
    assert(shapes.size() == fixtureDefs.size());

    // assign body position and create body
    bodyDef.position = position;
    b2Body* body = _world->CreateBody(&bodyDef);

    // assign shapes to fixure definitions and create fixtures
    for (int i = 0; i < fixtureDefs.size(); ++i) {
        fixtureDefs[i].shape = shapes[i].get();
        body->CreateFixture(&fixtureDefs[i]);
    }

    // add the actor and body to the body map
    _actorToBody[(void*)&actor] = body;

    return body;
}
