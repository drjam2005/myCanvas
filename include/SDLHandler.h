#pragma once
#ifndef SDLHANDLER_H
#define SDLHANDLER_H

#include <SDL3/SDL.h>

#include <deque>

bool SDLCALL WatchSDLEvent(void* userdata, SDL_Event* event);
std::deque<SDL_Event> DrainPenEvents();

#endif
