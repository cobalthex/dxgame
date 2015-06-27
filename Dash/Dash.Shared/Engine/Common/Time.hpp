#pragma once

//Basic time constants

#include <chrono>

typedef std::chrono::milliseconds TimeType;
typedef std::chrono::milliseconds TimeSpan;
typedef std::chrono::time_point<std::chrono::high_resolution_clock> TimePoint; //Standard hires time point. Should be used everywhere
typedef std::chrono::high_resolution_clock Timer; //standard hires timer