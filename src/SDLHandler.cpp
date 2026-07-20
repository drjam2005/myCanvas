#include "SDLHandler.h"
#include <iostream>
#include <cstdio>

namespace {
    float latestPressure = 1.0f;
    float latestTiltX = 0.0f;
    float latestTiltY = 0.0f;
    bool penActive = false;   // true while the pen tip is touching the surface
    bool penInRange = false;  // true while the pen is hovering/in proximity, even if not touching
    bool initialized = false;

    // Sticky "this happened since last consumed" flags. SDL_AddEventWatch fires
    // synchronously inside SDL_PollEvent, so these are set during PumpSDLTabletInput()
    // and should be consumed once per frame by the caller.
    bool penJustPressedFlag = false;
    bool penJustReleasedFlag = false;
}

bool SDLCALL WatchSDLEvent(void*, SDL_Event* event)
{
    switch (event->type)
    {
        case SDL_EVENT_PEN_PROXIMITY_IN:
        {
			std::cout << "penInRange" << '\n';
            penInRange = true;
            break;
        }
        case SDL_EVENT_PEN_PROXIMITY_OUT:
        {
			std::cout << "penOutRange" << '\n';
            penInRange = false;
            penActive = false;
            latestPressure = 0.0f;
            break;
        }
        case SDL_EVENT_PEN_DOWN:
        {
            penActive = true;
            penJustPressedFlag = true;
            break;
        }
        case SDL_EVENT_PEN_UP:
        {
            penActive = false;
            penJustReleasedFlag = true;
            latestPressure = 0.0f;
            break;
        }
        case SDL_EVENT_PEN_AXIS:
        {
            const SDL_PenAxisEvent& axisEvent = event->paxis;
            switch (axisEvent.axis)
            {
                case SDL_PEN_AXIS_PRESSURE:
                    latestPressure = axisEvent.value;
                    break;
                case SDL_PEN_AXIS_XTILT:
                    latestTiltX = axisEvent.value;
                    break;
                case SDL_PEN_AXIS_YTILT:
                    latestTiltY = axisEvent.value;
                    break;
                default:
                    break;
            }
            break;
        }
        default:
            break;
    }

    return true;
}

bool InitSDLTabletInput()
{
    if (initialized)
        return true;

    printf("SDL version: %s\n", SDL_GetRevision());

    // Pen -> mouse motion synthesis needs to stay ON, otherwise raylib's own
    // GetMousePos() never updates while drawing with the tablet (it only moves
    // in response to real SDL mouse-motion events). Press/release/pressure are
    // already read directly from real pen events elsewhere in this file, so
    // this only affects position tracking, not double-firing clicks.
    SDL_SetHint(SDL_HINT_PEN_MOUSE_EVENTS, "1");
    SDL_SetHint(SDL_HINT_PEN_TOUCH_EVENTS, "0");

    if (!SDL_AddEventWatch(WatchSDLEvent, nullptr)) {
        printf("Failed to add event watch: %s\n", SDL_GetError());
    } else {
        printf("SDL event watcher installed\n");
    }

    initialized = true;
    return true;
}

void PumpSDLTabletInput() { }

void ShutdownSDLTabletInput()
{
    if (!initialized)
        return;

    SDL_RemoveEventWatch(WatchSDLEvent, nullptr);
    initialized = false;
    penActive = false;
    penInRange = false;
    latestPressure = 1.0f;
    latestTiltX = 0.0f;
    latestTiltY = 0.0f;
}

bool GetLatestTabletPressure(float* pressure)
{
    if (pressure == nullptr || !penActive)
        return false;

    *pressure = latestPressure;
    return true;
}


bool IsTabletPenDown()
{
    return penActive;
}

bool ConsumeTabletPenPressed()
{
    bool value = penJustPressedFlag;
    penJustPressedFlag = false;
    return value;
}

// Same as above but for pen-up.
bool ConsumeTabletPenReleased()
{
    bool value = penJustReleasedFlag;
    penJustReleasedFlag = false;
    return value;
}

