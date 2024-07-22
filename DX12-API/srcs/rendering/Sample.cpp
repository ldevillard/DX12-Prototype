#include "PrecompiledHeaders.h"
#include "rendering/Sample.h"

#include <chrono>
#include <DirectXMath.h>

#include "helpers/Time.h"
#include "resources/Resource.h"
#include "resources/Shader.h"

using namespace DirectX;

#pragma region Public Methods

Sample::Sample(uint32_t w, uint32_t h)
    : width(w)
    , height(h)
    , scissorRect(CD3DX12_RECT(0, 0, LONG_MAX, LONG_MAX))
    , viewport(CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height)))
{
    parseCommandLineArguments();
    enableDebugLayer();
	allowTearing = checkTearingSupport();
}

#pragma region Getters

CommandList& Sample::GetCommandList()
{
    return *commandList;
}

RootSignature& Sample::GetRootSignature()
{
    return *rootSignature;
}

#pragma endregion

void Sample::OnInit(HWND _hWnd)
{
    Time::OnInit();

    hWnd = _hWnd;
	initPipeline();
}

void Sample::OnUpdate()
{
    Time::OnUpdate();

    std::string fps = std::to_string(Time::GetFrameRate()) + "\n";
    OutputDebugStringA(fps.c_str());

    preRender();
}

void Sample::OnRender()
{

    // ImGui
    ImGui::Render();
    ImDrawData* draw_data = ImGui::GetDrawData();
    ImGui_ImplDX12_RenderDrawData(draw_data, commandList->GetPtr());

    // present to the screen
    {
        UINT currentBackBufferIndex = swapChain->GetCurrentBackBufferIndex();
        Resource backBuffer = swapChain->GetCurrentBackBuffer();

        commandList->TransitionResource(backBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
        commandList->Close();

        ID3D12CommandList* const commandLists[] = { commandList->GetPtr() };
        commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);
        
        frameFenceValues[currentBackBufferIndex] = fence->Signal(*commandQueue);
        
        swapChain->Present(vSync, allowTearing);
        
        currentBackBufferIndex = swapChain->GetCurrentBackBufferIndex();
        fence->WaitForFenceValue(frameFenceValues[currentBackBufferIndex]);
    }
}

void Sample::OnDestroy()
{
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    fence->Flush(*commandQueue);
    CloseHandle(fence->GetEvent());
}

void Sample::Resize(uint32_t width, uint32_t height)
{
    if (this->width != width || this->height != height)
    {
        // don't allow 0 size swap chain back buffers
        this->width = std::max(1u, width);
        this->height = std::max(1u, height);

        // flush the GPU queue to make sure the swap chain's back buffers
        // are not being referenced by an in-flight command list
        fence->Flush(*commandQueue);

        for (int i = 0; i < SwapChain::FrameCount; ++i)
        {
            // Any references to the back buffers must be released
            // before the swap chain can be resized
            swapChain->ResetBackBuffer(i);
            frameFenceValues[i] = frameFenceValues[swapChain->GetCurrentBackBufferIndex()];
        }

        swapChain->Resize(this->width, this->height);
        swapChain->UpdateRenderTargetViews(*device, *RTVdescriptorHeap);

        viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<float>(this->width), static_cast<float>(this->height));
        resizeDepthBuffer(this->width, this->height);
    }
}

void Sample::SetupPipeline()
{
    // create the descriptor heap for the depth-stencil view
    DSVdescriptorHeap = std::make_unique<DescriptorHeap>(*device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1);

    // load shaders
    std::wstring directoryPath = GetExecutableDirectory();
    Shader vertexShader(directoryPath, L"VertexShader", VertexShader);
    Shader pixelShader(directoryPath, L"PixelShader", PixelShader);

    // create the vertex input layout
    D3D12_INPUT_ELEMENT_DESC inputLayout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    };

    rootSignature->Create(*device);

    // describe and create the graphics pipeline state object (PSO)
    {
        D3D12_DEPTH_STENCIL_DESC depthStencilDesc = {};
        depthStencilDesc.DepthEnable = TRUE;
        depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
        depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
        depthStencilDesc.StencilEnable = FALSE;

        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDescriptor = {};
        psoDescriptor.InputLayout = { inputLayout, _countof(inputLayout) };
        psoDescriptor.pRootSignature = rootSignature->GetPtr();
        psoDescriptor.VS = vertexShader.GetByteCode();
        psoDescriptor.PS = pixelShader.GetByteCode();
        psoDescriptor.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        
        //psoDescriptor.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
        //psoDescriptor.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;

        psoDescriptor.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        psoDescriptor.SampleMask = UINT_MAX;
        psoDescriptor.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        psoDescriptor.NumRenderTargets = 1;
        psoDescriptor.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
        psoDescriptor.DSVFormat = DXGI_FORMAT_D32_FLOAT;
        psoDescriptor.SampleDesc.Count = 1;
        psoDescriptor.DepthStencilState = depthStencilDesc;

        pipelineStateObject = std::make_unique<PipelineStateObject>(*device, psoDescriptor);
    }

    // close and execute command list to send data to gpu
    commandList->Close();
    ID3D12CommandList* const commandLists[] = { commandList->GetPtr() };
    commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);    
    
    // wait for the end of the command list execution
    fence->Flush(*commandQueue);

    // resize/create the depth buffer
    resizeDepthBuffer(width, height);
    initImGui(hWnd);
}

void Sample::ToggleVSync()
{
    vSync = !vSync;
}

void Sample::UpdateBufferResource(Resource& destinationResource, Resource& intermediateResource,
    size_t elementsCount, size_t elementSize, const void* bufferData,
    D3D12_RESOURCE_FLAGS flags) const
{
    size_t bufferSize = elementsCount * elementSize;

    ComPtr<ID3D12Resource> pDestinationResource = destinationResource.Get();
    ComPtr<ID3D12Resource> pIntermediateResource = intermediateResource.Get();

    // create a committed resource for the GPU resource in a default heap
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

    // create a committed resource for the upload
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

void Sample::enableDebugLayer()
{
#if defined(_DEBUG)
    // always enable the debug layer before doing anything DX12 related
    // so all possible errors generated while creating DX12 objects
    // are caught by the debug layer
    ComPtr<ID3D12Debug> debugInterface;
    ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface)));
    debugInterface->EnableDebugLayer();
#endif
}

void Sample::initPipeline()
{
	device = std::make_unique<Device>(useWarp);
	commandQueue = std::make_unique<CommandQueue>(*device);
	swapChain = std::make_unique<SwapChain>(hWnd, *commandQueue, width, height, allowTearing);
    RTVdescriptorHeap = std::make_unique<DescriptorHeap>(*device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, SwapChain::FrameCount);
    SRVdescriptorHeap = std::make_unique<DescriptorHeap>(*device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, SwapChain::FrameCount, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);

    swapChain->UpdateRenderTargetViews(*device, *RTVdescriptorHeap);

    for (int i = 0; i < SwapChain::FrameCount; ++i)
        commandAllocators[i] = std::make_unique<CommandAllocator>(*device, D3D12_COMMAND_LIST_TYPE_DIRECT);

    commandList = std::make_unique<CommandList>(*device, *commandAllocators[swapChain->GetCurrentBackBufferIndex()], D3D12_COMMAND_LIST_TYPE_DIRECT);
    fence = std::make_unique<Fence>(*device);
    rootSignature = std::make_unique<RootSignature>();
}

void Sample::initImGui(HWND hWnd)
{
    // create context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // imgui style
    ImGui::StyleColorsDark();

    // init win32 and dx12 backends
    ImGui_ImplWin32_Init(hWnd);
    ImGui_ImplDX12_Init(device->GetPtr(), SwapChain::FrameCount,
        DXGI_FORMAT_R8G8B8A8_UNORM, SRVdescriptorHeap->GetPtr(),
        SRVdescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
        SRVdescriptorHeap->GetGPUDescriptorHandleForHeapStart());
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

void Sample::preRender()
{
    UINT currentBackBufferIndex = swapChain->GetCurrentBackBufferIndex();
    Resource backBuffer = swapChain->GetCurrentBackBuffer();
    auto rtv = RTVdescriptorHeap->GetRenderTargetView(currentBackBufferIndex);
    auto dsv = DSVdescriptorHeap->GetCPUDescriptorHandleForHeapStart();

    commandAllocators[currentBackBufferIndex]->Reset();
    commandList->Reset(*commandAllocators[currentBackBufferIndex], *pipelineStateObject);

    commandList->ClearRenderTargets(backBuffer, rtv, dsv, clearColor);

    commandList->SetPipelineState(*pipelineStateObject);
    commandList->SetGraphicsRootSignature(*rootSignature);

    // prepare pipeline stages
    commandList->PrepareRasterizerStage(viewport, scissorRect);
    commandList->PrepareOutputMergerStage(&rtv, &dsv);

    ID3D12DescriptorHeap* descriptorHeaps[] = { SRVdescriptorHeap->GetPtr() };
    commandList->Get()->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
}

void Sample::resizeDepthBuffer(int width, int height)
{
    width = std::max(1, width);
    height = std::max(1, height);

    // resize screen dependent resources
    // create a depth buffer
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

    // update the depth-stencil view
    D3D12_DEPTH_STENCIL_VIEW_DESC dsv = {};
    dsv.Format = DXGI_FORMAT_D32_FLOAT;
    dsv.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    dsv.Texture2D.MipSlice = 0;
    dsv.Flags = D3D12_DSV_FLAG_NONE;

    device->Get()->CreateDepthStencilView(pDepthBuffer.Get(), &dsv,
        DSVdescriptorHeap->GetCPUDescriptorHandleForHeapStart());
}

#pragma endregion