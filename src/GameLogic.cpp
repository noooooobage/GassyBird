#include <cassert>
#include <vector>
#include <memory>
#include <iostream>
#include <math.h>

#include <box2d/box2d.h>

#include "GameLogic.hpp"
#include "DebugDrawer.hpp"

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

    // create bird body and add to world
    _timeSinceLastPoop = 0.0f;
    _playableBirdActor.stopFlying();
    _playableBirdActor.stopPooping();
    _playableBirdBody = addToWorld(_playableBirdActor, b2Vec2(8.0f, 6.0f));

    // set state to demo
    toDemo();
}

void GameLogic::update(const float& timeDelta) {

    assert(_initialized);

    // update bird
    updatePlayableBird(timeDelta);

    // increment physics
    _world->Step(timeDelta, 8, 3);
}

void GameLogic::toDemo() {

    assert(_initialized);

    // set bird to demo state
    _playableBirdActor.stopPooping();
    _playableBirdActor.stopFlying();

    // turn off gravity for the bird
    _playableBirdBody->SetGravityScale(0.0f);

    // set state
    _state = DEMO;
}

void GameLogic::toPlaying() {

    assert(_initialized);

    // set bird to initial playing state
    _timeSinceLastPoop = 0.0f;
    _playableBirdActor.stopPooping();
    _playableBirdActor.stopFlying();

    // turn on gravity and wake the bird
    _playableBirdBody->SetGravityScale(1.0f);
    _playableBirdBody->SetAwake(true);

    // set state
    _state = PLAYING;
}

void GameLogic::setDebugDrawer(DebugDrawer& debugDrawer) {
    
    assert(_initialized);

    _world->SetDebugDraw(&debugDrawer);
}

void GameLogic::debugDraw() {

    assert(_initialized);

    _world->DebugDraw();
}

const b2Body* GameLogic::getBody(const PhysicalActor& actor) {

    assert(_initialized);

    // return nullptr if actor does not have a physical body
    void* actorAddress = (void*)&actor;
    if (_actorToBody.find(actorAddress) == _actorToBody.end())
        return nullptr;
    
    // return cprresponding body pointer
    return _actorToBody.at(actorAddress);
}

void GameLogic::requestBirdStartFly() {

    assert(_initialized);

    // only allow if state is PLAYING
    if (_state == PLAYING)
        _playableBirdActor.startFlying();
}

void GameLogic::requestBirdStopFly() {

    assert(_initialized);
    
    // only allow if state is PLAYINS
    if (_state == PLAYING)
        _playableBirdActor.stopFlying();
}

void GameLogic::requestBirdPoop() {

    assert(_initialized);
    
    // only poop if the state is PLAYING and the bird is not currently pooping
    if (_state == PLAYING && !_playableBirdActor.isPooping()) {
        _playableBirdActor.startPooping();
        _timeSinceLastPoop = 0.0f;
    }
}

b2Body* GameLogic::addToWorld(const PhysicalActor& actor, const b2Vec2& position) {

    assert(_initialized);

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

//TODO: Discuss and implement the routine and procedures for spawning the NPC
//b2Body* GameLogic::spawnNPC(){
    //Create a new NPC
    
    //Get the physical properties
    //Assert that shapes and fixtures are equal in size
    //assign shapes to fixture
    //add the actor and body to the map

    //Add it to a list of NPCs being stored
//}

void GameLogic::updatePlayableBird(const float& timeDelta) {

    assert(_initialized);

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
