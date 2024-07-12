#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN // exclude rarely-used stuff from Windows headers.
#endif

#include <windows.h>

// undefine min max to avoid conflicts with std
#if defined(min)
#undef min
#endif

#if defined(max)
#undef max
#endif

#include <d3d12.h>
#include <d3dx12.h>
#include <dxgi1_6.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>

#include <string>
#include <wrl.h>
#include <shellapi.h>
