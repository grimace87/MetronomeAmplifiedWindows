#pragma once

#include "../BaseShader.h"
#include "../../ShaderStructures.h"

namespace shader
{
    class FontTransformShader : public BaseShader {
    public:
        FontTransformShader();
        void SetTransform(DirectX::XMMATRIX& transformMatrix);
        void SetPaintColor(float r, float g, float b, float a);
    protected:
        std::vector<D3D11_INPUT_ELEMENT_DESC> makeInputDescription() override;
        bool VertexShaderUsesConstantBuffer() override;
        bool PixelShaderUsesConstantBuffer() override;
        UINT GetConstantBufferSize() override;
        void* GetConstantBufferData() override;
    private:
        structures::TransformPaintColorConstantBuffer m_constantBufferData;
    };
}
