#include "pch.h"
#include "AlphaTextureTransformShader.h"

shader::AlphaTextureTransformShader::AlphaTextureTransformShader() : BaseShader(L"AlphaTextureTransformVertexShader.cso", L"AlphaTextureTransformPixelShader.cso")
{
}

std::vector<D3D11_INPUT_ELEMENT_DESC> shader::AlphaTextureTransformShader::makeInputDescription()
{
    return {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
}

bool shader::AlphaTextureTransformShader::VertexShaderUsesConstantBuffer()
{
    return true;
}

bool shader::AlphaTextureTransformShader::PixelShaderUsesConstantBuffer()
{
    return false;
}

UINT shader::AlphaTextureTransformShader::GetConstantBufferSize()
{
    return sizeof(structures::TransformConstantBuffer);
}

void* shader::AlphaTextureTransformShader::GetConstantBufferData()
{
    return &m_constantBufferData;
}

void shader::AlphaTextureTransformShader::SetTransform(DirectX::XMMATRIX& transformMatrix)
{
    m_constantBufferData.transform = transformMatrix;
}
