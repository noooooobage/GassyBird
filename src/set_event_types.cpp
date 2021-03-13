/**
 * Sets the TYPE attribute of all classes which inherit from Event.
 */

#include "Event.hpp"
#include "Events/WindowResizeEvent.hpp"
#include "Events/WindowCloseEvent.hpp"

const EventType WindowResizeEvent::TYPE = (EventType)&WindowResizeEvent::TYPE;
const EventType WindowCloseEvent::TYPE = (EventType)&WindowCloseEvent::TYPE;
