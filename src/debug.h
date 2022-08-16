#pragma once

inline bool isDebugOutputEnabled = true;

#define dout                                                                   \
    if (isDebugOutputEnabled)                                                  \
    std::cout
