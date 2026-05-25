#pragma once
#ifndef SDLHANDLER_H
#define SDLHANDLER_H

#include <SDL3/SDL.h>

#include <deque>

bool InitSDLTabletInput(void* nativeWindowHandle);
void PumpSDLTabletInput();
void ShutdownSDLTabletInput();
bool GetLatestTabletPressure(float* pressure);
bool SDLCALL WatchSDLEvent(void* userdata, SDL_Event* event);
std::deque<SDL_Event> DrainPenEvents();

#endif
