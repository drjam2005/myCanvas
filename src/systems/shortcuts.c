#include "../../include/systems/shortcut.h"
#include "../../include/systems/core.h"
#include <stdlib.h>
#include <string.h>

bool key_shortcut_register(ShortcutManager* manager, void (*func)(Core*), size_t key_count, const KeyboardKey* keys) {
	KeyShortcut newShortcut = {0};
	newShortcut.func = func;
	newShortcut.key_count = key_count;
	if (!func || !keys || key_count == 0 ||
	    key_count > sizeof(newShortcut.keys) / sizeof(newShortcut.keys[0]))
		return false;
	memcpy(newShortcut.keys, keys, sizeof(newShortcut.keys[0]) * key_count);

	KeyShortcut* shortcuts = realloc(manager->key_shortcuts,
	                                  (manager->keyshortcut_count + 1) * sizeof(*shortcuts));
	if (!shortcuts)
		return false;

	manager->key_shortcuts = shortcuts;
	manager->key_shortcuts[manager->keyshortcut_count++] = newShortcut;
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

        PEN_STATE state = core->state.pen_state;
        if (shortcut->pen_state == PEN_NULL ? state != PEN_NULL : !(shortcut->pen_state & state))
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
	free(manager->key_shortcuts);
	manager->key_shortcuts = NULL;
	manager->keyshortcut_count = 0;
}

void pen_shortcut_free(ShortcutManager* manager) {
	free(manager->pen_shortcuts);
	manager->pen_shortcuts = NULL;
	manager->penshortcut_count = 0;
}

