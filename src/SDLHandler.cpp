#include "SDLHandler.h"

#include <mutex>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cstring>
#include <cctype>
#include <string>

#if defined(__linux__)
#include <X11/Xlib.h>
#include <X11/extensions/XInput2.h>
#endif

namespace {
	std::mutex penEventsMutex;
	std::deque<SDL_Event> penEvents;
	bool sdlInitializedHere = false;
	float latestTabletPressure = 1.0f;
	bool hasLatestTabletPressure = false;

#if defined(__linux__)
	struct PressureAxis {
		int deviceId;
		int axisNumber;
		double min;
		double max;
	};

	Display* xinputDisplay = nullptr;
	int xinputOpcode = -1;
	std::vector<PressureAxis> pressureAxes;

	bool contains_case_insensitive(const char* text, const char* needle)
	{
		if (text == nullptr || needle == nullptr) return false;

		std::string haystack(text);
		std::string target(needle);
		std::transform(haystack.begin(), haystack.end(), haystack.begin(), [](unsigned char c) {
			return (char)std::tolower(c);
		});
		std::transform(target.begin(), target.end(), target.begin(), [](unsigned char c) {
			return (char)std::tolower(c);
		});

		return haystack.find(target) != std::string::npos;
	}

	const PressureAxis* find_pressure_axis(int deviceId)
	{
		for (const PressureAxis& axis : pressureAxes) {
			if (axis.deviceId == deviceId) return &axis;
		}
		return nullptr;
	}

	bool valuator_is_set(const unsigned char* mask, int axisNumber)
	{
		return (mask[axisNumber / 8] & (1 << (axisNumber % 8))) != 0;
	}

	int valuator_value_index(const unsigned char* mask, int axisNumber)
	{
		int index = 0;
		for (int i = 0; i < axisNumber; ++i) {
			if (valuator_is_set(mask, i)) ++index;
		}
		return index;
	}

	void InitXInputPressure()
	{
		xinputDisplay = XOpenDisplay(nullptr);
		if (xinputDisplay == nullptr) return;

		int event = 0;
		int error = 0;
		if (!XQueryExtension(xinputDisplay, "XInputExtension", &xinputOpcode, &event, &error)) {
			XCloseDisplay(xinputDisplay);
			xinputDisplay = nullptr;
			return;
		}

		int major = 2;
		int minor = 0;
		if (XIQueryVersion(xinputDisplay, &major, &minor) != Success) {
			XCloseDisplay(xinputDisplay);
			xinputDisplay = nullptr;
			return;
		}

		int deviceCount = 0;
		XIDeviceInfo* devices = XIQueryDevice(xinputDisplay, XIAllDevices, &deviceCount);
		for (int i = 0; i < deviceCount; ++i) {
			XIDeviceInfo& device = devices[i];
			for (int c = 0; c < device.num_classes; ++c) {
				XIAnyClassInfo* klass = device.classes[c];
				if (klass->type != XIValuatorClass) continue;

				XIValuatorClassInfo* valuator = (XIValuatorClassInfo*)klass;
				char* label = XGetAtomName(xinputDisplay, valuator->label);
				bool isPressure = contains_case_insensitive(label, "pressure");
				if (label != nullptr) XFree(label);

				if (isPressure && valuator->max > valuator->min) {
					pressureAxes.push_back({
						device.deviceid,
						valuator->number,
						valuator->min,
						valuator->max
					});
				}
			}
		}
		if (devices != nullptr) XIFreeDeviceInfo(devices);

		if (pressureAxes.empty()) {
			XCloseDisplay(xinputDisplay);
			xinputDisplay = nullptr;
			return;
		}

		unsigned char mask[(XI_LASTEVENT + 7) / 8] = {};
		XISetMask(mask, XI_RawMotion);
		XISetMask(mask, XI_RawButtonPress);
		XISetMask(mask, XI_RawButtonRelease);

		XIEventMask eventMask = {};
		eventMask.deviceid = XIAllDevices;
		eventMask.mask_len = sizeof(mask);
		eventMask.mask = mask;

		XISelectEvents(xinputDisplay, DefaultRootWindow(xinputDisplay), &eventMask, 1);
		XFlush(xinputDisplay);
	}

	void PumpXInputPressure()
	{
		if (xinputDisplay == nullptr) return;

		while (XPending(xinputDisplay) > 0) {
			XEvent event;
			XNextEvent(xinputDisplay, &event);

			if (event.xcookie.type != GenericEvent || event.xcookie.extension != xinputOpcode) continue;
			if (!XGetEventData(xinputDisplay, &event.xcookie)) continue;

			if (event.xcookie.evtype == XI_RawMotion || event.xcookie.evtype == XI_RawButtonPress) {
				XIRawEvent* raw = (XIRawEvent*)event.xcookie.data;
				const PressureAxis* axis = find_pressure_axis(raw->deviceid);

				if (axis != nullptr && raw->valuators.mask != nullptr &&
					axis->axisNumber < raw->valuators.mask_len * 8 &&
					valuator_is_set(raw->valuators.mask, axis->axisNumber)) {
					int valueIndex = valuator_value_index(raw->valuators.mask, axis->axisNumber);
					double* values = raw->raw_values != nullptr ? raw->raw_values : raw->valuators.values;
					if (values != nullptr) {
						double normalized = (values[valueIndex] - axis->min) / (axis->max - axis->min);
						latestTabletPressure = (float)std::clamp(normalized, 0.0, 1.0);
						hasLatestTabletPressure = true;
					}
				}
			} else if (event.xcookie.evtype == XI_RawButtonRelease) {
				latestTabletPressure = 1.0f;
				hasLatestTabletPressure = false;
			}

			XFreeEventData(xinputDisplay, &event.xcookie);
		}
	}

	void ShutdownXInputPressure()
	{
		pressureAxes.clear();
		if (xinputDisplay != nullptr) {
			XCloseDisplay(xinputDisplay);
			xinputDisplay = nullptr;
		}
	}
#endif
}

bool InitSDLTabletInput(void* nativeWindowHandle)
{
    (void)nativeWindowHandle;
    SDL_SetHint(SDL_HINT_PEN_MOUSE_EVENTS, "1");
    SDL_SetHint(SDL_HINT_PEN_TOUCH_EVENTS, "0");
    SDL_SetHint(SDL_HINT_VIDEO_X11_EXTERNAL_WINDOW_INPUT, "1");

    bool initialized = false;

    if ((SDL_WasInit(SDL_INIT_VIDEO) & SDL_INIT_VIDEO) == 0) {
        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
            std::cout << "SDL_Init failed: " << SDL_GetError() << '\n';
        } else {
            sdlInitializedHere = true;
            initialized = true;
        }
    } else {
        initialized = true;
    }

    if (initialized && !SDL_AddEventWatch(WatchSDLEvent, nullptr)) {
        std::cout << "SDL_AddEventWatch failed: " << SDL_GetError() << '\n';
        initialized = false;
    }

#if defined(__linux__)
    InitXInputPressure();
    initialized = initialized || xinputDisplay != nullptr;
#endif

    return initialized;
}

void PumpSDLTabletInput()
{
    if ((SDL_WasInit(SDL_INIT_VIDEO) & SDL_INIT_VIDEO) != 0) {
        SDL_PumpEvents();
    }

#if defined(__linux__)
    PumpXInputPressure();
#endif
}

void ShutdownSDLTabletInput()
{
#if defined(__linux__)
    ShutdownXInputPressure();
#endif

    SDL_RemoveEventWatch(WatchSDLEvent, nullptr);

    if (sdlInitializedHere) {
        SDL_QuitSubSystem(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
        sdlInitializedHere = false;
    }
}

bool GetLatestTabletPressure(float* pressure)
{
    if (!hasLatestTabletPressure || pressure == nullptr) return false;
    *pressure = latestTabletPressure;
    return true;
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
