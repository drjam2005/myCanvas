#include "SDLHandler.h"
#include <cstdio>
#include <vector>

namespace {
    float latestPressure = 1.0f;
    float latestTiltX = 0.0f;
    float latestTiltY = 0.0f;
    float latestPenX = 0.0f;
    float latestPenY = 0.0f;
    bool penActive = false;   // true while the pen tip is touching the surface
    bool penInRange = false;  // true while the pen is hovering/in proximity, even if not touching
    bool initialized = false;

    // Sticky "this happened since last consumed" flags. SDL_AddEventWatch fires
    // synchronously inside SDL_PollEvent, so these are set during PumpSDLTabletInput()
    // and should be consumed once per frame by the caller.
    bool penJustPressedFlag = false;
    bool penJustReleasedFlag = false;

    // Every real hardware sample the pen produces gets appended here, instead
    // of just overwriting "latest" values. Rendering only the latest value per
    // frame loses every intermediate point when the tablet reports faster than
    // you render (or than you happen to render THIS particular frame) -- that's
    // what produces the faceted/chunky stroke edges. Consumers should drain
    // this every frame and draw one segment per sample, not one per frame.
    std::vector<PenSample> pendingSamples;

    void PushSample(bool down)
    {
        PenSample sample;
        sample.x = latestPenX;
        sample.y = latestPenY;
        sample.pressure = latestPressure;
        sample.down = down;
        pendingSamples.push_back(sample);
    }
}

bool SDLCALL WatchSDLEvent(void*, SDL_Event* event)
{
    switch (event->type)
    {
        case SDL_EVENT_PEN_PROXIMITY_IN:
        {
            penInRange = true;
            break;
        }
        case SDL_EVENT_PEN_PROXIMITY_OUT:
        {
            penInRange = false;
            penActive = false;
            latestPressure = 0.0f;
            break;
        }
        case SDL_EVENT_PEN_DOWN:
        {
            penActive = true;
            penJustPressedFlag = true;
            latestPenX = event->ptouch.x;
            latestPenY = event->ptouch.y;
            PushSample(true);
            break;
        }
        case SDL_EVENT_PEN_UP:
        {
            penActive = false;
            penJustReleasedFlag = true;
            latestPenX = event->ptouch.x;
            latestPenY = event->ptouch.y;
            // Push a final sample at release so the last drawn segment ends
            // exactly where the pen actually lifted, then zero pressure so a
            // subsequent GetLatestTabletPressure() call (if made before the
            // next press) doesn't report a stale nonzero value.
            PushSample(true);
            latestPressure = 0.0f;
            break;
        }
        case SDL_EVENT_PEN_MOTION:
        {
            latestPenX = event->pmotion.x;
            latestPenY = event->pmotion.y;
            if (penActive)
                PushSample(true);
            break;
        }
        case SDL_EVENT_PEN_AXIS:
        {
            const SDL_PenAxisEvent& axisEvent = event->paxis;
            switch (axisEvent.axis)
            {
                case SDL_PEN_AXIS_PRESSURE:
                    latestPressure = axisEvent.value;
                    // Pressure can change independently of position (SDL fires
                    // separate axis events), so record a sample here too --
                    // otherwise a pressure ramp mid-stroke with no position
                    // change in between gets skipped entirely.
                    if (penActive)
                        PushSample(true);
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

    // We no longer rely on SDL's pen->mouse synthesis for anything -- position,
    // press/release, and pressure are all read directly from real pen events
    // now. Synthetic mouse-from-pen events are unreliable on Windows (WinTab
    // pen motion doesn't consistently generate SDL_EVENT_MOUSE_MOTION even
    // with this hint on), so turning it off avoids relying on that entirely.
    SDL_SetHint(SDL_HINT_PEN_MOUSE_EVENTS, "0");
    SDL_SetHint(SDL_HINT_PEN_TOUCH_EVENTS, "0");

    if (!SDL_AddEventWatch(WatchSDLEvent, nullptr)) {
        printf("Failed to add event watch: %s\n", SDL_GetError());
    } else {
        printf("SDL event watcher installed\n");
    }

    initialized = true;
    return true;
}

void PumpSDLTabletInput()
{
    // Intentionally does nothing now. SDL_AddEventWatch's callback fires
    // automatically whenever SDL processes an event (during SDL_PumpEvents,
    // which SDL_PollEvent calls internally) -- regardless of who actually
    // calls SDL_PollEvent. raylib's own PLATFORM_DESKTOP_SDL backend already
    // polls events every frame, which is enough to keep WatchSDLEvent firing.
    //
    // Kept as a no-op (rather than deleted) so existing call sites like
    // Canvas::handle_pen_events() don't need to change.
}

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
    pendingSamples.clear();
}

bool GetLatestTabletPressure(float* pressure)
{
    if (pressure == nullptr || !penActive)
        return false;

    *pressure = latestPressure;
    return true;
}

bool IsTabletPenInRange()
{
    return penInRange;
}

bool IsTabletPenDown()
{
    return penActive;
}

// Real pen coordinates, relative to the window client area -- same coordinate
// space as raylib's GetMousePosition(). Only meaningful while the pen is in
// range (hovering or touching); returns false otherwise so callers know to
// fall back to the physical mouse.
bool GetLatestTabletPosition(float* x, float* y)
{
    if (x == nullptr || y == nullptr || !penInRange)
        return false;

    *x = latestPenX;
    *y = latestPenY;
    return true;
}

// Call once per frame, after PumpSDLTabletInput(). Returns true if the pen
// touched down at any point since the last call, then clears the flag.
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

bool GetLatestTabletTilt(float* tiltX, float* tiltY)
{
    if (tiltX == nullptr || tiltY == nullptr || !penInRange)
        return false;

    *tiltX = latestTiltX;
    *tiltY = latestTiltY;
    return true;
}

// Returns every pen sample (position + pressure) recorded since the last call,
// in chronological order, then clears the buffer. Draw one line segment per
// consecutive pair of samples -- not just from the frame-start position to the
// frame-end position -- to avoid faceted/chunky strokes on fast movement.
std::vector<PenSample> ConsumeTabletPenSamples()
{
    std::vector<PenSample> samples;
    samples.swap(pendingSamples);
    return samples;
}
