#pragma once
#ifndef EVENTS_H

#include <deque>

enum EVENT_TYPE {
	EVENT_NOTIFY,
	EVENT_COUNT,
};

struct Event {
	EVENT_TYPE type;
	const char* notify_message = nullptr;
};

class EventBus {
	std::deque<Event> events;
public:
	std::deque<Event>& getEvents();

	void pushEvent(Event event);
	void emptyEvents();

};

#endif
