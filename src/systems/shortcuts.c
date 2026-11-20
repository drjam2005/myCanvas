#include "../../include/systems/shortcut.h"
#include "../../include/systems/core.h"
#include <stdlib.h>
#include <string.h>

bool key_shortcut_register(ShortcutManager* manager, void (*func)(Core*), size_t key_count, const KeyboardKey* keys) {
	KeyShortcut newShortcut = {0};
	newShortcut.func = func;
	newShortcut.key_count = key_count;
	memcpy(newShortcut.keys, keys, sizeof(KeyboardKey)*key_count);

	manager->key_shortcuts = realloc(manager->key_shortcuts, manager->keyshortcut_count * (sizeof(*manager->key_shortcuts) + 1));
	manager->keyshortcut_count++;
	manager->key_shortcuts[manager->keyshortcut_count-1] = newShortcut;

	return true;
}

bool pen_shortcut_register(ShortcutManager* manager, void (*func)(Core*), PEN_STATE state, size_t key_count, const KeyboardKey* keys) {
	PenShortcut newShortcut = {0};
	newShortcut.func = func;
	newShortcut.key_count = key_count;
	newShortcut.pen_state = state;
	if (!func || key_count > sizeof(newShortcut.keys) / sizeof(newShortcut.keys[0]) ||
	    (key_count > 0 && !keys))
		return false;
	if (key_count > 0)
		memcpy(newShortcut.keys, keys, sizeof(newShortcut.keys[0]) * key_count);

	PenShortcut* shortcuts = realloc(manager->pen_shortcuts,
	                                  (manager->penshortcut_count + 1) * sizeof(*shortcuts));
	if (!shortcuts)
		return false;

	manager->pen_shortcuts = shortcuts;
	manager->pen_shortcuts[manager->penshortcut_count++] = newShortcut;
	return true;
}

void key_shortcut_update(ShortcutManager* manager, Core* core) {
    KeyShortcut* best = NULL;

    for (size_t i = 0; i < manager->keyshortcut_count; ++i) {
        KeyShortcut* shortcut = &manager->key_shortcuts[i];

        size_t key_count = shortcut->key_count;
        if (key_count == 0)
            continue;

        bool active = true;

        for (size_t j = 0; j < key_count - 1; ++j) {
            if (!IsKeyDown(shortcut->keys[j])) {
                active = false;
                break;
            }
        }

        active &= IsKeyPressed(shortcut->keys[key_count - 1]);

        if (active && (!best || key_count > best->key_count))
            best = shortcut;
    }

    if (best)
        best->func(core);
}

void pen_shortcut_update(ShortcutManager* manager, Core* core) {
    PenShortcut* best = NULL;

    for (size_t i = 0; i < manager->penshortcut_count; ++i) {
        PenShortcut* shortcut = &manager->pen_shortcuts[i];

        if (shortcut->pen_state != core->state.pen_state)
            continue;

        bool active = true;
        for (size_t j = 0; j < shortcut->key_count; ++j) {
            if (!IsKeyDown(shortcut->keys[j])) {
                active = false;
                break;
            }
        }

        if (active && (!best || shortcut->key_count > best->key_count))
            best = shortcut;
    }

    if (best)
        best->func(core);
}

void key_shortcut_free(ShortcutManager* manager) {
}

void pen_shortcut_free(ShortcutManager* manager) {
}

