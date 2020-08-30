#include "pch.h"
#include "FontTransformShader.h"

shader::FontTransformShader::FontTransformShader() : BaseShader(L"FontTransformVertexShader.cso", L"FontTransformPixelShader.cso")
{
}

std::vector<D3D11_INPUT_ELEMENT_DESC> shader::FontTransformShader::makeInputDescription()
{
    return {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
}

bool shader::FontTransformShader::VertexShaderUsesConstantBuffer()
{
    return true;
}

bool shader::FontTransformShader::PixelShaderUsesConstantBuffer()
{
    return true;
}

UINT shader::FontTransformShader::GetConstantBufferSize()
{
    return sizeof(structures::TransformPaintColorConstantBuffer);
}

void* shader::FontTransformShader::GetConstantBufferData()
{
    return &m_constantBufferData;
}

void shader::FontTransformShader::SetTransform(DirectX::XMMATRIX& transformMatrix)
{
    m_constantBufferData.transform = transformMatrix;
}

void shader::FontTransformShader::SetPaintColor(float r, float g, float b, float a)
{
    m_constantBufferData.color.x = r;
    m_constantBufferData.color.y = g;
    m_constantBufferData.color.z = b;
    m_constantBufferData.color.w = a;
}
