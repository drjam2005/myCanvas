#ifndef PLUG_H
#define PLUG_H

#include <stdbool.h>

typedef struct {
	void* handle;

	void (*plug_update)(void);
	void (*plug_render)(void);

	void* (*plug_pre_reload)(void);
	bool  (*plug_post_reload)(void*);

	bool (*plug_init)(void);
} Plug;

bool reloadPlug(Plug* plug);

#ifndef RELOAD
#endif
bool  plug_init(void);
void  plug_update(void);
void  plug_render(void);
void* plug_pre_reload(void);
bool  plug_post_reload(void*);
#endif
