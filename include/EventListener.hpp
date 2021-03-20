#ifndef _EVENT_LISTENER_HPP_
#define _EVENT_LISTENER_HPP_

#include <functional>
#include <memory>
#include <cassert>

#include "Event.hpp"

/**
 * A EventListener is a callback function to be called when an event of a certain type is triggered.
 * Every EventListener has the following signature: void (const Event&).
 * 
 * Once created, a listener can only be called from the event messenger. This reduces the chance of
 * calling a member listener once its associated object no longer exists.
 */
class EventListener {

public:

    /**
     * Default constructor
     */
    EventListener() : _initialized(false) {}

    /**
     * Initializes from a non-member function. These can be:
     *   - functions that are not part of a class
     *   - static functions of a class
     * 
     * @param function_address memory address of the callback function
     * 
     * EXAMPLE:
     * 
     * void foo(const Event& event) {}
     * 
     * EventListener listener;
     * listener.init(&foo);
     */
    void init(void (*function_address)(const Event&)) {
        _function = std::function<void (const Event&)>(function_address);
        _initialized = true;
    }

    /**
     * Initializes from a member function, i.e. a non-static method of a class. Make sure the member
     * function's object remains alive during the lifetime of the EventListener.
     * 
     * @param member_function_address memory address of the member function
     * @param object_address memory address of the object
     * 
     * EXAMPLE:
     * 
     * class Foo {
     * public:
     *     void foo(const Event& event) {}
     * }
     * 
     * Foo fooInstance;
     * EventListener listener;
     * listener.init(&Foo::foo, &fooInstance);
     */
    template <typename T>
    void init(void (T::*member_function_address)(const Event&), T* object_address) {
        _function = std::bind(member_function_address, object_address, std::placeholders::_1);
        _initialized = true;
    }

    // the event messenger is the only class able to use the () operator
    friend class EventMessenger;

private:

    /**
     * Using the () operator will invoke the stored function with the given event. This is private
     * and only available to friend classes.
     */
    void operator()(const Event& event) const {
        assert(_initialized);
        _function(event);
    }

    bool _initialized;

    // stored function
    std::function<void(const Event&)> _function;
};

#endif // _EVENT_LISTENER_HPP_
