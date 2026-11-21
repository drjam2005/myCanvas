#ifndef PLUG_H
#define PLUG_H

#include <stdbool.h>

typedef struct {
	void* handle;

	void (*plug_update)(void);
	void (*plug_render)(void);

	void* (*plug_pre_reload)(void);
	bool  (*plug_post_reload)(void*);

	bool (*plug_init)(int, int);
} Plug;

bool reloadPlug(Plug* plug);

#ifndef RELOAD
#endif
bool  plug_init(int, int);
void  plug_update(void);
void  plug_render(void);
void* plug_pre_reload(void);
bool  plug_post_reload(void*);
#endif
