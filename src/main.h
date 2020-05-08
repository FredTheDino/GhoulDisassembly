#pragma once

typedef enum {
    UP,
    DOWN,
    LEFT,
    RIGHT,
    NUM_BINDINGS
} Binding;
Name bindings[NUM_BINDINGS];

#define NAME(binding) bindings[binding]

#define WIN_WIDTH 800
#define WIN_HEIGHT 800
