#ifndef _EVENT_MESSENGER_HPP_
#define _EVENT_MESSENGER_HPP_

#include <unordered_map>
#include <list>
#include <array>
#include <functional>
#include <memory>

#include "Event.hpp"
#include "EventListener.hpp"

typedef std::list<std::shared_ptr<Event>> EventQueue;
typedef std::list<const EventListener*> ListenerList;

/**
 * A system which handles Events and EventListeners. Every stored EventListener is associated with
 * some Event type. When an event is triggered, all listeners associated with the event's type are
 * called.
 * 
 * The event messenger does not store copies of the event listeners; rather, it only retains
 * references. Therefore, event listeners must be removed from the event messenger before they are
 * destroyed.
 */
class EventMessenger {

public:

    /**
     * Default constructor.
     */
    EventMessenger() {}

    /**
     * Adds an event listener which will be called when an event of eventType is triggered.
     * Duplicate listeners are not allowed; if a duplicate listener is attempted to be added, then
     * it is ignored.
     * 
     * This does not make a copy of the event listener, so the listener must remain valid until it
     * is removed from the event messenger. Thus, if an object adds event listeners which are member
     * functions, then it must remove those listeners before the object is destroyed.
     */
    void addListener(const EventType& eventType, const EventListener& listener);

    /**
     * Removes the given event listener associated with the given eventType, so that it will no
     * longer be called when an event of eventType is triggered. If the listener is not found, then
     * nothing happens.
     */
    void removeListener(const EventType& eventType, const EventListener& listener);

    /**
     * Queues the given event which will be triggered (i.e. it's listeners will be called) at a
     * later time. All queued events are triggered once per frame, at the end of Game::update(), in
     * the order in which they were queued. Once a queued event is triggered, it is removed from the
     * queue.
     * 
     * This stores a copy of the given event, so the caller of queueEvent() doesn't need to worry
     * about managing the event.
     */
    template <typename T>
    void queueEvent(const T& event) {
        // copy event and push to second queue
        _eventQueues.at(1).push_back(std::make_shared<T>(event));
    }

    /**
     * Immediately triggers the given event (i.e. its listeners are called). Since the event's
     * listeners are called immediately, be careful not to trigger an event whose listeners may
     * affect another process in an adverse way. E.g. don't trigger an event that modifies the
     * properties of physical bodies when the game's physics engine is computing; instead, queue the
     * event with queueEvent().
     */
    void triggerEvent(const Event& event);

    // the Game class is the only class able to trigger all queued events
    friend class Game;

private:

    /**
     * Triggers all events which have been queued up to this point. Any events which are queued
     * during this triggering process will be triggered during the following frame.
     */
    void triggerQueuedEvents();

    // stores events listeners by event type
    std::unordered_map<EventType, ListenerList> _listeners;

    // When events are queued, they are pushed to the second queue. When it's time to trigger all
    // queued events, the first and second queues are swapped, and all the events that are now in
    // the first queue (once the second queue) are triggered. This queue swapping technique ensures
    // that events cannot be pushed to the first queue while triggerQueuedEvents() is running.
    std::array<EventQueue, 2> _eventQueues;
};

#endif // _EVENT_MESSENGER_HPP_
