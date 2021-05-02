#include <cassert>
#include <vector>
#include <memory>
#include <iostream>
#include <math.h>
#include <algorithm>

#include <box2d/box2d.h>
#include <box2d/b2_math.h>

#include "GameLogic.hpp"
#include "DebugDrawer.hpp"
#include "Obstacle.hpp"
#include "ObstacleFactory.hpp"
#include "Globals.hpp"
#include "Utils.hpp"
#include "NPC.hpp"
#include "NPCFactory.hpp"
#include "Event.hpp"
#include "Events/GamePauseEvent.hpp"
#include "Events/CollisionEvent.hpp"
#include "Events/GameOverEvent.hpp"

GameLogic::GameLogic() :

    _initialized(false),

    _GRAVITY(0.0f, -25.0f),
    _INITIAL_WORLD_SCROLL_SPEED(7.0f),
    _worldScrollSpeed(_INITIAL_WORLD_SCROLL_SPEED),
    _STEP_TIME(2.f),
    _ACTION_TIME(2.f),

    _NUM_GROUNDS(5),
    _GROUND_WIDTH_METERS(400 * METERS_PER_PIXEL),
    _GROUND_OFFSET_METERS(0.5f),

    _playableBirdBody(nullptr),
    _BIRD_DEMO_POSITION(b2Vec2(8.0f, 10.5f)),
    _BIRD_MAX_HEIGHT(NATIVE_RESOLUTION.y * METERS_PER_PIXEL - 0.5f),
    _BIRD_SLOW_HEIGHT(_BIRD_MAX_HEIGHT - 1.0f),
    _BIRD_MAX_VELOCITY(sqrtf((NATIVE_RESOLUTION.y * METERS_PER_PIXEL -_GROUND_OFFSET_METERS) *
            -_GRAVITY.y * 2.0f)),
    _BIRD_POOP_DURATION(0.5f),
    _BIRD_MAX_POOPS(2),
    _POOP_DOWNWARD_VELOCITY(3.0f),
    _lastPoop(nullptr),

    _NPC_WALK_SPEED(2.0f),
    _DEFAULT_NPC_THROW_SPEED(25.0f),
    _BIRD_DEATH_TIME(15.0f),
    
    _difficulty(3)
{}

GameLogic::~GameLogic() {

    // remove event listeners
    eventMessenger.removeListener(GamePauseEvent::TYPE, _gamePauseListener);
    eventMessenger.removeListener(CollisionEvent::TYPE, _collisionListener);

    // remove every actor
    removeAllFromWorld();

    // free world memory
    _world.reset();

    // nullify pointers
    _playableBirdBody = nullptr;
}

void GameLogic::init() {

    _initialized = true;

    // initialize and add event listeners
    _gamePauseListener.init(&GameLogic::gamePauseHandler, this);
    _collisionListener.init(&GameLogic::collisionHandler, this);
    eventMessenger.addListener(GamePauseEvent::TYPE, _gamePauseListener);
    eventMessenger.addListener(CollisionEvent::TYPE, _collisionListener);

    // create world from gravity
    _world = std::make_shared<b2World>(_GRAVITY);

    // assign the contact listener to the world
    _world->SetContactListener(&_contactListener);

    // initialize playable bird
    _playableBirdActor.init();

    // set state to demo
    toDemo();
}

void GameLogic::update(const float& timeDelta) {

    assert(_initialized);

    // if the game is paused, do nothing
    if (_isPaused)
        return;

    // Ensure that scrolling actors are moving at the world scroll speed. This is necessary to do
    // every frame because sometimes varying timeDeltas affects the velocity
    setWorldScrollSpeed(_worldScrollSpeed);

    // perform removal and procedural generation
    removeOutOfBoundsActors();
    generateNewActors();

    // update actors
    updateGround();
    updatePlayableBird(timeDelta);
    updateNPCs(timeDelta);
    
    updateDifficulty();
    _timeSinceLastNPC += timeDelta;
    
    // increment physics
    _world->Step(timeDelta, 8, 4);

    //this is used as an approximation
    _totalTimePassed += timeDelta;
}

void GameLogic::toDemo() {

    assert(_initialized);

    // set state, also the game should not be paused
    _state = DEMO;
    _isPaused = false;

    // remove all actors and create initial map
    removeAllFromWorld();
    createMap();

    // add the playable bird to the physical world and reset _lastPoop
    _playableBirdBody = addToWorld(_playableBirdActor, _BIRD_DEMO_POSITION, false);
    _lastPoop = nullptr;

    // set bird to demo state
    _playableBirdActor.stopPooping();
    _playableBirdActor.startFlying();
    _totalTimePassed = 0.0;
    _difficulty = 3;

    // turn off gravity for the bird
    _playableBirdBody->SetGravityScale(0.0f);
}

void GameLogic::toPlaying() {

    assert(_initialized);

    // set state
    _state = PLAYING;

    // set bird to initial playing state
    _timeSinceLastPoop = 0.0f;
    _numPoopsLeft = _BIRD_MAX_POOPS;
    _playerScore = 0;
    _playableBirdActor.stopPooping();
    _playableBirdActor.stopFlying();
    _totalTimePassed = 0.0;
    _difficulty = 3;
    _spawnPositionLastObstacle = 0.0f;

    // set initial values for bird's physical body
    _playableBirdBody->SetGravityScale(1.0f);
    _playableBirdBody->SetLinearVelocity(b2Vec2(0.0f, 0.0f));
    _playableBirdBody->SetAwake(true);
}

void GameLogic::toGameOver() {

    assert(_initialized);

    // set the state to GAME_OVER
    _state = GAME_OVER;

    // set bird properties
    _playableBirdActor.stopPooping();
    _playableBirdActor.stopFlying();
    _playableBirdActor.die();
}

bool GameLogic::isPaused() const {

    assert(_initialized);

    return _isPaused;
}

void GameLogic::setDebugDrawer(DebugDrawer& debugDrawer) {
    
    assert(_initialized);

    _world->SetDebugDraw(&debugDrawer);
}

void GameLogic::debugDraw() {

    assert(_initialized);

    _world->DebugDraw();
}

const std::map<PhysicalActor*, b2Body*> GameLogic::getVisibleActors() const {

    assert(_initialized);

    return _physicalActors;
}

const std::list<std::shared_ptr<NPC>> GameLogic::getNPCs() const {

    assert(_initialized);

    return _NPCs;
}

int GameLogic::getNumPoopsLeft() const {

    assert(_initialized);

    return _numPoopsLeft;
}

float GameLogic::getPoopTimeLeft() const {

    assert(_initialized);

    return 1.0f - _timeSinceLastPoop / _BIRD_DEATH_TIME;
}

int GameLogic::getPlayerScore() const {

    assert(_initialized);

    return _playerScore;
}

void GameLogic::requestBirdStartFly() {

    assert(_initialized);

    // only allow if state is PLAYING and the game is not paused
    if (_state == PLAYING && !_isPaused)
        _playableBirdActor.startFlying();
}

void GameLogic::requestBirdStopFly() {

    assert(_initialized);
    
    // only allow if state is PLAYING; don't worry about if the game is paused
    if (_state == PLAYING)
        _playableBirdActor.stopFlying();
}

void GameLogic::requestBirdPoop() {

    assert(_initialized);
    
    // only poop if all of the following:
    //     - the state is PLAYING
    //     - the game is not paused
    //     - the bird has at least one poop left
    //     - the bird is not currently pooping
    if (_state == PLAYING && !_isPaused && _numPoopsLeft > 0 && !_playableBirdActor.isPooping()) {

        // set variables to reflect poop start
        _playableBirdActor.startPooping();
        --_numPoopsLeft;
        _timeSinceLastPoop = 0.0f;

        // Make a poop obstacle and give it a vertical velocity which will make it shoot downward
        // from the bird.
        _obstacles.push_back(ObstacleFactory::makePoop(_playableBirdBody->GetLinearVelocity().y -
                _POOP_DOWNWARD_VELOCITY));
        _lastPoop = _obstacles.back().get();
        addToWorld(*_obstacles.back(), _playableBirdBody->GetPosition() - b2Vec2(0.5f, 0.5f),
                false);
    }
}

void GameLogic::requestNPCAction(NPC& npc, const NPC::ACTION& action, const float& delay,
            const float& duration) {
    
    // do not do a throw action if the mode isn't playing
    if (_state != PLAYING &&
            (action == NPC::ACTION::START_THROW || action == NPC::ACTION::FINISH_THROW))
        return;

    npc.doAction(action, delay, duration);

    // if the action was FINISH_THROW, then generate a rock
    if (action == NPC::ACTION::FINISH_THROW) {

        b2Body* npcBody = getBody(&npc);
        assert(npcBody);

        // determine spawn position of the rock and other initial variables
        b2Vec2 spawnPos = npcBody->GetPosition() + b2Vec2(0.0f, 2.5f);
        b2Vec2 birdPos = _playableBirdBody->GetPosition();
        b2Vec2 birdVel = _playableBirdBody->GetLinearVelocity();

        // Determine the velocity of the rock by doing some math, also add some random jitter
        // TODO: make throwing speed based on difficulty
        float S = _DEFAULT_NPC_THROW_SPEED;
        float G = -_GRAVITY.y;
        float toi = b2Distance(spawnPos, birdPos) / S;
        b2Vec2 P = (birdPos + toi * birdVel) - spawnPos;
        float desc = powf(S, 4.0f) - G * (G * P.x * P.x + 2 * S * S * P.y);
        float denom = G * P.x;
        float angle;
        if (desc >= 0.0f && denom != 0.0f)
            angle = atan2f(S * S - sqrtf(desc), denom);
        else
            angle = atan2f(P.y, P.x);
        angle += randomFloat(-0.1f, 0.1f);
        b2Vec2 rockVelocity = S * b2Vec2(cosf(angle), sinf(angle));

        // add the rock to the world and set its physical properties
        _projectiles.push_back(ObstacleFactory::makeRock());
        b2Body* rockBody = addToWorld(*_projectiles.back(), spawnPos, false);
        rockBody->ApplyLinearImpulseToCenter(rockBody->GetMass() * rockVelocity, true);

        // TODO: also change the npc's direction if necessary
    }
}

void GameLogic::gamePauseHandler(const Event& event) {

    assert(_initialized);

    // make sure the event is a GamePauseEvent and convert it
    assert(event.getType() == GamePauseEvent::TYPE);
    const GamePauseEvent& e = dynamic_cast<const GamePauseEvent&>(event);

    // ignore if the state is not PLAYING
    if (_state != PLAYING)
        return;

    // set the _isPaused variable accordingly
    if (e.action == GamePauseEvent::ACTION::PAUSE) {
        _isPaused = true;
    } else {
        _isPaused = false;
    }
}

void GameLogic::collisionHandler(const Event& event) {

    assert(_initialized);

    // make sure the event is a CollisionEvent and convert it
    assert(event.getType() == CollisionEvent::TYPE);
    const CollisionEvent& e = dynamic_cast<const CollisionEvent&>(event);

    // Make sure that both actors currently exist -- this is necessary because this handler may have
    // been called after the involved actors have been removed from the world.
    b2Body* bodyA = getBody(e.actorA);
    b2Body* bodyB = getBody(e.actorB);
    if (!(bodyA && bodyB))
        return;

    // call respective helper methods
    if (e.involvesType(PhysicalActor::TYPE::POOP))
        handlePoopCollision(e);
    if (e.involvesType(PhysicalActor::TYPE::PLAYABLE_BIRD))
        handleBirdCollision(e);
}

void GameLogic::handlePoopCollision(const CollisionEvent& e) {
    
    // get the actor which is the poop
    PhysicalActor* poop =
            e.actorA->getType() == PhysicalActor::TYPE::POOP ? e.actorA : e.actorB;
    
    // do not consider if the bird pooped on itself (lmao), or if the poop doesn't exist anymore
    if (e.involvesType(PhysicalActor::TYPE::PLAYABLE_BIRD) || getBody(poop) == nullptr)
        return;

    // if PLAYING, then the poop can have an effect
    if (_state == PLAYING) {

        // if it collided with an NPC, then increase the score and reset poops left
        if (e.involvesType(PhysicalActor::TYPE::NPC) && _state == PLAYING) {
            ++_playerScore;
            _numPoopsLeft = _BIRD_MAX_POOPS;
        
        // If it didn't collide with an NPC, it's the last poop, and they're aren't any poops
        // left, then it's game over.
        } else if (poop == _lastPoop && _numPoopsLeft <= 0) {
            eventMessenger.triggerEvent(GameOverEvent());
        }
    }

    // remove the poop from the world and add a poop splatter
    removeFromWorld(*poop);
    _obstacles.push_back(ObstacleFactory::makePoopSplatter());
    addToWorld(*_obstacles.back(), e.position);
}

void GameLogic::handleBirdCollision(const CollisionEvent& e) {

    // do not consider if the state isn't playing or the bird collided with its own poop
    if (_state != PLAYING || e.involvesType(PhysicalActor::TYPE::POOP))
        return;

    // the bird collided with something, so that's game over bro
    eventMessenger.triggerEvent(GameOverEvent());
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
        bool isOutOfBounds = body->GetPosition().x < -10.0f &&
                actor->getType() != PhysicalActor::TYPE::PLAYABLE_BIRD &&
                actor->getType() != PhysicalActor::TYPE::GROUND;
        if (isOutOfBounds)
            OOBActors.push_back(actor);
    }

    // next, iterate through the list of out of bounds actors and remove each one from the world
    for (auto actor : OOBActors) {
        removeFromWorld(*actor);
    }
}

void GameLogic::generateNewActors() {
    
    // The current metric for determining when to spawn actors is maintaining a minimum number of
    // entities on screen. If this number falls below some threshold (right now is 3), then we know
    // to spawn another one.
    // TODO: This is an okay placeholder, but we might want to think about using a more
    // sophisticated method of determining when to spawn things.
    int numEntities = _obstacles.size() + _NPCs.size();
    bool needToSpawn = numEntities < _difficulty;
    // If we do need to spawn something, then determine a random position past the right of the
    // screen and spawn an NPE there.
    float xPosition = NATIVE_RESOLUTION.x * METERS_PER_PIXEL + randomFloat(2.0f, 5.0f);
    if (needToSpawn && _worldScrollSpeed * _totalTimePassed + xPosition > _spawnPositionLastObstacle + 5.0f) {
        _spawnPositionLastObstacle = _worldScrollSpeed * _totalTimePassed + xPosition;
        spawnNPE(b2Vec2(xPosition, _GROUND_OFFSET_METERS));
    }
}

void GameLogic::spawnNPE(const b2Vec2& position) {

    //0: Streetlight
    //1: Tree
    //2: Cloud
    //3: Lifeguard Tower
    //4: Docks
    //5: Umbrella
    //6: NPC
    int obstacleType = randomInt(0, 6);
    
    //if the same type of obstacle is chosen twice in a row, repoll the random number generator
    while(obstacleType == _lastObstacleSpawned)
        obstacleType = randomInt(0, 6);

    // force the spawning of an NPC if there aren't any on screen
    if (_NPCs.size() == 0)
        obstacleType = 6;

    float heightMeters = randomFloat(4.0f, 9.0f);
    int numEntities = _obstacles.size() + _NPCs.size(); //used for checking whether an obstacle was actually generated or not
    bool faceLeft = randomBool();

    switch(obstacleType) {
        case 0:
            {
                if(_lastObstacleSpawned != 1) { //don't spawn streetlights directly after trees
                    _obstacles.push_back(ObstacleFactory::makeStreetlight(heightMeters, faceLeft));
                    
                    addToWorld(*_obstacles.back(), position);
                }
                break;
            }
        case 1:
            _obstacles.push_back(ObstacleFactory::makeTree(heightMeters));
            _spawnPositionLastObstacle += 1.44f + heightMeters;
            addToWorld(*_obstacles.back(), position);
            break;
        case 2:
            {
            float height = randomFloat(6.0f, 12.0f);
            if(_state == DEMO && height == _BIRD_DEMO_POSITION.y) {
                height = 11.0f;
            }
            _obstacles.push_back(ObstacleFactory::makeCloud());
            addToWorld(*_obstacles.back(), b2Vec2(position.x, height));
            break;
            }
        case 3:
            if(_lastObstacleSpawned != 4) { //don't spawn lifeguard towers directly after docks (doesn't make sense)
                _obstacles.push_back(ObstacleFactory::makeLifeguard(faceLeft));
                addToWorld(*_obstacles.back(), position);
                break;
            }
        case 4:
            {
                int width = randomInt(1, 5);
                int height = randomInt(1, 5);
                _obstacles.push_back(ObstacleFactory::makeDocks(width, height));
                addToWorld(*_obstacles.back(), position);
                _spawnPositionLastObstacle += width;
                bool spawnNPC = randomBool();
                if(spawnNPC) {
                    _NPCs.push_back(randomBool() ? NPCFactory::makeMale() : NPCFactory::makeFemale());
                    addToWorld(*_NPCs.back(), b2Vec2(position.x+randomFloat(2.0f, 2.0f+width), height));
                    _timeSinceLastNPC = 0.0f;
                }
                break;
            }
        case 5:
            {
                float angle = randomFloat(-PI/4.0f, PI / 4.0f);
                _obstacles.push_back(ObstacleFactory::makeUmbrella(angle));
                addToWorld(*_obstacles.back(), position);
                break;
            }
        case 6:
            _NPCs.push_back(randomBool() ? NPCFactory::makeMale() : NPCFactory::makeFemale());
            addToWorld(*_NPCs.back(), position);
            _timeSinceLastNPC = 0.0f;
            break;
    }
    if(obstacleType != 6 && numEntities != _obstacles.size() + _NPCs.size()) { //this is called in all cases regardless of whether an obstacle was actually spawned or not
        _lastObstacleSpawned = obstacleType;
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

    // set the user data, position, and velocity of the body
    bodyDef.userData.pointer = (uintptr_t)actorAddress;
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
    b2Body* body = getBody(actorAddress);

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
    removeFromList(actor, _projectiles);
}

void GameLogic::removeAllFromWorld() {
    while (true) {
        auto i = _physicalActors.begin();
        if (i == _physicalActors.end())
            return;
        removeFromWorld(*i->first);
    }
}

b2Body* GameLogic::getBody(const PhysicalActor* actor) const {

    assert(_initialized);

    // return nullptr if actor does not have a physical body
    PhysicalActor* actorAddress = (PhysicalActor*)actor;

    if (_physicalActors.find(actorAddress) == _physicalActors.end()){
        return nullptr;    
    }
    
    // return corresponding body pointer
    return _physicalActors.at(actorAddress);
}

void GameLogic::updatePlayableBird(const float& timeDelta) {

    assert(_initialized);
    
    // make sure the bird's body isn't nullptr
    assert(_playableBirdBody);

    // update bird pooping status
    _timeSinceLastPoop += timeDelta;
    if (_timeSinceLastPoop >= _BIRD_POOP_DURATION)
        _playableBirdActor.stopPooping();

    // kill the bird if it hasn't pooped for a while
    if (_timeSinceLastPoop >= _BIRD_DEATH_TIME && _state == PLAYING) {
        eventMessenger.triggerEvent(GameOverEvent());
    }
    
    // if the bird is flying and state is PLAYING, then apply an upward force opposite to gravity
    if (_playableBirdActor.isFlying() && _state == PLAYING) {
        float targetAccel = -2.0f * _GRAVITY.y;
        float force = _playableBirdBody->GetMass() * targetAccel;
        _playableBirdBody->ApplyForceToCenter(b2Vec2(0.0f, force), true);
    }

    // get bird's position and velocity
    const b2Vec2& velocity = _playableBirdBody->GetLinearVelocity();
    const b2Vec2& position = _playableBirdBody->GetPosition();
    
    // Set the bird's rotation based on its velocity, dampen the rotation slightly so it's not so
    // severe. Also make sure the bird is in its demo x-position. Only do this if the state isn't
    // GAME_OVER, so that the bird's body can ragdoll when the game is over
    if (_state != GAME_OVER) {
        float angle = atan2f(velocity.y, _worldScrollSpeed * 2.0f);
        _playableBirdBody->SetTransform(b2Vec2(_BIRD_DEMO_POSITION.x, position.y), angle);
        _playableBirdBody->SetLinearVelocity(b2Vec2(0.0f, velocity.y));
    }

    // Prevent the bird from going past the top of the screen. Accomplished by having an area at the
    // top of the screen that gradually caps the bird's upward velocity.
    if (position.y > _BIRD_SLOW_HEIGHT) {
        float lerpValue = clamp((position.y - _BIRD_SLOW_HEIGHT) /
                (_BIRD_MAX_HEIGHT - _BIRD_SLOW_HEIGHT), 0.0f, 1.0f);
        float maxVelocity = _BIRD_MAX_VELOCITY * (1.0f - lerpValue);
        if (velocity.y > maxVelocity)
            _playableBirdBody->SetLinearVelocity(b2Vec2(velocity.x, maxVelocity));
        if (position.y > _BIRD_MAX_HEIGHT)
            _playableBirdBody->SetTransform(b2Vec2(position.x, _BIRD_MAX_HEIGHT),
                    _playableBirdBody->GetAngle());
    }

    // call bird's own update() method
    _playableBirdActor.update(timeDelta);
}

void GameLogic::updateNPCs(const float& timeDelta) {

    assert(_initialized);

    for (auto npc : _NPCs) {

        b2Body* npcBody = getBody(npc.get());
        assert(npcBody);

        // if the npc is walking, then move it in the direction it's facing
        if (npc->isWalking()) {

            float xVelocity = -_worldScrollSpeed +
                    _NPC_WALK_SPEED * (npc->isFacingLeft() ? -1.0f : 1.0f);
            npcBody->SetLinearVelocity(b2Vec2(xVelocity, npcBody->GetLinearVelocity().y));
        
        // if the npc is throwing, then make it always face the bird
        } else if (npc->isThrowing()) {
            bool shouldFaceLeft = _playableBirdBody->GetPosition().x < npcBody->GetPosition().x;
            npc->setFacingLeft(shouldFaceLeft);
        }

        npc->update(timeDelta);
    }
}

void GameLogic::updateGround() {

    assert(_initialized);

    // ensure that the ground covers the screen as best as possible
    while (true) {

        // get the leftmost ground (the one at the front of the list is always the leftmost)
        std::shared_ptr<Obstacle> leftGround = _grounds.front();
        b2Body* leftGroundBody = getBody(leftGround.get());

        // sanity check -- make sure the body was able to be found
        assert(leftGroundBody);

        // if this ground obstacle is a little past the left edge of the screen, then need to:
        //   1. move this ground to the right of the rightmost ground
        //   2. make this ground the rightmost ground in the list
        if (leftGroundBody->GetPosition().x <= -5.0f) {

            b2Body* rightGroundBody = getBody(_grounds.back().get());
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

void GameLogic::updateDifficulty() {
    _difficulty = (_totalTimePassed / 30) + 3;

}

void GameLogic::setWorldScrollSpeed(const float& amount) {

    assert(_initialized);

    // iterate through all bodies and change their velocities
    for (auto& pair : _physicalActors) {

        // get the actor
        PhysicalActor* actor = pair.first;
        assert(actor);

        // only apply velocity if it's of type GROUND, NPC, or GENERIC_OBSTACLE
        if (
            actor->getType() == PhysicalActor::TYPE::GROUND ||
            actor->getType() == PhysicalActor::TYPE::NPC ||
            actor->getType() == PhysicalActor::TYPE::GENERIC_OBSTACLE
        ) {
            b2Body* body = pair.second;
            assert(body);
            body->SetLinearVelocity(b2Vec2(-_worldScrollSpeed, body->GetLinearVelocity().y));
        }
    }

    // update the _worldScrollSpeed variable
    _worldScrollSpeed = amount;
}
