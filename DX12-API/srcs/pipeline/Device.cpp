#include "PrecompiledHeaders.h"
#include "pipeline/Device.h"

#pragma region Public Methods

Device::Device(bool useWarp)
{
    adapter = getAdapter(useWarp);
    ThrowIfFailed(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_2, IID_PPV_ARGS(&device)));

    // enable debug messages in debug mode.
#if defined(_DEBUG)
    ComPtr<ID3D12InfoQueue> pInfoQueue;
    if (SUCCEEDED(device.As(&pInfoQueue)))
    {
        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);

        // suppress messages based on their severity level
        D3D12_MESSAGE_SEVERITY severities[] =
        {
            D3D12_MESSAGE_SEVERITY_INFO
        };

        // suppress individual messages by their ID
        D3D12_MESSAGE_ID denyIds[] = {
            D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,
            D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,                     
            D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE, 
            D3D12_MESSAGE_ID_CREATERESOURCE_STATE_IGNORED,
        };

        D3D12_INFO_QUEUE_FILTER newFilter = {};
        newFilter.DenyList.NumSeverities = _countof(severities);
        newFilter.DenyList.pSeverityList = severities;
        newFilter.DenyList.NumIDs = _countof(denyIds);
        newFilter.DenyList.pIDList = denyIds;

        ThrowIfFailed(pInfoQueue->PushStorageFilter(&newFilter));
    }
#endif
}

#pragma region Getters

const ComPtr<ID3D12Device2>& Device::Get() const
{
    return device;
}

ID3D12Device2* Device::GetPtr() const
{
    return device.Get();
}

#pragma endregion
#pragma endregion

#pragma region Private Methods

ComPtr<IDXGIAdapter4> Device::getAdapter(bool useWarp)
{
    ComPtr<IDXGIFactory4> dxgiFactory;
    UINT createFactoryFlags = 0;
#if defined(_DEBUG)
    createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif

    ThrowIfFailed(CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory)));

    ComPtr<IDXGIAdapter1> dxgiAdapter1;
    ComPtr<IDXGIAdapter4> dxgiAdapter4;

    if (useWarp)
    {
        ThrowIfFailed(dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&dxgiAdapter1)));
        ThrowIfFailed(dxgiAdapter1.As(&dxgiAdapter4));
    }
    else
    {
        SIZE_T maxDedicatedVideoMemory = 0;
        for (UINT i = 0; dxgiFactory->EnumAdapters1(i, &dxgiAdapter1) != DXGI_ERROR_NOT_FOUND; ++i)
        {
            DXGI_ADAPTER_DESC1 dxgiAdapterDesc1;
            dxgiAdapter1->GetDesc1(&dxgiAdapterDesc1);

            // check to see if the adapter can create a D3D12 device without actually 
            // creating it. The adapter with the largest dedicated video memory
            // is favored.
            if ((dxgiAdapterDesc1.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0 &&
                SUCCEEDED(D3D12CreateDevice(dxgiAdapter1.Get(),
                    D3D_FEATURE_LEVEL_12_2, __uuidof(ID3D12Device), nullptr)) &&
                dxgiAdapterDesc1.DedicatedVideoMemory > maxDedicatedVideoMemory)
            {
                maxDedicatedVideoMemory = dxgiAdapterDesc1.DedicatedVideoMemory;
                ThrowIfFailed(dxgiAdapter1.As(&dxgiAdapter4));
            }
        }
    }

    return dxgiAdapter4;
}

#pragma endregion