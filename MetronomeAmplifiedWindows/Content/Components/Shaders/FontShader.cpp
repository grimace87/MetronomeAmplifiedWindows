#include "pch.h"
#include "FontShader.h"

shader::FontShader::FontShader() : BaseShader(L"FontVertexShader.cso", L"FontPixelShader.cso")
{
}

std::vector<D3D11_INPUT_ELEMENT_DESC> shader::FontShader::makeInputDescription()
{
	return {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
}

bool shader::FontShader::HasConstantBuffer()
{
	return true;
}

UINT shader::FontShader::GetConstantBufferSize()
{
	return sizeof(structures::PaintColorConstantBuffer);
}

void* shader::FontShader::GetConstantBufferData()
{
	return &m_paintColorData;
}

void shader::FontShader::SetPaintColor(float r, float g, float b, float a)
{
	m_paintColorData.color.x = r;
	m_paintColorData.color.y = g;
	m_paintColorData.color.z = b;
	m_paintColorData.color.w = a;
}
