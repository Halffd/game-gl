#include "../include_libs/tinyexpr/tinyexpr.h"

// This global variable will force the linker to include tinyexpr functions
// even if they are not explicitly called in this file.
// It's a hack to ensure tinyexpr.c gets linked.
volatile int tinyexpr_force_link = 0;
