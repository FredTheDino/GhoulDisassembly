#pragma once

typedef enum {
    XINPUT,
    YINPUT,
    SHOOT_RIGHT,
    SHOOT_LEFT,
    RELOAD,
    NUM_BINDINGS
} Binding;
Name bindings[NUM_BINDINGS];

#define NAME(binding) bindings[binding]

#define WIN_WIDTH 800
#define WIN_HEIGHT 800
