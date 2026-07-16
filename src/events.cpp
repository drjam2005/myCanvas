#include "events.h"

std::deque<Event>& EventBus::getEvents() { 
	return this->events;
}

void EventBus::emptyEvents() {
	this->events.clear();
}

void EventBus::pushEvent(Event event) {
	this->events.push_back(event);
}
