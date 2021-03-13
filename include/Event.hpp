#ifndef _EVENT_HPP_
#define _EVENT_HPP_

typedef void* EventType;

class Event {

public:

    virtual ~Event() {}

    virtual const EventType& getType() const = 0;
};

#endif // _EVENT_HPP_
