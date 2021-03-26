/**
 * Sets the TYPE attribute of all classes which inherit from Event.
 */

#include "Event.hpp"
#include "Events/KeyPressEvent.hpp"
#include "Events/KeyReleaseEvent.hpp"
#include "Events/WindowResizeEvent.hpp"
#include "Events/WindowCloseEvent.hpp"
#include "Events/ButtonPressEvent.hpp"

const EventType WindowResizeEvent::TYPE = (EventType)&WindowResizeEvent::TYPE;
const EventType WindowCloseEvent::TYPE = (EventType)&WindowCloseEvent::TYPE;
const EventType KeyPressEvent::TYPE = (EventType)&KeyPressEvent::TYPE;
const EventType KeyReleaseEvent::TYPE = (EventType)&KeyReleaseEvent::TYPE;
const EventType ButtonPressEvent::TYPE = (EventType)&ButtonPressEvent::TYPE;

