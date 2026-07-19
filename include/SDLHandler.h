#pragma once
#ifndef SDLHANDLER_H
#define SDLHANDLER_H

#include <SDL3/SDL.h>

bool InitSDLTabletInput();
void PumpSDLTabletInput();
void ShutdownSDLTabletInput();

bool GetLatestTabletPressure(float* pressure);
bool GetLatestTabletTilt(float* tiltX, float* tiltY);

bool IsTabletPenInRange();
bool IsTabletPenDown();

bool ConsumeTabletPenPressed();
bool ConsumeTabletPenReleased();

bool SDLCALL WatchSDLEvent(void* userdata, SDL_Event* event);

#endif
