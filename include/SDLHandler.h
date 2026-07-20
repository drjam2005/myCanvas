#pragma once
#ifndef SDLHANDLER_H
#define SDLHANDLER_H

#include <SDL3/SDL.h>
#include <deque>

bool InitSDLTabletInput();
void PumpSDLTabletInput();
void ShutdownSDLTabletInput();

bool GetLatestTabletPressure(float* pressure);
void GetLatestTabletPosition(float* x, float* y);

bool IsTabletPenDown();
bool IsTabletPenInProximity();

bool ConsumeTabletPenPressed();
bool ConsumeTabletPenReleased();

bool SDLCALL WatchSDLEvent(void* userdata, SDL_Event* event);

#endif
