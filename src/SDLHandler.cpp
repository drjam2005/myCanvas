#include "SDLHandler.h"

#include <mutex>

namespace {
std::mutex penEventsMutex;
std::deque<SDL_Event> penEvents;
}

bool SDLCALL WatchSDLEvent(void*, SDL_Event* event)
{
    switch (event->type) {
        case SDL_EVENT_PEN_PROXIMITY_IN:
        case SDL_EVENT_PEN_PROXIMITY_OUT:
        case SDL_EVENT_PEN_DOWN:
        case SDL_EVENT_PEN_UP:
        case SDL_EVENT_PEN_MOTION:
        case SDL_EVENT_PEN_AXIS:
        {
            std::lock_guard<std::mutex> lock(penEventsMutex);
            penEvents.push_back(*event);
            break;
        }
        default:
            break;
    }

    return true;
}

std::deque<SDL_Event> DrainPenEvents()
{
    std::deque<SDL_Event> events;
    std::lock_guard<std::mutex> lock(penEventsMutex);
    events.swap(penEvents);
    return events;
}
