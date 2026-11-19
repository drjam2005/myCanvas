#include "../../include/plug/plug.h"
#include <dlfcn.h>
#ifdef RELOAD
#	include <stdio.h>
#endif
#include <stdbool.h>

#ifdef RELOAD
#define OPN(plug, lib)\
	if(plug->handle) dlclose(plug->handle); \
	plug->handle = dlopen(lib, RTLD_NOW); \
	if(plug->handle == NULL) { \
		fprintf(stderr, "[ERROR] dlopen: %s\n", dlerror()); \
		return false; \
	} 

#define SYM(plug, sym) \
	plug->sym = dlsym(plug->handle, #sym);  \
	if(plug->sym == NULL) { \
		fprintf(stderr, "[ERROR] dlopen: %s\n", dlerror()); \
		return false; \
	}
#else

#define OPN(plug, lib) ((void)(0))
#define SYM(plug, sym) ((plug)->sym = sym)

#endif

bool reloadPlug(Plug* plug) {
	OPN(plug, "./build/libplug.so");
	SYM(plug, plug_init);
	SYM(plug, plug_update);
	SYM(plug, plug_render);
	SYM(plug, plug_pre_reload);
	SYM(plug, plug_post_reload);
	return true;
}
