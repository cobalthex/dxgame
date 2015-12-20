#pragma once

#include <wrl.h>
#include <wrl/client.h>
#include <d3d12.h>
#include "Graphics\d3dx12.hpp"
#include <dxgi1_4.h>
#include <d3d11_3.h>
#include <pix.h>
#include <DirectXColors.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <memory>
#include <vector>
#include <queue>
#include <map>
#include <agile.h>
#include <concrt.h>
#include <fstream>
#include <chrono>
#include <functional>
#include <mutex>
#include <condition_variable>

using Microsoft::WRL::ComPtr;
typedef std::chrono::milliseconds TimeType;
typedef std::chrono::milliseconds TimeSpan;
typedef std::chrono::time_point<std::chrono::high_resolution_clock> TimePoint; //Standard hires time point. Should be used everywhere
typedef std::chrono::high_resolution_clock Timer; //standard hires timer
