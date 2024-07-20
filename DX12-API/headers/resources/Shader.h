#pragma once

#include "helpers/Helpers.h"

enum ShaderType
{
	VertexShader,
	PixelShader,
	ComputeShader
};

class Shader
{
public:
	// TODO maybe take .cso path and .hlsl path as params
	// currently it's only .cso file path
	// filename is without extension (without .hlsl or .cso)
	Shader(std::wstring path, std::wstring filename, ShaderType type);

	// getters
	const ComPtr<ID3DBlob>& Get() const;
	ID3DBlob* GetPtr() const;
	CD3DX12_SHADER_BYTECODE GetByteCode() const;

	// not used yet, but this method will be usefull
	// when hot shader compilation will be available
	void Compile(std::wstring path);

private:
	// read precompilated shader file (.cso)
	void readFromFile();

private:
	// compilation constants
	static constexpr const char* entryPoint = "main";
	static constexpr const char* vertexShaderTarget = "vs_5_1";
	static constexpr const char* pixelShaderTarget = "ps_5_1";
	static constexpr const char* computeShaderTarget = "cs_5_1";

	std::wstring path;
	std::wstring filename;
	ShaderType type;

	ComPtr<ID3DBlob> blob;
};