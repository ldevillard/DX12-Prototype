#pragma once

#include <DirectXMath.h>

using Float3 = DirectX::XMFLOAT3;
using Matrix4 = DirectX::XMMATRIX;
using Vector = DirectX::XMVECTOR;

inline void VecToFloat(Vector vec, FLOAT* outArray)
{
    DirectX::XMFLOAT4 float4;
    DirectX::XMStoreFloat4(&float4, vec);

    outArray[0] = float4.x;
    outArray[1] = float4.y;
    outArray[2] = float4.z;
    outArray[3] = float4.w;
}