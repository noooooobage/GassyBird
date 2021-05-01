#include <cassert>
#include <vector>
#include <memory>
#include <iostream>
#include <math.h>
#include <algorithm>

#include <box2d/box2d.h>

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
    _STEP_TIME(.5f),

    _NUM_GROUNDS(4),
    _GROUND_WIDTH_METERS(400 * METERS_PER_PIXEL),
    _GROUND_OFFSET_METERS(0.5f),

    _playableBirdBody(nullptr),
    _BIRD_DEMO_POSITION(b2Vec2(8.0f, 10.0f)),
    _BIRD_MAX_HEIGHT(NATIVE_RESOLUTION.y * METERS_PER_PIXEL - 0.5f),
    _BIRD_SLOW_HEIGHT(_BIRD_MAX_HEIGHT - 1.0f),
    _BIRD_MAX_VELOCITY(sqrtf((NATIVE_RESOLUTION.y * METERS_PER_PIXEL -_GROUND_OFFSET_METERS) *
            -_GRAVITY.y * 2.0f)),
    _BIRD_POOP_DURATION(1.0f),
    _BIRD_MAX_POOPS(2),
    _POOP_DOWNWARD_VELOCITY(3.0f),
    _BIRD_DEATH_TIME(60.0f),
    _lastPoop(nullptr),
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
    updateNPCs();
    updateDifficulty();
    _timeSinceLastNPC += timeDelta;
    // Ensure that GROUND and GENERIC_OBSTACLE actors are moving at the world scroll speed. This is
    // necessary to do every frame because sometimes varying timeDeltas affects the velocity
    setWorldScrollSpeed(_worldScrollSpeed);
    
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
    _totalTimePassed = 0.0f;
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
    _totalTimePassed = 0.0f;
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

const std::unordered_map<PhysicalActor*, b2Body*> GameLogic::getVisibleActors() const {

    assert(_initialized);

    return _physicalActors;
}

int GameLogic::getNumPoopsLeft() const {

    assert(_initialized);

    return _numPoopsLeft;
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

    // do not consider if the state isn't PLAYING or if the bird pooped on itself (lmao)
    if (_state != PLAYING || e.involvesType(PhysicalActor::TYPE::PLAYABLE_BIRD))
        return;

    // get the actor which is the poop
    PhysicalActor* poop =
            e.actorA->getType() == PhysicalActor::TYPE::POOP ? e.actorA : e.actorB;

    // if the poop isn't already in the dead poops list, it can still have an effect
    if (std::find(_deadPoops.begin(), _deadPoops.end(), poop) == _deadPoops.end()) {

        // add to dead poops list
        _deadPoops.push_back(poop);

        // if it collided with an NPC, then increase the score and reset poops left
        if (e.involvesType(PhysicalActor::TYPE::NPC)) {
            ++_playerScore;
            _numPoopsLeft = _BIRD_MAX_POOPS;
        
        // If it didn't collide with an NPC, it's the last poop, and they're aren't any poops
        // left, then it's game over.
        } else if (poop == _lastPoop && _numPoopsLeft <= 0) {
            eventMessenger.triggerEvent(GameOverEvent());
        }
    }
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

void GameLogic::requestNPCStep() {

    assert(_initialized);
    //Get some random NPC from the NPC list
    int index = randomInt(0,_NPCs.size());
    auto curNPC = _NPCs.begin();
    std::advance(curNPC, index);

    std::shared_ptr<NPC> item = *curNPC;
    
    //if the game is playing
    if (_state == PLAYING){    
        //Get that _NPCs body
        //curNPC->moveRight();
        //Change the velocity
        b2BodyDef body = item->getBodyDef();
        body.linearVelocity = (b2Vec2(_worldScrollSpeed, 0.f));
        //start time of entity since it last moved
        item->isMoving = true;
    }
}

void GameLogic::requestTriggerAction(){
    assert(_initialized);

    
    //If there are NPCs to trigger
    if (_NPCs.size() > 0){
        //Get NPC from the NPC list
        auto curNPC = _NPCs.begin();
        
        //if the game is playing
        if (_state == PLAYING){
            //Call the change of animation frames
            //curNPC->triggerAction();
            //get that NPC actor and activate its animation

            //Temp code for trigger event            
            b2Body* tbody = getBody(curNPC->get());

            //Safety check for null pointer
            if (tbody != nullptr){
                //Calculate the angle for the throw
                float opp = (_playableBirdBody->GetPosition().y - tbody->GetPosition().y);
                float adj = (_playableBirdBody->GetPosition().x - tbody->GetPosition().x);
                float angle = (1 / (tan(opp/adj)));

                //Create the rock
                _projectiles.push_back(ObstacleFactory::makeRock(angle));
                auto rBody = addToWorld(*_projectiles.back(), {tbody->GetPosition().x, 5.f}, true);
                

                //Safety check for nullptr
                if(rBody != nullptr){
                    float targetAccel = -2.0f;
                    float force = rBody->GetMass() * targetAccel;
                    rBody->ApplyForceToCenter(b2Vec2(-200.0f, force), true);
                    std::cout << "Push" << std::endl;
                }

                
            }
        }
    }
    //Temp else
    else{
        std::cout << "No NPCs" << std::endl;
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
    else if(_timeSinceLastNPC >= _BIRD_DEATH_TIME/2.0f) { //if the game has not spawned an NPC within the time limit, spawn one and return    
        // std::cout << "Game should be forced to spawn an NPC" << std::endl;
        _NPCs.push_back(NPCFactory::makeDefault());
        addToWorld(*_NPCs.back(), b2Vec2(xPosition, _GROUND_OFFSET_METERS));
        _spawnPositionLastObstacle = _worldScrollSpeed * _totalTimePassed + xPosition;
        _timeSinceLastNPC = 0.0f;
    }
}

void GameLogic::spawnNPE(const b2Vec2& position) {

    // Only two options right now: streetlight or default NPC, so just decide between the two with a
    // random bool.
    //0: Streetlight
    //1: Tree
    //2: Cloud
    //3: Lifeguard Tower
    //4: Docks
    //5: Spawn NPC
    //6: Umbrella
    int obstacleType = randomInt(0, 6);
    float metersOffset;
    //if the same type of obstacle is chosen twice in a row, repoll the random number generator
    while(obstacleType == _lastObstacleSpawned) {
        obstacleType = randomInt(0, 6);
    }
    float heightMeters = randomFloat(4.0f, 9.0f);
    bool spawnStreetlight = randomBool();
    bool faceLeft = randomBool();
    switch(obstacleType) {
        case 0:
            {
            _obstacles.push_back(ObstacleFactory::makeStreetlight(heightMeters, faceLeft));
            
            addToWorld(*_obstacles.back(), position);
            break;
            }
        case 1:
            _obstacles.push_back(ObstacleFactory::makeTree(heightMeters));
            _spawnPositionLastObstacle += 1.44f;
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
            _obstacles.push_back(ObstacleFactory::makeLifeguard(faceLeft));
            addToWorld(*_obstacles.back(), position);
            break;
        case 4:
            {
                int width = randomInt(1, 5);
                int height = randomInt(1, 5);
                _obstacles.push_back(ObstacleFactory::makeDocks(width, height));
                addToWorld(*_obstacles.back(), position);
                _spawnPositionLastObstacle += width;
                bool spawnNPC = randomBool();
                if(spawnNPC) {
                    _NPCs.push_back(NPCFactory::makeDefault());
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
            _NPCs.push_back(NPCFactory::makeDefault());
            addToWorld(*_NPCs.back(), position);
            _timeSinceLastNPC = 0.0f;
            break;
    }
    if(obstacleType != 5) {
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
    _deadPoops.remove(actorAddress);
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

void GameLogic::updateGround() {

    assert(_initialized);

    // ensure that the ground covers the screen as best as possible
    while (true) {

        // get the leftmost ground (the one at the front of the list is always the leftmost)
        std::shared_ptr<Obstacle> leftGround = _grounds.front();
        b2Body* leftGroundBody = getBody(leftGround.get());

        // sanity check -- make sure the body was able to be found
        assert(leftGroundBody);

        // if this ground obstacle is a little to the left of the screen, then need to:
        //   1. move this ground to the right of the rightmost ground
        //   2. make this ground the rightmost ground in the list
        if (leftGroundBody->GetPosition().x <= -1.0f) {

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

void GameLogic::updateNPCs(){
    assert(_initialized);

    if (_NPCs.size() > 0){
        
        //iterate through the _NPCs
        for (auto& item : _NPCs) {

            std::shared_ptr<NPC> curNPC = item;

            // make sure actor and body aren't nullptr
            assert(curNPC);

            if (curNPC->getType() == PhysicalActor::TYPE::NPC){
                //Check their move timer
                //Compare and decide if it needs to be reset
                if (curNPC->getTimeMoving() == _STEP_TIME){
                    //Set time to zero and speed to world speed
                    curNPC->setTimeMoving();

                    b2BodyDef body = curNPC->getBodyDef();
                    body.linearVelocity = (b2Vec2(-_worldScrollSpeed, 0.f));
                    //body->SetLinearVelocity(b2Vec2(-_worldScrollSpeed, body->GetLinearVelocity().y));
                    curNPC->isMoving = false;
                }
                    
                //Check their throw timer
                    //Compare and decide if it needs to be reset
            }
        }
    }
}
