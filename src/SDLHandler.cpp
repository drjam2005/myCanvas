#include "SDLHandler.h"
#include "SDL3/SDL_events.h"
#include <cstdio>

namespace {
    float latestPressure = 1.0f;
    bool penActive = false;
    bool penInRange = false;
    bool initialized = false;

	float penLastPositionX = 0.0f;
	float penLastPositionY = 0.0f;

    bool penJustPressedFlag = false;
    bool penJustReleasedFlag = false;

}

bool SDLCALL WatchSDLEvent(void*, SDL_Event* event)
{
    switch (event->type)
    {
        case SDL_EVENT_PEN_PROXIMITY_IN:
        case SDL_EVENT_PEN_DOWN:
        case SDL_EVENT_PEN_UP:
        case SDL_EVENT_PEN_AXIS:
            penLastPositionX = event->paxis.x;
            penLastPositionY = event->paxis.y;
            break;

        case SDL_EVENT_PEN_MOTION:
            penLastPositionX = event->pmotion.x;
            penLastPositionY = event->pmotion.y;
            break;
    }

    switch (event->type)
    {
        case SDL_EVENT_PEN_PROXIMITY_IN:
            penInRange = true;
            break;

        case SDL_EVENT_PEN_PROXIMITY_OUT:
            penInRange = false;
            penActive = false;
            latestPressure = 0.0f;
            break;

        case SDL_EVENT_PEN_DOWN:
            penInRange = true;
            penActive = true;
            penJustPressedFlag = true;
            break;

        case SDL_EVENT_PEN_MOTION:
            penInRange = true;
            break;

        case SDL_EVENT_PEN_UP:
            penInRange = true;
            penActive = false;
            penJustReleasedFlag = true;
            latestPressure = 0.0f;
            break;

        case SDL_EVENT_PEN_AXIS:
            if (event->paxis.axis == SDL_PEN_AXIS_PRESSURE)
                latestPressure = event->paxis.value;
            break;
    }

    return true;
}

bool InitSDLTabletInput() {
    if (initialized)
        return true;

    SDL_SetHint(SDL_HINT_PEN_MOUSE_EVENTS, "1");
    SDL_SetHint(SDL_HINT_PEN_TOUCH_EVENTS, "0");

    if (!SDL_AddEventWatch(WatchSDLEvent, nullptr)) {
        printf("Failed to add event watch: %s\n", SDL_GetError());
    }

    initialized = true;
    return true;
}

void PumpSDLTabletInput() {
	SDL_PumpEvents();
}

void ShutdownSDLTabletInput() {
    if (!initialized)
        return;

    SDL_RemoveEventWatch(WatchSDLEvent, nullptr);
    initialized = false;
    penActive = false;
    penInRange = false;
    latestPressure = 1.0f;
}

bool GetLatestTabletPressure(float* pressure) {
    if (pressure == nullptr || !penActive)
        return false;

    *pressure = latestPressure;
    return true;
}

void GetLatestTabletPosition(float* x, float* y) {
	*x = penLastPositionX;
	*y = penLastPositionY;
}


bool IsTabletPenDown() {
    return penActive;
}

bool IsTabletPenInProximity() {
    return penInRange;
}

bool ConsumeTabletPenPressed() {
    bool value = penJustPressedFlag;
    penJustPressedFlag = false;
    return value;
}

bool ConsumeTabletPenReleased() {
    bool value = penJustReleasedFlag;
    penJustReleasedFlag = false;
    return value;
}
