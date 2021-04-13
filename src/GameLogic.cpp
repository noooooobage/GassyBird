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
#include "Utils.hpp"
#include "NPC.hpp"
#include "NPCFactory.hpp"

GameLogic::GameLogic() :

    _initialized(false),

    _GRAVITY(0.0f, -25.0f),
    _INITIAL_WORLD_SCROLL_SPEED(7.0f),
    _worldScrollSpeed(_INITIAL_WORLD_SCROLL_SPEED),

    _BIRD_POOP_DURATION(1.0f),
    _BIRD_MAX_POOPS(2),
    _POOP_DOWNWARD_VELOCITY(3.0f),

    _NUM_GROUNDS(4),
    _GROUND_WIDTH_METERS(400 * METERS_PER_PIXEL),
    _GROUND_OFFSET_METERS(0.5f)
{}

GameLogic::~GameLogic() {

    // remove every actor
    while (true) {
        auto i = _physicalActors.begin();
        if (i == _physicalActors.end())
            break;
        removeFromWorld(*i->first);
    }

    // free world memory
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
    _playableBirdBody = addToWorld(_playableBirdActor, b2Vec2(8.0f, 10.0f), false);

    // set state to demo
    toDemo();

    // create the first actors
    createMap();
}

void GameLogic::update(const float& timeDelta) {

    assert(_initialized);

    // perform removal and procedural generation
    removeOutOfBoundsActors();
    generateNewActors();

    // update actors
    updateGround();
    updatePlayableBird(timeDelta);

    // Ensure that GROUND and GENERIC_OBSTACLE actors are moving at the world scroll speed. This is
    // necessary to do every frame because sometimes varying timeDeltas affects the velocity
    setWorldScrollSpeed(_worldScrollSpeed);
    
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

void GameLogic::createMap() {

    // create the ground objects
    for (int i = 0; i < _NUM_GROUNDS; ++i) {
        _grounds.push_back(ObstacleFactory::makeGround(_GROUND_WIDTH_METERS));
        addToWorld(*_grounds.back(), b2Vec2(_GROUND_WIDTH_METERS + i * _GROUND_WIDTH_METERS,
                _GROUND_OFFSET_METERS));
    }

    // spawn 3 random NPEs
    for (int i = 0; i < 3; ++i) {
        float xPosition = i * (NATIVE_RESOLUTION.x * METERS_PER_PIXEL) / 2.0f + 4.0f;
        spawnNPE(b2Vec2(xPosition, _GROUND_OFFSET_METERS));
    }
}

void GameLogic::removeOutOfBoundsActors() {

    // First, collect a list of out of bounds actors, so that we're not iterating through the
    // _physicalActors map while removing any of them.
    std::list<const PhysicalActor*> OOBActors;
    for (auto& pair : _physicalActors) {

        const PhysicalActor* actor = pair.first;
        const b2Body* body = pair.second;

        // make sure actor and body aren't nullptr
        assert(actor);
        assert(body);

        // The actor is out of bounds if its body is 5 meters to the left of the screen. Doesn't
        // care about ground obstacles or the bird itself.
        bool isOutOfBounds = body->GetPosition().x < -5.0f &&
                actor->getType() != PhysicalActor::TYPE::PLAYABLE_BIRD &&
                actor->getType() != PhysicalActor::TYPE::GROUND;
        if (isOutOfBounds)
            OOBActors.push_back(actor);
    }

    // next, iterate through the list of out of bounds actors and remove each one from the world
    for (auto actor : OOBActors)
        removeFromWorld(*actor);
}

void GameLogic::generateNewActors() {
    
    // The current metric for determining when to spawn actors is maintaining a minimum number of
    // entities on screen. If this number falls below some threshold (right now is 3), then we know
    // to spawn another one.
    // TODO: This is an okay placeholder, but we might want to think about using a more
    // sophisticated method of determining when to spawn things.
    int numEntities = _obstacles.size() + _NPCs.size();
    bool needToSpawn = numEntities < 3;

    // If we do need to spawn something, then determine a random position past the right of the
    // screen and spawn an NPE there.
    if (needToSpawn) {
        float xPosition = NATIVE_RESOLUTION.x * METERS_PER_PIXEL + randomFloat(2.0f, 5.0f);
        spawnNPE(b2Vec2(xPosition, _GROUND_OFFSET_METERS));
    }
}

void GameLogic::spawnNPE(const b2Vec2& position) {

    // Only two options right now: streetlight or default NPC, so just decide between the two with a
    // random bool.
    bool spawnStreetlight = randomBool();

    if (spawnStreetlight) {
        // determine a random height and face direction
        float heightMeters = randomFloat(4.0f, 9.0f);
        bool faceLeft = randomBool();
        _obstacles.push_back(ObstacleFactory::makeStreetlight(heightMeters, faceLeft));
        addToWorld(*_obstacles.back(), position);

    } else {
        // not spawning a streetlight, so spawn an NPC instead
        _NPCs.push_back(NPCFactory::makeDefault());
        addToWorld(*_NPCs.back(), position);
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

    // set the position and velocity of the body
    bodyDef.position = position;
    if (inheritWorldScroll)
        bodyDef.linearVelocity += b2Vec2(-_worldScrollSpeed, 0.0f);

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

void GameLogic::removeFromWorld(const PhysicalActor& actor) {

    assert(_initialized);

    // get the address and find the associated body
    PhysicalActor* actorAddress = (PhysicalActor*)&actor;
    b2Body* body = getBody(actor);

    // if there is an associated body, then destroy it and nullify it
    if (body) {
        _world->DestroyBody(body);
        body = nullptr;
    }

    // remove entry from the _physicalActors map
    _physicalActors.erase(actorAddress);
    
    // Actors in lists are dynamically allocated, so need to remove them from the list and free the
    // associated memory. Just attempt to remove the actor from all the actor lists.
    removeFromList(actor, _grounds);
    removeFromList(actor, _obstacles);
    removeFromList(actor, _NPCs);
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
    float angle = atan2f(_playableBirdBody->GetLinearVelocity().y, _worldScrollSpeed * 2.0f);
    _playableBirdBody->SetTransform(_playableBirdBody->GetPosition(), angle);

    // call bird's own update() method
    _playableBirdActor.update(timeDelta);
}

void GameLogic::updateGround() {

    assert(_initialized);

    // Ensure that the ground's velocity is consistent with the world scroll speed -- this can
    // actually get out of sync in the beginning due to wildly varying frameDelta times.
    for (auto ground : _grounds) {

        b2Body* groundBody = getBody(*ground);
        assert(groundBody);

        b2Vec2 currentVelocity = groundBody->GetLinearVelocity();

        if (fabs(currentVelocity.x + _worldScrollSpeed) > SMALL)
            groundBody->SetLinearVelocity(b2Vec2(-_worldScrollSpeed, currentVelocity.y));
    }

    // ensure that the ground covers the screen as best as possible
    while (true) {

        // get the leftmost ground (the one at the front of the list is always the leftmost)
        std::shared_ptr<Obstacle> leftGround = _grounds.front();
        b2Body* leftGroundBody = getBody(*leftGround);

        // sanity check -- make sure the body was able to be found
        assert(leftGroundBody);

        // if this ground obstacle is a little to the left of the screen, then need to:
        //   1. move this ground to the right of the rightmost ground
        //   2. make this ground the rightmost ground in the list
        if (leftGroundBody->GetPosition().x <= -1.0f) {

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

void GameLogic::setWorldScrollSpeed(const float& amount) {

    assert(_initialized);

    // iterate through all bodies and change their velocities
    for (auto& pair : _physicalActors) {

        // get the actor and skip if it's not of type GROUND or GENERIC_OBSTACLE
        PhysicalActor* actor = pair.first;
        assert(actor);
        if (!(
            actor->getType() == PhysicalActor::TYPE::GROUND ||
            actor->getType() == PhysicalActor::TYPE::GENERIC_OBSTACLE
        ))
            continue;
        
        // get the associated body and set its linear velocity accordingly
        b2Body* body = pair.second;
        assert(body);
        body->SetLinearVelocity(b2Vec2(-_worldScrollSpeed, body->GetLinearVelocity().y));
    }

    // update the _worldScrollSpeed variable
    _worldScrollSpeed = amount;
}
