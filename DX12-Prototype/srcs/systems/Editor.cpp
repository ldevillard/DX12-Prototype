#include "PrecompiledHeaders.h"
#include "systems/Editor.h"

#include "data/geometry/Vertex.h"
#include "helpers/Time.h"
#include "maths/Maths.h"
#include "objects/meshes/Mesh.h"
#include "pipeline/CommandList.h"
#include "pipeline/RootSignature.h"

#pragma region Public Methods

Editor::Editor(uint32_t _width, uint32_t _height)
	: width(_width)
	, height(_height)
{
	sample = std::make_unique<Sample>(width, height);
    camera = std::make_unique<Camera>(Vector({ 0, 0, -20, 1 }));

    //models.push_back(Model("C:\\Users\\logan\\Desktop\\DX12-Prototype\\x64\\Debug\\resources\\models\\dragon.fbx"));
    //models.push_back(Model("C:\\Users\\logan\\Desktop\\DX12-Prototype\\x64\\Debug\\resources\\models\\pokemon.obj"));
    //models.push_back(Model("C:\\Users\\logan\\Desktop\\DX12-Prototype\\x64\\Debug\\resources\\models\\temple.obj"));
    models.push_back(Model("C:\\Users\\logan\\Desktop\\DX12-Prototype\\x64\\Debug\\resources\\models\\sax.obj"));
    //models.push_back(Model(ModelPrimitive::CubePrimitive));
    //models.push_back(Model(ModelPrimitive::CubePrimitive));
    //models.push_back(Model(ModelPrimitive::CubePrimitive));
}

void Editor::OnInit(HWND hWnd)
{
	sample->OnInit(hWnd);
    setupRootSignature();

    // init data (entities later)
    for (Model& model : models)
        model.OnInit(*sample);

    sample->SetupPipeline();
}

void Editor::OnUpdate()
{
	sample->OnUpdate();
    camera->OnUpdate(width, height);

    // movement for testing
    //float angle = static_cast<float>(sin(Time::GetTimeElapsed()) * 200);
    //const Vector rotationAxis = DirectX::XMVectorSet(0, 1, 1, 0);
    //Matrix4 rotation = DirectX::XMMatrixRotationAxis(rotationAxis, DirectX::XMConvertToRadians(angle));
    //for (int i = 0; i < models.size(); i++)
    //{
    //    models[i].SetTransform(rotation * DirectX::XMMatrixTranslation(-6.0f * (static_cast<float>(i) - 1.0f), 0, 0));
    //    models[i].OnUpdate();
    //}
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

    commandList.Get()->SetGraphicsRoot32BitConstants(1, sizeof(Matrix4) / 4, &view, 0);
    commandList.Get()->SetGraphicsRoot32BitConstants(2, sizeof(Matrix4) / 4, &proj, 0);
    commandList.Get()->SetGraphicsRoot32BitConstants(3, sizeof(Vector) / 4, &cameraPos, 0);

    for (Model& model : models)
        model.OnRender(commandList);

    // ImGui
    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("DX-12 Prototype");
    ImGui::Text("Delta Time: %.1f ms", Time::GetDeltaTime() * 1000);
    ImGui::Text("FPS: %.1f", Time::GetFrameRate());
    ImGui::Text("FOV: %.1f", camera->GetFOV());
    ImGui::End();
}

void Editor::setupRootSignature()
{
    RootSignature& rootSignature = sample->GetRootSignature();

    rootSignature.Add32BitConstant(sizeof(Matrix4) / 4, 0);
    rootSignature.Add32BitConstant(sizeof(Matrix4) / 4, 1);
    rootSignature.Add32BitConstant(sizeof(Matrix4) / 4, 2);
    rootSignature.Add32BitConstant(sizeof(Vector) / 4, 3);
}

#pragma endregion