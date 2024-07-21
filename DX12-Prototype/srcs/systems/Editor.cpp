#include "PrecompiledHeaders.h"
#include "systems/Editor.h"

#include "data/geometry/Vertex.h"
#include "helpers/Time.h"
#include "maths/Maths.h"
#include "objects/meshes/Mesh.h"
#include "pipeline/CommandList.h"
#include "pipeline/RootSignature.h"

// testing variables
static Vertex g_Vertices[24] = {
    // back face
    { Float3(-1.0f, -1.0f, -1.0f), Float3(0.78f, 0.549f, 0.941f), Float3(0.0f, 0.0f, -1.0f) }, // 0
    { Float3(-1.0f,  1.0f, -1.0f), Float3(0.78f, 0.549f, 0.941f), Float3(0.0f, 0.0f, -1.0f) }, // 1
    { Float3(1.0f,  1.0f, -1.0f), Float3(0.78f, 0.549f, 0.941f), Float3(0.0f, 0.0f, -1.0f) }, // 2
    { Float3(1.0f, -1.0f, -1.0f), Float3(0.78f, 0.549f, 0.941f), Float3(0.0f, 0.0f, -1.0f) }, // 3

    // front face
    { Float3(-1.0f, -1.0f,  1.0f), Float3(0.78f, 0.549f, 0.941f), Float3(0.0f, 0.0f, 1.0f) }, // 4
    { Float3(-1.0f,  1.0f,  1.0f), Float3(0.78f, 0.549f, 0.941f), Float3(0.0f, 0.0f, 1.0f) }, // 5
    { Float3(1.0f,  1.0f,  1.0f), Float3(0.78f, 0.549f, 0.941f), Float3(0.0f, 0.0f, 1.0f) }, // 6
    { Float3(1.0f, -1.0f,  1.0f), Float3(0.78f, 0.549f, 0.941f), Float3(0.0f, 0.0f, 1.0f) }, // 7

    // left face
    { Float3(-1.0f, -1.0f, -1.0f), Float3(0.78f, 0.549f, 0.941f), Float3(-1.0f, 0.0f, 0.0f) }, // 8
    { Float3(-1.0f,  -1.0f, 1.0f), Float3(0.78f, 0.549f, 0.941f), Float3(-1.0f, 0.0f, 0.0f) }, // 9
    { Float3(-1.0f,  1.0f,  1.0f), Float3(0.78f, 0.549f, 0.941f), Float3(-1.0f, 0.0f, 0.0f) }, // 10
    { Float3(-1.0f, 1.0f,  -1.0f), Float3(0.78f, 0.549f, 0.941f), Float3(-1.0f, 0.0f, 0.0f) }, // 11

    // right face
    { Float3(1.0f, -1.0f, -1.0f), Float3(0.78f, 0.549f, 0.941f), Float3(1.0f, 0.0f, 0.0f) }, // 12
    { Float3(1.0f,  1.0f, -1.0f), Float3(0.78f, 0.549f, 0.941f), Float3(1.0f, 0.0f, 0.0f) }, // 13
    { Float3(1.0f,  1.0f,  1.0f), Float3(0.78f, 0.549f, 0.941f), Float3(1.0f, 0.0f, 0.0f) }, // 14
    { Float3(1.0f, -1.0f,  1.0f), Float3(0.78f, 0.549f, 0.941f), Float3(1.0f, 0.0f, 0.0f) }, // 15

    // up face
    { Float3(-1.0f,  1.0f, -1.0f), Float3(0.78f, 0.549f, 0.941f), Float3(0.0f, 1.0f, 0.0f) }, // 16
    { Float3(-1.0f,  1.0f,  1.0f), Float3(0.78f, 0.549f, 0.941f), Float3(0.0f, 1.0f, 0.0f) }, // 17
    { Float3(1.0f,  1.0f,  1.0f), Float3(0.78f, 0.549f, 0.941f), Float3(0.0f, 1.0f, 0.0f) }, // 18
    { Float3(1.0f,  1.0f, -1.0f), Float3(0.78f, 0.549f, 0.941f), Float3(0.0f, 1.0f, 0.0f) }, // 19

    // down face
    { Float3(-1.0f, -1.0f, -1.0f), Float3(0.78f, 0.549f, 0.941f), Float3(0.0f, -1.0f, 0.0f) }, // 20
    { Float3(1.0f, -1.0f,  -1.0f), Float3(0.78f, 0.549f, 0.941f), Float3(0.0f, -1.0f, 0.0f) }, // 21
    { Float3(1.0f, -1.0f,  1.0f), Float3(0.78f, 0.549f, 0.941f), Float3(0.0f, -1.0f, 0.0f) }, // 22
    { Float3(-1.0f, -1.0f, 1.0f), Float3(0.78f, 0.549f, 0.941f), Float3(0.0f, -1.0f, 0.0f) }  // 23
};

static WORD g_Indices[36] = {
    0, 1, 2, 0, 2, 3,
    4, 6, 5, 4, 7, 6,
    8, 9, 10, 8, 10, 11,
    12, 13, 14, 12, 14, 15,
    16, 17, 18, 16, 18, 19,
    20, 21, 22, 20, 22, 23
};

#pragma region Public Methods

Editor::Editor(uint32_t _width, uint32_t _height)
	: width(_width)
	, height(_height)
{
	sample = std::make_unique<Sample>(width, height);
    camera = std::make_unique<Camera>(Vector({ 0, 0, -20, 1 }));

    //test
    std::vector<Vertex> vertices;
    for (int i = 0; i < 24; i++)
        vertices.push_back(g_Vertices[i]);

    std::vector<WORD> indices;
    for (int i = 0; i < 36; i++)
        indices.push_back(g_Indices[i]);

    mesh = Mesh(vertices, indices);
}

void Editor::OnInit(HWND hWnd)
{
	sample->OnInit(hWnd);
    setupRootSignature();

    // init data (entities later) OnInit method?
    mesh.UpdateBuffersResource(*sample);

    sample->SetupPipeline();
}

void Editor::OnUpdate()
{
	sample->OnUpdate();
}

void Editor::OnRender()
{
    render();
    sample->OnRender();
}

void Editor::OnDestroy()
{
	sample->OnDestroy();
}

void Editor::ProcessCameraInputs(float x, float y, float z, bool accelerate)
{
    camera->ProcessInputs(x, y, z, accelerate);
}

void Editor::ProcessCameraMouseMovement(float xOffset, float yOffset)
{
    camera->ProcessMouseMovement(xOffset, yOffset);
}

void Editor::ProcessCameraMouseScroll(float offset)
{
    camera->ProcessMouseScroll(offset);
}

void Editor::Resize(uint32_t _width, uint32_t _height)
{
    width = _width;
    height = _height;

    sample->Resize(width, height);
}

void Editor::ToggleVSync()
{
    sample->ToggleVSync();
}

#pragma endregion

#pragma region Private Methods

void Editor::render()
{
    CommandList& commandList = sample->GetCommandList();

    Matrix4 view = camera->GetViewMatrix();
    Matrix4 proj = camera->GetProjectionMatrix(width, height);
    Vector cameraPos = camera->GetPosition();
    Matrix4 mvpMatrix = XMMatrixMultiply(DirectX::XMMatrixIdentity(), view);
    mvpMatrix = XMMatrixMultiply(mvpMatrix, proj);
    commandList.Get()->SetGraphicsRoot32BitConstants(0, sizeof(Matrix4) / 4, &mvpMatrix, 0);
    commandList.Get()->SetGraphicsRoot32BitConstants(2, sizeof(Vector) / 4, &cameraPos, 0);

    mesh.OnRender(commandList);
    mesh2.OnRender(commandList);
    mesh3.OnRender(commandList);

    // ImGui
    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("DX-12 Prototype");
    ImGui::Text("Delta Time: %.1f ms", Time::GetDeltaTime() * 1000);
    ImGui::Text("FPS: %.1f", Time::GetFrameRate());
    //ImGui::Text("FOV: %.1f", camera.GetFOV());
    ImGui::End();
}

void Editor::setupRootSignature()
{
    RootSignature& rootSignature = sample->GetRootSignature();

    rootSignature.Add32BitConstant(sizeof(Matrix4) / 4, 0);
    rootSignature.Add32BitConstant(sizeof(Matrix4) / 4, 1);
    rootSignature.Add32BitConstant(sizeof(Matrix4) / 4, 2);
}

#pragma endregion