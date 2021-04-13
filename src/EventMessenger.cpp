#include <memory>
#include <list>
#include <cassert>
#include <algorithm>
#include <iostream>

#include "EventMessenger.hpp"

void EventMessenger::addListener(const EventType& eventType, const EventListener& listener) {

    // get the list associated with the event type (this also creates one if it doesn't exist yet)
    ListenerList& listenerList = _listeners[eventType];
    
    // only add the listener if it doesn't exist in the list
    if (std::find(listenerList.begin(), listenerList.end(), &listener) == listenerList.end())
        listenerList.push_back(&listener);
}

void EventMessenger::removeListener(const EventType& eventType, const EventListener& listener) {
    _listeners[eventType].remove(&listener);
}

void EventMessenger::triggerEvent(const Event& event) {

    // Copy the list of listeners to call. Very important that this is copied, as using the actual
    // list could segfualt if the list changes during iteration.
    ListenerList listenerList = _listeners[event.getType()];

    // iterate through list and call all listeners
    for (const EventListener*& listener : listenerList) 
        (*listener)(event);
}

void EventMessenger::triggerQueuedEvents() {

    // switch first and second queues
    std::swap(_eventQueues.at(0), _eventQueues.at(1));

    // trigger all events in first queue; after event is triggered, free its memory
    for (std::shared_ptr<Event>& event : _eventQueues.at(0)) {
        triggerEvent(*event);
        event.reset();
    }
    
    // clear first queue
    _eventQueues.at(0).clear();
}
