#include "PrecompiledHeaders.h"
#include "rendering/Sample.h"

#include <chrono>
#include <DirectXMath.h>

#include "resources/Resource.h"

using namespace DirectX;

// vertex data for a colored cube.
struct VertexPosColor
{
    XMFLOAT3 Position;
    XMFLOAT3 Color;
};

static VertexPosColor g_Vertices[8] = {
    { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f) }, // 0
    { XMFLOAT3(-1.0f,  1.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) }, // 1
    { XMFLOAT3(1.0f,  1.0f, -1.0f), XMFLOAT3(1.0f, 1.0f, 0.0f) }, // 2
    { XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) }, // 3
    { XMFLOAT3(-1.0f, -1.0f,  1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) }, // 4
    { XMFLOAT3(-1.0f,  1.0f,  1.0f), XMFLOAT3(0.0f, 1.0f, 1.0f) }, // 5
    { XMFLOAT3(1.0f,  1.0f,  1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f) }, // 6
    { XMFLOAT3(1.0f, -1.0f,  1.0f), XMFLOAT3(1.0f, 0.0f, 1.0f) }  // 7
};

static WORD g_Indicies[36] =
{
    0, 1, 2, 0, 2, 3,
    4, 6, 5, 4, 7, 6,
    4, 5, 1, 4, 1, 0,
    3, 2, 6, 3, 6, 7,
    1, 5, 6, 1, 6, 2,
    4, 0, 3, 4, 3, 7
};

#pragma region Public Methods

Sample::Sample(uint32_t w, uint32_t h)
    : width(w)
    , height(h)
    , timer()
    , scissorRect(CD3DX12_RECT(0, 0, LONG_MAX, LONG_MAX))
    , viewport(CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height)))
{
    parseCommandLineArguments();
    enableDebugLayer();
	allowTearing = checkTearingSupport();
}

void Sample::OnInit(HWND hWnd)
{
    handleWin = hWnd;
	loadPipeline();
    loadAssets();
}

void Sample::OnUpdate()
{
    timer.OnUpdate();

    std::string fps = std::to_string(timer.GetFrameRate()) + "\n";
    OutputDebugStringA(fps.c_str());

    float angle = static_cast<float>(45.0);
    const XMVECTOR rotationAxis = XMVectorSet(0, 1, 1, 0);
    modelMatrix = XMMatrixRotationAxis(rotationAxis, XMConvertToRadians(angle));

    // update the view matrix.
    const XMVECTOR eyePosition = XMVectorSet(0, 0, -10, 1);
    const XMVECTOR focusPoint = XMVectorSet(0, 0, 0, 1);
    const XMVECTOR upDirection = XMVectorSet(0, 1, 0, 0);
    viewMatrix = XMMatrixLookAtLH(eyePosition, focusPoint, upDirection);

    // update the projection matrix.
    float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    projectionMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(fov), aspectRatio, 0.1f, 100.0f);
}

void Sample::OnRender()
{
    //UINT currentBackBufferIndex = swapChain->GetCurrentBackBufferIndex();
    //std::unique_ptr<CommandAllocator>& commandAllocator = commandAllocators[currentBackBufferIndex];
    //Resource backBuffer = swapChain->GetCurrentBackBuffer();
    //
    //commandList->Populate(*RTVdescriptorHeap, *DSVdescriptorHeap, *commandAllocator, backBuffer, currentBackBufferIndex);
    //
    //ID3D12CommandList* const commandLists[] = 
    //{
    //    commandList->GetPtr()
    //};
    //commandQueue->Get()->ExecuteCommandLists(_countof(commandLists), commandLists);
    //
    //frameFenceValues[currentBackBufferIndex] = fence->Signal(*commandQueue);
    //
    //UINT syncInterval = vSync ? 1 : 0;
    //UINT presentFlags = allowTearing && !vSync ? DXGI_PRESENT_ALLOW_TEARING : 0;
    //ThrowIfFailed(swapChain->Get()->Present(syncInterval, presentFlags));
    //
    //currentBackBufferIndex = swapChain->GetCurrentBackBufferIndex();
    //
    //fence->WaitForFenceValue(frameFenceValues[currentBackBufferIndex]);

    UINT currentBackBufferIndex = swapChain->GetCurrentBackBufferIndex();
    Resource backBuffer = swapChain->GetCurrentBackBuffer();
    auto rtv = RTVdescriptorHeap->GetRenderTargetView(currentBackBufferIndex);
    auto dsv = DSVdescriptorHeap->GetCPUDescriptorHandleForHeapStart();

    commandAllocators[currentBackBufferIndex]->Reset();
    commandList->Get()->Reset(commandAllocators[currentBackBufferIndex]->GetPtr(), nullptr);

    // Clear the render targets.
    {
        commandList->TransitionResource(backBuffer, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

        FLOAT clearColor[] = { 0.4f, 0.6f, 0.9f, 1.0f };

        commandList->ClearRTV(rtv, clearColor);
        commandList->ClearDepth(dsv);
    }

    commandList->Get()->SetPipelineState(m_PipelineState.Get());
    commandList->Get()->SetGraphicsRootSignature(m_RootSignature.Get());

    commandList->Get()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    commandList->Get()->IASetVertexBuffers(0, 1, &vertexBufferView);
    commandList->Get()->IASetIndexBuffer(&indexBufferView);

    commandList->Get()->RSSetViewports(1, &viewport);
    commandList->Get()->RSSetScissorRects(1, &scissorRect);

    commandList->Get()->OMSetRenderTargets(1, &rtv, FALSE, &dsv);

    // Update the MVP matrix
    XMMATRIX mvpMatrix = XMMatrixMultiply(modelMatrix, viewMatrix);
    mvpMatrix = XMMatrixMultiply(mvpMatrix, projectionMatrix);
    commandList->Get()->SetGraphicsRoot32BitConstants(0, sizeof(XMMATRIX) / 4, &mvpMatrix, 0);

    commandList->Get()->DrawIndexedInstanced(_countof(g_Indicies), 1, 0, 0, 0);

    // Present
    {
        commandList->TransitionResource(backBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
        ThrowIfFailed(commandList->Get()->Close());

        ID3D12CommandList* const commandLists[] = 
        {
            commandList->GetPtr()
        };
        commandQueue->Get()->ExecuteCommandLists(_countof(commandLists), commandLists);
        
        frameFenceValues[currentBackBufferIndex] = fence->Signal(*commandQueue);
        
        UINT syncInterval = vSync ? 1 : 0;
        UINT presentFlags = allowTearing && !vSync ? DXGI_PRESENT_ALLOW_TEARING : 0;
        ThrowIfFailed(swapChain->Get()->Present(syncInterval, presentFlags));
        
        currentBackBufferIndex = swapChain->GetCurrentBackBufferIndex();
        
        fence->WaitForFenceValue(frameFenceValues[currentBackBufferIndex]);
    }
}

void Sample::OnDestroy()
{
    fence->Flush(*commandQueue);
    CloseHandle(fence->GetEvent());
}

void Sample::ToggleVSync()
{
    vSync = !vSync;
}

void Sample::Resize(uint32_t width, uint32_t height)
{
    if (this->width != width || this->height != height)
    {
        //dDon't allow 0 size swap chain back buffers.
        this->width = std::max(1u, width);
        this->height = std::max(1u, height);

        // flush the GPU queue to make sure the swap chain's back buffers
        // are not being referenced by an in-flight command list.
        fence->Flush(*commandQueue);

        for (int i = 0; i < SwapChain::FrameCount; ++i)
        {
            // Any references to the back buffers must be released
            // before the swap chain can be resized.
            swapChain->ResetBackBuffer(i);
            frameFenceValues[i] = frameFenceValues[swapChain->GetCurrentBackBufferIndex()];
        }

        swapChain->Resize(this->width, this->height);
        swapChain->UpdateRenderTargetViews(*device, *RTVdescriptorHeap);
    }
}

#pragma endregion

#pragma region Private Methods

bool Sample::checkTearingSupport()
{
    BOOL allowTearing = FALSE;

    // rather than create the DXGI 1.5 factory interface directly, we create the
    // DXGI 1.4 interface and query for the 1.5 interface. This is to enable the 
    // graphics debugging tools which will not support the 1.5 factory interface 
    // until a future update.
    ComPtr<IDXGIFactory4> factory4;
    if (SUCCEEDED(CreateDXGIFactory1(IID_PPV_ARGS(&factory4))))
    {
        ComPtr<IDXGIFactory5> factory5;
        if (SUCCEEDED(factory4.As(&factory5)))
        {
            if (FAILED(factory5->CheckFeatureSupport(
                DXGI_FEATURE_PRESENT_ALLOW_TEARING,
                &allowTearing, sizeof(allowTearing))))
            {
                allowTearing = FALSE;
            }
        }
    }

    return allowTearing == TRUE;
}

void Sample::loadPipeline()
{
	device = std::make_unique<Device>(useWarp);
	commandQueue = std::make_unique<CommandQueue>(*device);
	swapChain = std::make_unique<SwapChain>(handleWin, *commandQueue, width, height, allowTearing);
    RTVdescriptorHeap = std::make_unique<DescriptorHeap>(*device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, SwapChain::FrameCount);

    swapChain->UpdateRenderTargetViews(*device, *RTVdescriptorHeap);

    for (int i = 0; i < SwapChain::FrameCount; ++i)
        commandAllocators[i] = std::make_unique<CommandAllocator>(*device, D3D12_COMMAND_LIST_TYPE_DIRECT);

    commandList = std::make_unique<CommandList>(*device, *commandAllocators[swapChain->GetCurrentBackBufferIndex()], D3D12_COMMAND_LIST_TYPE_DIRECT);
    fence = std::make_unique<Fence>(*device);
}

void Sample::loadAssets()
{
    // upload vertex buffer data.
    Resource intermediateVertexBuffer;
    updateBufferResource(vertexBuffer, intermediateVertexBuffer, _countof(g_Vertices), sizeof(VertexPosColor), g_Vertices);
    // create the vertex buffer view.
    vertexBufferView.BufferLocation = vertexBuffer.Get()->GetGPUVirtualAddress();
    vertexBufferView.SizeInBytes = sizeof(g_Vertices);
    vertexBufferView.StrideInBytes = sizeof(VertexPosColor);

    // upload index buffer data.
    Resource intermediateIndexBuffer;
    updateBufferResource(indexBuffer, intermediateIndexBuffer, _countof(g_Indicies), sizeof(WORD), g_Indicies);
    // Create index buffer view.
    indexBufferView.BufferLocation = indexBuffer.Get()->GetGPUVirtualAddress();
    indexBufferView.Format = DXGI_FORMAT_R16_UINT;
    indexBufferView.SizeInBytes = sizeof(g_Indicies);

    // create the descriptor heap for the depth-stencil view.
    DSVdescriptorHeap = std::make_unique<DescriptorHeap>(*device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1);

    // load the vertex shader.
    ComPtr<ID3DBlob> vertexShaderBlob;
    ThrowIfFailed(D3DReadFileToBlob(L"..\\x64\\Debug\\VertexShader.cso", &vertexShaderBlob));

    // load the pixel shader.
    ComPtr<ID3DBlob> pixelShaderBlob;
    ThrowIfFailed(D3DReadFileToBlob(L"..\\x64\\Debug\\PixelShader.cso", &pixelShaderBlob));

    // create the vertex input layout
    D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    };

    // create a root signature.
    D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
    featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
    if (FAILED(device->Get()->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
    {
        featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
    }

    // allow input layout and deny unnecessary access to certain pipeline stages.
    D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

    // a single 32-bit constant root parameter that is used by the vertex shader.
    CD3DX12_ROOT_PARAMETER1 rootParameters[1];
    rootParameters[0].InitAsConstants(sizeof(XMMATRIX) / 4, 0, 0, D3D12_SHADER_VISIBILITY_VERTEX);

    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription;
    rootSignatureDescription.Init_1_1(_countof(rootParameters), rootParameters, 0, nullptr, rootSignatureFlags);

    // serialize the root signature.
    ComPtr<ID3DBlob> rootSignatureBlob;
    ComPtr<ID3DBlob> errorBlob;
    ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&rootSignatureDescription,
        featureData.HighestVersion, &rootSignatureBlob, &errorBlob));
    // create the root signature.
    ThrowIfFailed(device->Get()->CreateRootSignature(0, rootSignatureBlob->GetBufferPointer(),
        rootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&m_RootSignature)));

    struct PipelineStateStream
    {
        CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE pRootSignature;
        CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT InputLayout;
        CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PrimitiveTopologyType;
        CD3DX12_PIPELINE_STATE_STREAM_VS VS;
        CD3DX12_PIPELINE_STATE_STREAM_PS PS;
        CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT DSVFormat;
        CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
    } pipelineStateStream;

    D3D12_RT_FORMAT_ARRAY rtvFormats = {};
    rtvFormats.NumRenderTargets = 1;
    rtvFormats.RTFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

    pipelineStateStream.pRootSignature = m_RootSignature.Get();
    pipelineStateStream.InputLayout = { inputLayout, _countof(inputLayout) };
    pipelineStateStream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pipelineStateStream.VS = CD3DX12_SHADER_BYTECODE(vertexShaderBlob.Get());
    pipelineStateStream.PS = CD3DX12_SHADER_BYTECODE(pixelShaderBlob.Get());
    pipelineStateStream.DSVFormat = DXGI_FORMAT_D32_FLOAT;
    pipelineStateStream.RTVFormats = rtvFormats;

    D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = {
        sizeof(PipelineStateStream), &pipelineStateStream
    };
    ThrowIfFailed(device->Get()->CreatePipelineState(&pipelineStateStreamDesc, IID_PPV_ARGS(&m_PipelineState)));

    // execute command list
    {
        ThrowIfFailed(commandList->Get()->Close());
    
        //ID3D12CommandAllocator* commandAlloc;
        //UINT dataSize = sizeof(commandAlloc);
        //ThrowIfFailed(commandList->Get()->GetPrivateData(__uuidof(ID3D12CommandAllocator), &dataSize, &commandAlloc));
    
        ID3D12CommandList* const commandLists[] = 
        {
            commandList->GetPtr()
        };
        
        commandQueue->Get()->ExecuteCommandLists(_countof(commandLists), commandLists);
        uint64_t fenceValue = fence->Signal(*commandQueue);
        //ThrowIfFailed(commandAlloc->Release());
        fence->WaitForFenceValue(fenceValue);
    }

    // resize/Create the depth buffer.
    resizeDepthBuffer(width, height);
}

void Sample::parseCommandLineArguments()
{
    int argc;
    wchar_t** argv = ::CommandLineToArgvW(::GetCommandLineW(), &argc);
    for (int i = 1; i < argc; ++i)
    {
        if (_wcsnicmp(argv[i], L"-warp", wcslen(argv[i])) == 0 ||
            _wcsnicmp(argv[i], L"/warp", wcslen(argv[i])) == 0)
        {
            useWarp = true;
        }
    }
    ::LocalFree(argv);
}

void Sample::enableDebugLayer()
{
#if defined(_DEBUG)
    // always enable the debug layer before doing anything DX12 related
    // so all possible errors generated while creating DX12 objects
    // are caught by the debug layer.
    ComPtr<ID3D12Debug> debugInterface;
    ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface)));
    debugInterface->EnableDebugLayer();
#endif
}

void Sample::updateBufferResource(Resource& destinationResource, Resource& intermediateResource,
                                  size_t numElements, size_t elementSize, const void* bufferData,
                                  D3D12_RESOURCE_FLAGS flags)
{
    size_t bufferSize = numElements * elementSize;

    ComPtr<ID3D12Resource> pDestinationResource = destinationResource.Get();
    ComPtr<ID3D12Resource> pIntermediateResource = intermediateResource.Get();

    // create a committed resource for the GPU resource in a default heap.
    CD3DX12_HEAP_PROPERTIES defaultHeapProperties(D3D12_HEAP_TYPE_DEFAULT);
    CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize, flags);
    
    ThrowIfFailed(device->Get()->CreateCommittedResource(
        &defaultHeapProperties,
        D3D12_HEAP_FLAG_NONE,
        &bufferDesc,
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_PPV_ARGS(&pDestinationResource)));

    destinationResource.Set(pDestinationResource);

    // create a committed resource for the upload.
    if (bufferData)
    {
        CD3DX12_HEAP_PROPERTIES uploadHeapProperties(D3D12_HEAP_TYPE_UPLOAD);
        CD3DX12_RESOURCE_DESC uploadBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

        ThrowIfFailed(device->Get()->CreateCommittedResource(
            &uploadHeapProperties,
            D3D12_HEAP_FLAG_NONE,
            &uploadBufferDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&pIntermediateResource)));

        intermediateResource.Set(pIntermediateResource);

        D3D12_SUBRESOURCE_DATA subresourceData = {};
        subresourceData.pData = bufferData;
        subresourceData.RowPitch = bufferSize;
        subresourceData.SlicePitch = subresourceData.RowPitch;

        UpdateSubresources(commandList->GetPtr(), destinationResource.GetPtr(), intermediateResource.GetPtr(), 0, 0, 1, &subresourceData);
    }
}

void Sample::resizeDepthBuffer(int width, int height)
{
    // Flush any GPU commands that might be referencing the depth buffer.
    fence->Flush(*commandQueue);

    width = std::max(1, width);
    height = std::max(1, height);

    // Resize screen dependent resources.
    // Create a depth buffer.
    D3D12_CLEAR_VALUE optimizedClearValue = {};
    optimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
    optimizedClearValue.DepthStencil = { 1.0f, 0 };

    ComPtr<ID3D12Resource> pDepthBuffer = depthBuffer.Get();

    CD3DX12_HEAP_PROPERTIES defaultHeapProperties(D3D12_HEAP_TYPE_DEFAULT);
    CD3DX12_RESOURCE_DESC bufferTexDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, width, height,
                                       1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

    ThrowIfFailed(device->Get()->CreateCommittedResource(
        &defaultHeapProperties,
        D3D12_HEAP_FLAG_NONE,
        &bufferTexDesc,
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        &optimizedClearValue,
        IID_PPV_ARGS(&pDepthBuffer)
    ));

    depthBuffer.Set(pDepthBuffer);

    // Update the depth-stencil view.
    D3D12_DEPTH_STENCIL_VIEW_DESC dsv = {};
    dsv.Format = DXGI_FORMAT_D32_FLOAT;
    dsv.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    dsv.Texture2D.MipSlice = 0;
    dsv.Flags = D3D12_DSV_FLAG_NONE;

    device->Get()->CreateDepthStencilView(pDepthBuffer.Get(), &dsv,
        DSVdescriptorHeap->GetCPUDescriptorHandleForHeapStart());
}

#pragma endregion