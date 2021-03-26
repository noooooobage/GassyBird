#include <cassert>
#include <vector>
#include <memory>
#include <iostream>
#include <math.h>

#include <box2d/box2d.h>

#include "GameLogic.hpp"

GameLogic::GameLogic() :

    _initialized(false),

    _GRAVITY(0.0f, -25.0f),
    _world_scroll_speed(15.0f),

    _BIRD_POOP_DURATION(1.0f)
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

    // reset bird variables, and create bird body and add to world
    _timeSinceLastPoop = 0.0f;
    _playableBirdActor.stopFlying();
    _playableBirdActor.stopPooping();
    _playableBirdBody = addToWorld(_playableBirdActor, b2Vec2(8.0f, 6.0f));
}

void GameLogic::update(const float& timeDelta) {

    assert(_initialized);

    // update bird
    updatePlayableBird(timeDelta);

    // increment physics
    _world->Step(timeDelta, 8, 3);
}

const b2Body* GameLogic::getBody(const PhysicalActor& actor) {

    // return nullptr if actor does not have a physical body
    void* actorAddress = (void*)&actor;
    if (_actorToBody.find(actorAddress) == _actorToBody.end())
        return nullptr;
    
    // return cprresponding body pointer
    return _actorToBody.at(actorAddress);
}

void GameLogic::requestBirdStartFly() {
    _playableBirdActor.startFlying();
}

void GameLogic::requestBirdStopFly() {
    _playableBirdActor.stopFlying();
}

void GameLogic::requestBirdPoop() {
    // only poop if the bird is not currently pooping
    if (!_playableBirdActor.isPooping()) {
        _playableBirdActor.startPooping();
        _timeSinceLastPoop = 0.0f;
    }
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

void GameLogic::updatePlayableBird(const float& timeDelta) {

    // update bird pooping status
    _timeSinceLastPoop += timeDelta;
    if (_timeSinceLastPoop >= _BIRD_POOP_DURATION)
        _playableBirdActor.stopPooping();

    // if the bird is flying, then apply an upward force opposite to gravity
    if (_playableBirdActor.isFlying()) {
        float targetAccel = -2.0f * _GRAVITY.y;
        float force = _playableBirdBody->GetMass() * targetAccel;
        _playableBirdBody->ApplyForceToCenter(b2Vec2(0.0f, force), true);
    }
    
    // set the bird's rotation based on its velocity
    float angle = atan2f(_playableBirdBody->GetLinearVelocity().y, _world_scroll_speed);
    _playableBirdBody->SetTransform(_playableBirdBody->GetPosition(), angle);

    // call bird's own update() method
    _playableBirdActor.update(timeDelta);
}
