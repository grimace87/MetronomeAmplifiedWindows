#include "pch.h"
#include "AlphaTextureShader.h"

shader::AlphaTexture::AlphaTexture() : BaseShader(L"AlphaTextureVertexShader.cso", L"AlphaTexturePixelShader.cso")
{
}

std::vector<D3D11_INPUT_ELEMENT_DESC> shader::AlphaTexture::makeInputDescription()
{
	return {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
}

bool shader::AlphaTexture::HasConstantBuffer()
{
	return false;
}

UINT shader::AlphaTexture::GetConstantBufferSize()
{
	return 0;
}

void* shader::AlphaTexture::GetConstantBufferData()
{
	return nullptr;
}
