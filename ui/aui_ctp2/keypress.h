











#pragma once
#ifndef KEYPRESS_H
#define KEYPRESS_H 1

 
class KeyboardHandler;
enum KEY_FUNCTION;

extern void init_keymap();
extern void init_defaultKeymap();
extern void cleanup_keymap();
extern sint32 ui_HandleKeypress(WPARAM wParam, LPARAM lParam);

void keypress_RegisterHandler(KeyboardHandler *handler);
void keypress_RemoveHandler(KeyboardHandler *handler);

bool keypress_IsGameFunction(KEY_FUNCTION kf);
#endif 
