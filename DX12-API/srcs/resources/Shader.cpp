#include "PrecompiledHeaders.h"
#include "resources/Shader.h"

#pragma region Public Methods

Shader::Shader(std::wstring _path, std::wstring _filename, ShaderType _type)
	: path(_path)
    , filename(_filename)
	, type(_type)
{
	readFromFile();
}

#pragma region Getters

const ComPtr<ID3DBlob>& Shader::Get() const
{
	return blob;
}

ID3DBlob* Shader::GetPtr() const
{
	return blob.Get();
}

CD3DX12_SHADER_BYTECODE Shader::GetByteCode() const
{
    return CD3DX12_SHADER_BYTECODE(blob.Get());
}

#pragma endregion

void Shader::Compile(std::wstring path)
{
    // path is given as param because .cso file may not be in the same
    // place than .hlsl file (Compile need an .hlsl)

    ComPtr<ID3DBlob> errorBlob;

    const char* target = type == VertexShader ? vertexShaderTarget : type == PixelShader ? pixelShaderTarget : computeShaderTarget;

    HRESULT hr = D3DCompileFromFile(
        path.c_str(),
        nullptr,                      // compilation params
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        entryPoint,                   // shader's entry point
        target,                       // compilation target (ex : "vs_5_1" for vertex shader)
        D3DCOMPILE_ENABLE_STRICTNESS, // warning as error
        0,
        &blob,                        // compilated blob
        &errorBlob);                  // error message blob

    if (FAILED(hr)) 
    {
        if (errorBlob) 
        {
            OutputDebugStringA((char*)errorBlob->GetBufferPointer());
        }
        throw std::runtime_error("Shader compilation failed!");
    }
}

#pragma endregion

#pragma region Private Methods

void Shader::readFromFile()
{
    std::wstring filePath = path + L"\\" + filename + L".cso";
    ThrowIfFailed(D3DReadFileToBlob(filePath.c_str(), &blob));
}

#pragma endregion