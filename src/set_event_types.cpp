/**
 * Sets the TYPE attribute of all classes which inherit from Event.
 */

#include "Event.hpp"
#include "Events/WindowResizeEvent.hpp"
#include "Events/WindowCloseEvent.hpp"
#include "Events/KeyPressEvent.hpp"
#include "Events/KeyReleaseEvent.hpp"
#include "Events/MouseMoveEvent.hpp"
#include "Events/MousePressEvent.hpp"
#include "Events/MouseReleaseEvent.hpp"
#include "Events/ButtonClickEvent.hpp"
#include "Events/GamePauseEvent.hpp"

const EventType WindowResizeEvent::TYPE = (EventType)&WindowResizeEvent::TYPE;
const EventType WindowCloseEvent::TYPE =  (EventType)&WindowCloseEvent::TYPE;
const EventType KeyPressEvent::TYPE =     (EventType)&KeyPressEvent::TYPE;
const EventType KeyReleaseEvent::TYPE =   (EventType)&KeyReleaseEvent::TYPE;
const EventType MouseMoveEvent::TYPE =    (EventType)&MouseMoveEvent::TYPE;
const EventType MousePressEvent::TYPE =   (EventType)&MousePressEvent::TYPE;
const EventType MouseReleaseEvent::TYPE = (EventType)&MouseReleaseEvent::TYPE;
const EventType ButtonClickEvent::TYPE =  (EventType)&ButtonClickEvent::TYPE;
const EventType GamePauseEvent::TYPE =    (EventType)&GamePauseEvent::TYPE;
