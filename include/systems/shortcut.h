#ifndef SHORTCUT_H
#define SHORTCUT_H

#if defined(_WIN32)
#include "../../vendor/raylib/windows/include/raylib.h"
#elif defined(__unix__) || defined(__APPLE__)
#include "../../vendor/raylib/unix/include/raylib.h"
#endif
#include "./pen_state.h"
#include <stddef.h>

typedef struct Core Core;

typedef struct {
    void (*func)(Core*);

    KeyboardKey keys[8];
    size_t key_count;
} KeyShortcut;

typedef struct {
    void (*func)(Core*);

    KeyboardKey keys[7];
	PEN_STATE pen_state;
    size_t key_count;
} PenShortcut;

typedef struct {
    KeyShortcut* key_shortcuts;
    size_t keyshortcut_count;

    PenShortcut* pen_shortcuts;
    size_t penshortcut_count;
} ShortcutManager;

bool key_shortcut_register(ShortcutManager* manager, void (*func)(Core*), size_t key_count, const KeyboardKey* keys);
bool pen_shortcut_register(ShortcutManager* manager, void (*func)(Core*), PEN_STATE state, size_t key_count, const KeyboardKey* keys);

void key_shortcut_update(ShortcutManager* manager, Core* core);
void key_shortcut_free(ShortcutManager* manager);
void pen_shortcut_update(ShortcutManager* manager, Core* core);
void pen_shortcut_free(ShortcutManager* manager);

#define KEY_SHORTCUT(manager, func, ...) \
    key_shortcut_register( \
        manager, func, \
        sizeof((KeyboardKey[]){__VA_ARGS__}) / sizeof(KeyboardKey), \
        (KeyboardKey[]){__VA_ARGS__} \
    )

#define PEN_SHORTCUT(manager, func, state, ...) \
    pen_shortcut_register( \
        manager, func, state, \
        sizeof((KeyboardKey[]){__VA_ARGS__}) / sizeof(KeyboardKey), \
        (KeyboardKey[]){__VA_ARGS__} \
    )

#endif
