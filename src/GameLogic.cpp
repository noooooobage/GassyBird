#include <cassert>
#include <vector>
#include <memory>
#include <iostream>
#include <math.h>

#include <box2d/box2d.h>

#include "GameLogic.hpp"
#include "DebugDrawer.hpp"
#include "Obstacle.hpp"
#include "ObstacleFactory.hpp"
#include "Globals.hpp"

GameLogic::GameLogic() :

    _initialized(false),

    _GRAVITY(0.0f, -25.0f),
    _INITIAL_WORLD_SCROLL_SPEED(7.0f),
    _world_scroll_speed(_INITIAL_WORLD_SCROLL_SPEED),

    _BIRD_POOP_DURATION(1.0f),
    _BIRD_MAX_POOPS(2),
    _POOP_DOWNWARD_VELOCITY(3.0f),

    _NUM_GROUNDS(4),
    _GROUND_WIDTH_METERS(400 * METERS_PER_PIXEL),
    _GROUND_OFFSET_METERS(0.5f)
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

    // initialize playable bird and add it to the physics world
    _playableBirdActor.init();
    _playableBirdBody = addToWorld(_playableBirdActor, b2Vec2(8.0f, 6.0f), false);

    // create the ground objects
    for (int i = 0; i < _NUM_GROUNDS; ++i) {
        _grounds.push_back(ObstacleFactory::makeGround(_GROUND_WIDTH_METERS));
        addToWorld(*_grounds.back(), b2Vec2(_GROUND_WIDTH_METERS + i * _GROUND_WIDTH_METERS,
                _GROUND_OFFSET_METERS));
    }

    // add two streetlights of different heights
    // TODO: remove these later, this is only temporary
    _obstacles.push_back(ObstacleFactory::makeStreetlight(4.0f, true));
    addToWorld(*_obstacles.back(), b2Vec2(13.0f, _GROUND_OFFSET_METERS));
    _obstacles.push_back(ObstacleFactory::makeStreetlight(8.0f, false));
    addToWorld(*_obstacles.back(), b2Vec2(21.0f, _GROUND_OFFSET_METERS));

    // set state to demo
    toDemo();
}

void GameLogic::update(const float& timeDelta) {

    assert(_initialized);

    float t = timeDelta * 1.0f;

    updateGround();
    updatePlayableBird(t);
    
    // increment physics
    _world->Step(t, 8, 3);
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
    _numPoopsLeft = _BIRD_MAX_POOPS;
    _playerScore = 0;
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

b2Body* GameLogic::getBody(const PhysicalActor& actor) const {

    assert(_initialized);

    // return nullptr if actor does not have a physical body
    PhysicalActor* actorAddress = (PhysicalActor*)&actor;
    if (_physicalActors.find(actorAddress) == _physicalActors.end())
        return nullptr;
    
    // return cprresponding body pointer
    return _physicalActors.at(actorAddress);
}

const std::unordered_map<PhysicalActor*, b2Body*> GameLogic::getVisibleActors() const {

    assert(_initialized);

    return _physicalActors;
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
    
    // only poop if all of the following:
    //     - the state is PLAYING
    //     - the bird has at least one poop left
    //     - the bird is not currently pooping
    if (_state == PLAYING && _numPoopsLeft > 0 && !_playableBirdActor.isPooping()) {

        // set variables to reflect poop start
        _playableBirdActor.startPooping();
        --_numPoopsLeft;
        _timeSinceLastPoop = 0.0f;

        // Make a poop obstacle and give it a vertical velocity which will make it shoot downward
        // from the bird.
        _obstacles.push_back(ObstacleFactory::makePoop(_playableBirdBody->GetLinearVelocity().y -
                _POOP_DOWNWARD_VELOCITY));
        addToWorld(*_obstacles.back(), _playableBirdBody->GetPosition() - b2Vec2(0.5f, 0.5f),
                false);
    }
}

b2Body* GameLogic::addToWorld(const PhysicalActor& actor, const b2Vec2& position,
        bool inheritWorldScroll) {

    assert(_initialized);

    // make sure that the actor hasn't already been added
    PhysicalActor* actorAddress = (PhysicalActor*)&actor;
    assert(_physicalActors.find(actorAddress) == _physicalActors.end());

    // get items from physical properties
    b2BodyDef bodyDef = actor.getBodyDef();
    std::vector<std::shared_ptr<b2Shape>> shapes = actor.getShapes();
    std::vector<b2FixtureDef> fixtureDefs = actor.getFixtureDefs();

    // make sure that the number of shapes and fixtures are equal
    assert(shapes.size() == fixtureDefs.size());

    // assign body position and velocity
    bodyDef.position = position;
    if (inheritWorldScroll)
        bodyDef.linearVelocity += b2Vec2(-_world_scroll_speed, 0.0f);

    // create the body
    b2Body* body = _world->CreateBody(&bodyDef);
    
    // assign shapes to fixure definitions and create fixtures
    for (int i = 0; i < fixtureDefs.size(); ++i) {
        fixtureDefs[i].shape = shapes[i].get();
        body->CreateFixture(&fixtureDefs[i]);
    }

    // add the actor and body to the body map
    _physicalActors[actorAddress] = body;
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
    
    // Set the bird's rotation based on its velocity, dampen the rotation slightly so it's not so
    // severe.
    float angle = atan2f(_playableBirdBody->GetLinearVelocity().y, _world_scroll_speed * 2.0f);
    _playableBirdBody->SetTransform(_playableBirdBody->GetPosition(), angle);

    // call bird's own update() method
    _playableBirdActor.update(timeDelta);
}

void GameLogic::updateGround() {

    assert(_initialized);

    while (true) {

        // get the leftmost ground (the one at the front of the list is always the leftmost)
        std::shared_ptr<Obstacle> leftGround = _grounds.front();
        b2Body* leftGroundBody = getBody(*leftGround);

        // sanity check -- make sure the body was able to be found
        assert(leftGroundBody);

        // if this ground obstacle is a little to the left of the screen, then need to:
        //   1. move this ground to the right of the rightmost ground
        //   2. make this ground the rightmost ground in the list
        if (leftGroundBody->GetPosition().x <= 0.1f) {

            b2Body* rightGroundBody = getBody(*_grounds.back());
            assert(rightGroundBody);

            // step 1
            const b2Vec2& rightGroundPosition = rightGroundBody->GetPosition();
            leftGroundBody->SetTransform(
                b2Vec2(rightGroundPosition.x + _GROUND_WIDTH_METERS, rightGroundPosition.y),
                rightGroundBody->GetAngle()
            );

            // step 2
            _grounds.pop_front();
            _grounds.push_back(leftGround);

        // if this ground isn't to the left of the screen, then stop
        } else {
            break;
        }
    }
}

void GameLogic::increaseScrollSpeed(const float& amount) {

    assert(_initialized);

    // iterate through all bodies and change their velocities
    for (auto& pair : _physicalActors) {

        b2Body* body = pair.second;

        // skip if it's the bird
        if (body == _playableBirdBody)
            continue;

        body->SetLinearVelocity(body->GetLinearVelocity() + b2Vec2(-amount, 0.0f));
    }

    // update the _world_scroll_speed variable
    _world_scroll_speed += amount;
}
